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
#include "komposetask.h"

#include "komposetaskmanager.h"
#include "komposesettings.h"
#include "komposeglobal.h"

#include <qobject.h>
#include <qimage.h>
#include <qapplication.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <kpixmapio.h>

#include <kwinmodule.h>
#include <klocale.h>

#include <netwm.h>
#include <time.h>

// Last include as it's Xlibs includes conflict with qt includes
#include "komposetaskvisualizer.h"
#include <X11/Xlib.h>

KomposeTask::KomposeTask(WId win, KWinModule *kwinmod, QObject *parent, const char *name)
    : QObject(parent, name),
    kwinmodule(kwinmod),
    windowID(win)
{
  findWmFrame();
  
  // Initial refresh
  refresh();

  // Create visualizer object
  visualizer = new KomposeTaskVisualizer( this );
}


KomposeTask::~KomposeTask()
{
  emit closed();
}

void KomposeTask::findWmFrame()
{
  Window target_win, parent, root;
  Window *children;
  uint status, nchildren;

  target_win = windowID;
  for (;;)
  {
    status = XQueryTree(QPaintDevice::x11AppDisplay(), target_win, &root, &parent, &children, &nchildren);
    if (!status)
      break;
    if (children)
      XFree(children);

    if (!parent || parent == root)
      break;
    else
      target_win = parent;
  }
  
  wmframeID = target_win;
}

void KomposeTask::slotActivated()
{
  visualizer->slotTaskActivated();
}

void KomposeTask::slotUpdateScreenshot()
{
  visualizer->slotUpdateScreenshot();
}

bool KomposeTask::isOnTop() const
{
  return KomposeTaskManager::instance()->isOnTop( this );
}


bool KomposeTask::isActive() const
{
  return ( kwinmodule->activeWindow() & windowID );
}

bool KomposeTask::isMaximized() const
{
  return (windowInfo.state() & NET::Max);
}

bool KomposeTask::isIconified() const
{
  //  return (windowInfo.mappingState == NET::Iconic);
  return windowInfo.isMinimized();
}

bool KomposeTask::isAlwaysOnTop() const
{
  return (windowInfo.state() & NET::StaysOnTop);
}

bool KomposeTask::isShaded() const
{
  return (windowInfo.state() & NET::Shaded);
}

bool KomposeTask::isOnCurrentDesktop() const
{
  return (windowInfo.onAllDesktops() || windowInfo.desktop() == KWin::currentDesktop());
}

int KomposeTask::onDesktop() const
{
  return windowInfo.desktop();
}

bool KomposeTask::isOnAllDesktops() const
{
  return windowInfo.onAllDesktops();
}


bool KomposeTask::isModified() const
{
  static QString modStr = QString::fromUtf8("[") + i18n("modified") + QString::fromUtf8("]");
  int modStrPos = windowInfo.visibleName().find(modStr);

  return ( modStrPos != -1 );
}


void KomposeTask::maximize()
{
  NETWinInfo ni( qt_xdisplay(),  windowID, qt_xrootwin(), NET::WMState);
  ni.setState( NET::Max, NET::Max );

  if (windowInfo.mappingState() == NET::Iconic)
    activate();
}

void KomposeTask::restore()
{
  KWin::deIconifyWindow( windowID, false );
  //   NETWinInfo ni( qt_xdisplay(),  windowID, qt_xrootwin(), NET::WMState);
  //   ni.setState( 0, NET::Max );
  //
  //   if (windowInfo.mappingState == NET::Iconic)
  //     activate();
}

void KomposeTask::iconify()
{
  //XIconifyWindow( qt_xdisplay(), windowID, qt_xscreen() );
  KWin::iconifyWindow( windowID, false );
}

void KomposeTask::close()
{
  NETRootInfo ri( qt_xdisplay(),  NET::CloseWindow );
  ri.closeWindowRequest( windowID );
}

void KomposeTask::raise()
{
  //    kdDebug(1210) << "Task::raise(): " << name() << endl;
  XRaiseWindow( qt_xdisplay(), windowID );
}

void KomposeTask::lower()
{
  //    kdDebug(1210) << "Task::lower(): " << name() << endl;
  XLowerWindow( qt_xdisplay(), windowID );
}


void KomposeTask::activate()
{
  KWin::forceActiveWindow(windowID);
}

// void KomposeTask::activateOrRaise()
// {
//   if ( !isActive() || isIconified() )
//   {
//     activate();
//   }
//   else if ( !isOnTop() )
//   {
//     raise();
//   }
// }

void KomposeTask::minimizeOrRestore()
{
  if ( isIconified() )
  {
    restore();
  }
  else
  {
    iconify();
  }
}

void KomposeTask::toDesktop(int desk)
{
  NETWinInfo ni(qt_xdisplay(), windowID, qt_xrootwin(), NET::WMDesktop);
  if (desk == 0)
  {
    ni.setDesktop(NETWinInfo::OnAllDesktops);
    return;
  }
  //ni.setDesktop(desk+1);
  KWin::setOnDesktop( windowID, desk );
}

void KomposeTask::toCurrentDesktop()
{
  toDesktop(kwinmodule->currentDesktop());
}

void KomposeTask::setAlwaysOnTop(bool stay)
{
  NETWinInfo ni( qt_xdisplay(),  windowID, qt_xrootwin(), NET::WMState);
  if(stay)
    ni.setState( NET::StaysOnTop, NET::StaysOnTop );
  else
    ni.setState( 0, NET::StaysOnTop );
}

void KomposeTask::toggleAlwaysOnTop()
{
  setAlwaysOnTop( !isAlwaysOnTop() );
}

void KomposeTask::setShaded(bool shade)
{
  NETWinInfo ni( qt_xdisplay(),  windowID, qt_xrootwin(), NET::WMState);
  if(shade)
    ni.setState( NET::Shaded, NET::Shaded );
  else
    ni.setState( 0, NET::Shaded );
}

void KomposeTask::toggleShaded()
{
  setShaded( !isShaded() );
}

/*
 * Update Info data
 */
void KomposeTask::refresh()
{
  qDebug("KomposeTask::refresh() (WId %d) - Window parameters have changed or initial refresh", windowID);
  windowInfo = KWin::windowInfo(windowID);

  if ( !windowInfo.valid() )
  {
    qDebug("KomposeTask::refresh() (WId %d) - Invalid window Info. window closed?!", windowID);
    // TODO: Find out what I have todo when this happens ( it does! )
    return;
  }

  emit stateChanged(); //TODO: Only emit this when state has really changed
}


/**
 * This has to be called by x11EventFilter ehenever ConfigureNotify for this win occurs
 * sorta hacky but x11EventFilter can only be implemented in a kapp object which redirects it 
 * to the taskmanager, that picks out the right task by winId and notifies us :(
 */
void KomposeTask::slotX11ConfigureNotify()
{
  emit x11ConfigureNotify();
}

int KomposeTask::getHeightForWidth ( int w ) const
{
  return ((double)w / (double)getGeometry().width()) * getGeometry().height();
}

int KomposeTask::getWidthForHeight ( int h ) const
{
  return ((double)h / (double)getGeometry().height()) * getGeometry().width();
}

double KomposeTask::getAspectRatio()
{
  return (double)(getGeometry().width()) / (double)(getGeometry().height());
}

QPixmap KomposeTask::getIcon( int size )
{
  // Cache the icons???
  //   if ( pm_icon.isNull() || pm_icon.width() != size )
  //   {
  //     if (size == -1)
  //       pm_icon = KWin::icon( windowID );
  //     else
  //       pm_icon = KWin::icon( windowID, size, size, true );
  //   }
  //
  //   return pm_icon;
  if (size == -1)
    return KWin::icon( windowID );
  else
    return KWin::icon( windowID, size, size, true );
}


#include "komposetask.moc"
