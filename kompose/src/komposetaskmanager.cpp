/***************************************************************************
 *   Copyright (C) 2004 by Hans Oischinger                                 *
 *   oisch@sourceforge.net                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "komposetaskmanager.h"

#include "komposelayout.h"
#include "komposedesktopwidget.h"
#include "komposesettings.h"
#include "komposetaskwidget.h"
#include "komposefullscreenwidget.h"
#include "komposeglobal.h"

#include <kwinmodule.h>
#include <netwm.h>
#include <kwin.h>
#include <kapplication.h>

#include <qtimer.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qapplication.h>

#ifdef COMPOSITE
 #include <X11/extensions/Xrender.h>
 #include <X11/extensions/Xcomposite.h>
#endif

static KomposeTaskManager* taskManagerInstance = 0;


/*
 * Taskmanager is a singleton
 */
KomposeTaskManager* KomposeTaskManager::instance()
{
  if ( !taskManagerInstance )
  {
    qDebug("KomposeTaskManager::instance() - Creating Singleton instance");
    taskManagerInstance = new KomposeTaskManager();
  }
  return taskManagerInstance;
}


KomposeTaskManager::KomposeTaskManager()
    : QObject(), DCOPObject( "KomposeTaskMgrDcopIface" ),
    viewWidget(),
    activeView(0)
{
  taskManagerInstance = this;
  qDebug("KomposeTaskManager::KomposeTaskManager()");

#ifdef COMPOSITE
  // Enable offscreen rendering for all Rootwins
  Display *dpy = QPaintDevice::x11AppDisplay();
  for ( int i = 0; i < ScreenCount( dpy ); i++ )
    XCompositeRedirectSubwindows( dpy, RootWindow( dpy, i ),
                                  CompositeRedirectAutomatic );
  // End of XComposite stuff
#endif  
  
  kwinmodule = new KWinModule(this, 2);
  numDesks = KWin::numberOfDesktops();

  // Added and removed wil alwasy be connected
  connect( kwinmodule, SIGNAL(windowAdded(WId)), this, SLOT(slotWindowAdded(WId)) );
  connect( kwinmodule, SIGNAL(windowRemoved(WId)), this, SLOT(slotWindowRemoved(WId)) );

  connect( kwinmodule, SIGNAL(numberOfDesktopsChanged(int)), this, SLOT(slotDesktopCountChanged(int)) );
  // The other slots depend on user's settings
  connect( KomposeSettings::instance(), SIGNAL(settingsChanged()), this, SLOT(slotStartWindowListeners()) );

  // register existing windows
  const QValueList<WId> windows = kwinmodule->windows();
  for (QValueList<WId>::ConstIterator it = windows.begin(); it != windows.end(); ++it )
    slotWindowAdded(*it);

  if (KomposeSettings::instance()->getPassiveScreenshots())
  {
    qDebug("KomposeTaskManager::KomposeTaskManager() - Grabbing Screenshots passively");
    connect( kwinmodule, SIGNAL(activeWindowChanged(WId)), this, SLOT(slotUpdateScreenshot(WId)) );
  }
  else
  {
    qDebug("KomposeTaskManager::KomposeTaskManager() - Passive Screenshots disabled");
    disconnect( kwinmodule, SIGNAL(activeWindowChanged(WId)), this, SLOT(slotUpdateScreenshot(WId)) );
  }
}

KomposeTaskManager::~KomposeTaskManager()
{
  //   delete taskManagerInstance;
}


void KomposeTaskManager::slotStartWindowListeners()
{
  connect( kwinmodule, SIGNAL(windowChanged( WId, unsigned int )), this,
           SLOT(slotWindowChanged( WId, unsigned int )) );
}

KomposeTask* KomposeTaskManager::findTask(WId w)
{
  for (KomposeTask* t = tasklist.first(); t != 0; t = tasklist.next())
    if (t->window() == w )
      return t;
  return 0;
}

void KomposeTaskManager::slotWindowChanged( WId w, unsigned int dirty)
{
  if( dirty & NET::WMState )
  {
    NETWinInfo info ( qt_xdisplay(),  w, qt_xrootwin(), NET::WMState );
    if ( (info.state() & NET::SkipTaskbar) != 0 )
    {
      slotWindowRemoved( w );
      return;
    }
    else
    {
      if( !findTask( w ))
        slotWindowAdded( w );
    }
  }

  // check if any state we are interested in is marked dirty
  if(!(dirty & (NET::WMVisibleName|NET::WMName|NET::WMState|NET::WMIcon|NET::XAWMState|NET::WMDesktop)) )
    return;

  // find task
  KomposeTask* t = findTask( w );
  if (!t) return;

  //kdDebug() << "TaskManager::windowChanged " << w << " " << dirty << endl;


  // refresh icon pixmap if necessary
  //   if (dirty & NET::WMIcon)
  //     t->refresh(true);
  //   else
  //     t->refresh();

  if(dirty & (NET::WMDesktop|NET::WMState|NET::XAWMState))
    t->refresh();// moved to different desktop or is on all or change in iconification/withdrawnnes
}


void KomposeTaskManager::slotWindowRemoved(WId w )
{
  // find task
  KomposeTask* task = findTask( w );
  if (!task) return;

  //qDebug("KomposeTaskManager::slotWindowRemoved(WId w ) - Removing task %s", task->visibleNameWithState());
  tasklist.remove( task );
  delete task;
}

void KomposeTaskManager::slotWindowAdded(WId w )
{
  // ignore myself
  if ( viewWidget && w == viewWidget->winId() )
  {
    return;
  }

  KWin::WindowInfo info = KWin::windowInfo(w);

  // ignore NET::Tool and other special window types
  if (info.windowType(NET::AllTypesMask) != NET::Normal
      && info.windowType(NET::AllTypesMask) != NET::Override
      && info.windowType(NET::AllTypesMask) != NET::Unknown
      && info.windowType(NET::AllTypesMask) != NET::Dialog)
    return;

  // ignore windows that want to be ignored by the taskbar
  if ((info.state() & NET::SkipTaskbar) != 0)
  {
    return;
  }

  if ( !info.valid() )
    return;
    
  KomposeTask* t = new KomposeTask(w, kwinmodule, this);
  tasklist.append(t);

  emit newTask( t );

  qDebug("KomposeTaskManager - KomposeTask added for WId: %d", w);
}


/*
 * Updates the screenshots for all apps
 */
void KomposeTaskManager::slotUpdateScreenshots()
{
  qDebug("KomposeTaskManager::slotUpdateScreenshots()");

  QPtrListIterator<KomposeTask> it( tasklist );
  KomposeTask *task;

  // Disable passive screenshots temporarily as we want to force screenshots now
  bool passiveScreenshots = KomposeSettings::instance()->getPassiveScreenshots();
  KomposeSettings::instance()->setPassiveScreenshots( false );

  while ( (task = it.current()) != 0 )
  {
    ++it;
    task->updateScreenshot();
  }

  if ( passiveScreenshots )
    KomposeSettings::instance()->setPassiveScreenshots( true );
}

/*
 * Updates the screenshot for the specified app
 */
void KomposeTaskManager::slotUpdateScreenshot(WId winId)
{
  qDebug("KomposeTaskManager::slotUpdateScreenshot( %d )", winId);
  QPtrListIterator<KomposeTask> it( tasklist );
  KomposeTask *task;
  while ( (task = it.current()) != 0 )
  {
    ++it;
    if ( winId == task->window() )
    {
      task->updateScreenshot();
      return;
    }
  }
}


void KomposeTaskManager::createView( int type )
{
  if (type == -1)
    type = KomposeSettings::instance()->getDefaultView();

  switch(type)
  {
  case KOMPOSEDISPLAY_VIRTUALDESKS:
    createVirtualDesktopView();
    break;
  case KOMPOSEDISPLAY_WORLD:
    createWorldView();
    break;
  }
}


void KomposeTaskManager::createVirtualDesktopView()
{
  // Set activeView to false during this funcion as it will be checked by the layout
  int tmp_activeview = activeView;
  activeView = false;

  if ( !tmp_activeview  )
  {
    // Remember current desktop
    deskBeforeSnaps = KWin::currentDesktop();
    slotUpdateScreenshots();
  }

  qDebug("KomposeTaskManager::createVirtualDesktopView - Creating View");

  if ( !tmp_activeview  )
    viewWidget = new KomposeFullscreenWidget( KOMPOSEDISPLAY_VIRTUALDESKS );
  else
    viewWidget->setType( KOMPOSEDISPLAY_VIRTUALDESKS );

  KWin::forceActiveWindow( viewWidget->winId() );

  activeView = true;

  slotStartWindowListeners();
}


void KomposeTaskManager::createWorldView()
{
  // Set activeView to false during this funcion as it will be checked by the layout
  int tmp_activeview = activeView;
  activeView = false;

  if ( !tmp_activeview )
  {
    // Remember current desktop
    deskBeforeSnaps = KWin::currentDesktop();
    slotUpdateScreenshots();
  }

  qDebug("KomposeTaskManager::createWorldView - Creating View");

  if ( !tmp_activeview )
    viewWidget = new KomposeFullscreenWidget( KOMPOSEDISPLAY_WORLD );
  else
    viewWidget->setType( KOMPOSEDISPLAY_WORLD );

  KWin::forceActiveWindow( viewWidget->winId() );

  activeView = true;

  slotStartWindowListeners();
}


void KomposeTaskManager::closeCurrentView()
{
  if ( !activeView )
    return;

  activeView = false;

  viewWidget->setUpdatesEnabled( false );
  viewWidget->close(true);
  viewWidget = 0;

  emit viewClosed();

  if ( KomposeGlobal::instance()->getSingleShot() )
    kapp->quit();

  // Reset old Desktop
  KWin::setCurrentDesktop( deskBeforeSnaps );
}

void KomposeTaskManager::slotDesktopCountChanged(int d)
{
  numDesks = d;
}



bool KomposeTaskManager::isOnTop(const KomposeTask* task)
{
  if(!task) return false;

  for (QValueList<WId>::ConstIterator it = kwinmodule->stackingOrder().fromLast();
       it != kwinmodule->stackingOrder().end(); --it )
  {
    for (KomposeTask* t = tasklist.first(); t != 0; t = tasklist.next() )
    {
      if ( (*it) == t->window() )
      {
        if ( t == task )
          return true;
        if ( !t->isIconified() && (t->isAlwaysOnTop() == task->isAlwaysOnTop()) )
          return false;
        break;
      }
    }
  }
  return false;
}

void KomposeTaskManager::setCurrentDesktop( int desknum )
{
  closeCurrentView();
  KWin::setCurrentDesktop(desknum+1);
}

void KomposeTaskManager::activateTask( KomposeTask *task )
{
  closeCurrentView();
  task->activate();
}

void KomposeTaskManager::createDefaultView()
{
  createView();
}

// bool KomposeTaskManager::process(const QCString &fun, const QByteArray &data, QCString &replyType, QByteArray &replyData)
// {}
// {
//   if (fun == "createDefaultView()")
//   {
// //     QString result = createView();
// //     QDataStream reply(replyData, IO_WriteOnly);
// //     reply << result;
// //     replyType = "QString";
//      createView();
//      return true;
//   }
//   else
//   {
//     qDebug("unknown function call to KomposeTaskManager::process()");
//     return false;
//   }
// }

#include "komposetaskmanager.moc"
