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
#include "komposeglobal.h"
#include "komposetask.h"
#include "komposeviewmanager.h"

#include <qtimer.h>
#include <qimage.h>
#include <qpixmap.h>

#include <kapplication.h>
#include <kwinmodule.h>
#include <netwm.h>
#include <kwin.h>
#include <kapplication.h>

#ifdef COMPOSITE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
// #include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
// #include <X11/extensions/Xdamage.h>
// #include <X11/extensions/Xrender.h>
#endif

static KomposeTaskManager* taskManagerInstance = 0;


/**
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
    : QObject()
{
  taskManagerInstance = this;
  kwin_module = new KWinModule();
  numDesks = KWin::numberOfDesktops();

#ifdef COMPOSITE
  // Redirect all root windows to offscreen buffers
  if ( KomposeGlobal::instance()->hasXcomposite() )
  {
    Display *dpy = QPaintDevice::x11AppDisplay();
    for ( int i = 0; i < ScreenCount( dpy ); i++ )
      XCompositeRedirectSubwindows( dpy, RootWindow( dpy, i ), CompositeRedirectAutomatic );
  }
#endif


  // Listeners for KWinmodule signals
  connect( kwin_module, SIGNAL(windowAdded(WId)), this, SLOT(slotWindowAdded(WId)) );
  connect( kwin_module, SIGNAL(windowRemoved(WId)), this, SLOT(slotWindowRemoved(WId)) );
  connect( kwin_module, SIGNAL(numberOfDesktopsChanged(int)), this, SLOT(slotDesktopCountChanged(int)) );
  connect( kwin_module, SIGNAL(currentDesktopChanged(int)), this, SLOT(slotCurrentDesktopChanged(int)) );

  connect( KomposeSettings::instance(), SIGNAL(settingsChanged()), this, SLOT(slotStartWindowListeners()) );

  // register existing windows
  const QValueList<WId> windows = kwin_module->windows();
  for (QValueList<WId>::ConstIterator it = windows.begin(); it != windows.end(); ++it )
    slotWindowAdded(*it);

  connect( kwin_module, SIGNAL(activeWindowChanged(WId)), this, SLOT(slotTaskActivated(WId)) );
  slotStartWindowListeners();
}

KomposeTaskManager::~KomposeTaskManager()
{}

/**
 * Helper function that finds a KomposeTask object by it's window id
 * @param w WindowID of the Task
 * @return Corresponding KomposeTask object
 */
KomposeTask* KomposeTaskManager::findTask(WId w, bool wmFrameIds )
{
  for (KomposeTask* t = tasklist.first(); t != 0; t = tasklist.next())
    if ((!wmFrameIds && t->window() == w) || (wmFrameIds && t->wmFrame() == w) )
      return t;
  return 0;
}

void KomposeTaskManager::slotStartWindowListeners()
{
  disconnect( kwin_module, SIGNAL(windowChanged( WId, unsigned int )), this,
              SLOT(slotWindowChanged( WId, unsigned int )) );
  connect( kwin_module, SIGNAL(windowChanged( WId, unsigned int )), this,
           SLOT(slotWindowChanged( WId, unsigned int )) );
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
  if(!(dirty & (NET::WMVisibleName|NET::WMVisibleIconName|NET::WMName|NET::WMState|NET::WMIcon|NET::XAWMState|NET::WMDesktop)) )
    return;

  // find task
  KomposeTask* t = findTask( w );
  if (!t) return;

  // TODO: Instead of one refresh() method we could implement specific method for names and geometry, etc...
  // checked like this: if(dirty & (NET::WMDesktop|NET::WMState|NET::XAWMState))
  t->refresh();
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
  if ( KomposeViewManager::instance()->hasActiveView() && w == KomposeViewManager::instance()->getViewWidget()->winId() )
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

  qDebug("KomposeTaskManager::slotWindowAdded(WId %d ) - Adding KomposeTask", w);
  KomposeTask* t = new KomposeTask(w, kwin_module, this);
  tasklist.append(t);

  emit newTask( t );
}


/**
 * Called when Komposé requires screenshots of all tasks
 */
void KomposeTaskManager::slotUpdateScreenshots()
{
  qDebug("KomposeTaskManager::slotUpdateScreenshots()");

  QPtrListIterator<KomposeTask> it( tasklist );
  KomposeTask *task;

  // Iterate through tasks sorted by desktops (this minimizes desktop switching if necessary)
  for ( int desk = 1; desk <= numDesks; ++desk )
  {
    it.toFirst();
    while ( (task = it.current()) != 0 )
    {
      ++it;
      if ( task->onDesktop() == desk )
        task->slotUpdateScreenshot();
    }
  }
}


/**
 * simulates a window activated event for the currently active window (no forcing of screenshots)
 */
void KomposeTaskManager::simulatePasvScreenshotEvent()
{
  qDebug("KomposeTaskManager::simulatePasvScreenshotEvent()");
  slotTaskActivated( kwin_module->activeWindow() );
}

/**
 * Signals the task object that it's window has been activated
 */
void KomposeTaskManager::slotTaskActivated(WId winId)
{
  qDebug("KomposeTaskManager::slotTaskActivated ( %d )", winId);
  QPtrListIterator<KomposeTask> it( tasklist );
  KomposeTask *task;
  while ( (task = it.current()) != 0 )
  {
    ++it;
    if ( winId == task->window() )
    {
      task->slotActivated();
      return;
    }
  }
}


void KomposeTaskManager::slotDesktopCountChanged(int d)
{
  numDesks = d;
}

bool KomposeTaskManager::isOnTop(const KomposeTask* task)
{
  if(!task) return false;

  for (QValueList<WId>::ConstIterator it = kwin_module->stackingOrder().fromLast();
       it != kwin_module->stackingOrder().end(); --it )
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


/**
 * The kapp x11EventFilter method redirect to this method
 * @param event 
 * @return 
 */
bool KomposeTaskManager::processX11Event( XEvent *event )
{
#ifdef COMPOSITE
  if ( KomposeGlobal::instance()->hasXcomposite() )
  {
    if ( event->type == ConfigureNotify )
    {
      XConfigureEvent *e = &event->xconfigure;

      KomposeTask* t = findTask( e->window, true );
      if (!t)
        return false;
      t->slotX11ConfigureNotify();
    }
  }
#endif
  return false;
}

void KomposeTaskManager::slotCurrentDesktopChanged(int d)
{
#ifdef COMPOSITE
  if ( KomposeGlobal::instance()->hasXcomposite() )
  {
    // Strangely a ConfigureNotify is only sent when I click on a window on the new desktop
    // and not when I cahnge the desktop, although the windows get mapped at this point.
    // Is this a X bug? However the following hack helps:
    // Do as if we received a ConfigureNotify event to update all backing pixmaps
    for (KomposeTask* t = tasklist.first(); t != 0; t = tasklist.next())
      if ( t->onDesktop() == d )
        t->slotX11ConfigureNotify();
  }
#endif
}

#include "komposetaskmanager.moc"
