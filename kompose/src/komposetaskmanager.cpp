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
// #include "komposeglfullscreenwidget.h"

#include <kwinmodule.h>
#include <netwm.h>
#include <kwin.h>

#include <qtimer.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qapplication.h>

static KomposeTaskManager* taskManagerInstance = 0;


/*
 * Taskmanager is a singleton
 */
KomposeTaskManager* KomposeTaskManager::instance()
{
  if ( !taskManagerInstance )
  {
    qDebug("KomposeTaskManager::instance() - Creating Singleton instance");
    KomposeTaskManager *taskManagerInstance = new KomposeTaskManager();
  }
  return taskManagerInstance;
}


KomposeTaskManager::KomposeTaskManager()
    : QObject(),
    viewWidget(),
    activeView(0)
{
  taskManagerInstance = this;
  kwinmodule = new KWinModule();
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
    connect( kwinmodule, SIGNAL(activeWindowChanged(WId)), this, SLOT(slotUpdateScreenshot(WId)) );
  else
    disconnect( kwinmodule, SIGNAL(activeWindowChanged(WId)), this, SLOT(slotUpdateScreenshot(WId)) );
}

KomposeTaskManager::~KomposeTaskManager()
{
  delete taskManagerInstance;
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
      //_skiptaskbar_windows.push_front( w );
      return;
    }
    else
    {
      //_skiptaskbar_windows.remove( w );
      if( !findTask( w ))
        slotWindowAdded( w ); // skipTaskBar state was removed, so add this window
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
  delete task;  // FIXME: Always segfaulting when trying to do this :(
}

void KomposeTaskManager::slotWindowAdded(WId w )
{
  // ignore myself
  if ( viewWidget && w == viewWidget->winId() )
  {
    return;
  }

  NETWinInfo info (qt_xdisplay(),  w, qt_xrootwin(),
                   NET::WMWindowType | NET::WMPid | NET::WMState );

  // ignore NET::Tool and other special window types
  if (info.windowType() != NET::Normal
      && info.windowType() != NET::Override
      && info.windowType() != NET::Unknown
      && info.windowType() != NET::Dialog)
    return;

  // ignore windows that want to be ignored by the taskbar
  if ((info.state() & NET::SkipTaskbar) != 0)
  {
    return;
  }

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
  QPtrListIterator<KomposeTask> it( tasklist );
  KomposeTask *task;
  while ( (task = it.current()) != 0 )
  {
    ++it;
    task->updateScreenshot();
  }
}

/*
 * Updates the screenshot for the specified app
 */
void KomposeTaskManager::slotUpdateScreenshot(WId winId)
{
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


void KomposeTaskManager::createVirtualDesktopView()
{
  if ( activeView )
    return;

  qDebug("KomposeTaskManager::createVirtualDesktopView - Creating View");

  slotUpdateScreenshots();

  qDebug("Got Screenshots");

  //   if (KomposeSettings::instance()->getUseGL())
  //     createGLVirtualDestopView();
  //   else
  createQtVirtualDestopView();

  KWin::forceActiveWindow( viewWidget->winId() );

  activeView = true;
  viewWidget->show();

  // Reactivate viewWidget
  //qApp->processEvents();

  slotStartWindowListeners();

}

// void KomposeTaskManager::createGLVirtualDestopView()
// {
//   qDebug("KomposeTaskManager::createGLVirtualDestopView");
//   // The parent widget we're working on
//   viewWidget = new KomposeGLFullscreenWidget();
// }

void KomposeTaskManager::createQtVirtualDestopView()
{
  qDebug("KomposeTaskManager::createQtVirtualDestopView");
  // The parent widget we're working on
  viewWidget = new KomposeFullscreenWidget();
}

void KomposeTaskManager::closeVirtualDesktopView()
{
  if ( !activeView )
    return;

  activeView = false;

  viewWidget->hide();
  viewWidget->close();
  delete viewWidget;
  viewWidget = 0;
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
  KWin::setCurrentDesktop(desknum+1);
  closeVirtualDesktopView();
}

void KomposeTaskManager::activateTask( KomposeTask *task )
{
  task->activate();
  closeVirtualDesktopView();
}


#include "komposetaskmanager.moc"
