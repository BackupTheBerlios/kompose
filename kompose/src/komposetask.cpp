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
#include "komposetaskwidget.h"

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



KomposeTask::KomposeTask(WId win, KWinModule *kwinmod, QObject *parent, const char *name)
    : QObject(parent, name),
    kwinmodule(kwinmod),
    active(false), windowID(win),
    pm_screenshot(),
    img_screenshot(),
    imageNeedsUpdate(true)
{
  windowInfo = KWin::info(windowID);
}


KomposeTask::~KomposeTask()
{
  emit closed();
}


bool KomposeTask::isOnTop() const
{
  return KomposeTaskManager::instance()->isOnTop( this );
}


bool KomposeTask::isActive() const
{
  return active;
}

bool KomposeTask::isMaximized() const
{
  return (windowInfo.state & NET::Max);
}

bool KomposeTask::isIconified() const
{
  //  return (windowInfo.mappingState == NET::Iconic);
  return windowInfo.isMinimized();
}

bool KomposeTask::isAlwaysOnTop() const
{
  return (windowInfo.state & NET::StaysOnTop);
}

bool KomposeTask::isShaded() const
{
  return (windowInfo.state & NET::Shaded);
}

bool KomposeTask::isOnCurrentDesktop() const
{
  return (windowInfo.onAllDesktops || windowInfo.desktop == kwinmodule->currentDesktop());
}

int KomposeTask::onDesktop() const
{
  return windowInfo.desktop;
}

bool KomposeTask::isOnAllDesktops() const
{
  return windowInfo.onAllDesktops;
}


bool KomposeTask::isModified() const
{
  static QString modStr = QString::fromUtf8("[") + i18n("modified") + QString::fromUtf8("]");
  int modStrPos = windowInfo.visibleName.find(modStr);

  return ( modStrPos != -1 );
}


void KomposeTask::maximize()
{
  NETWinInfo ni( qt_xdisplay(),  windowID, qt_xrootwin(), NET::WMState);
  ni.setState( NET::Max, NET::Max );

  if (windowInfo.mappingState == NET::Iconic)
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
  NETRootInfo ri( qt_xdisplay(), 0 );
  ri.setActiveWindow( windowID );
}

void KomposeTask::activateOrRaise()
{
  if ( !isActive() || isIconified() )
  {
    activate();
  }
  else if ( !isOnTop() )
  {
    raise();
  }
}

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
  
  refresh();
}

void KomposeTask::toDesktop(int desk)
{
  qDebug("KomposeTask::toDesktop %d", desk);
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
  qDebug("KomposeTask::refresh() - Window parameters have changed");
  windowInfo = KWin::info(windowID);

  if ( !KomposeTaskManager::instance()->hasActiveView() )
    return;

  emit stateChanged();
}

void KomposeTask::updateScreenshot()
{
  bool iconifyLater = false;

  if ( !pm_screenshot.isNull() )
  {
    if ( ( !KomposeSettings::instance()->getPassiveScreenshots() && KomposeSettings::instance()->getOnlyOneScreenshot() ) ||
         ( KomposeSettings::instance()->getPassiveScreenshots() &&
           ( ( !KomposeSettings::instance()->getOnlyOneScreenshot() && !isOnTop() )
             || KomposeSettings::instance()->getOnlyOneScreenshot() ) ) )
    {
      qDebug("KomposeTask::updateScreenshot() - Screenshot already exists... skipping.");
      return;
    }
  }

  if ( isIconified() == true )
  {
    qDebug("KomposeTask::updateScreenshot() - Window iconified... we have to raise it and iconify it again later.");
    iconifyLater = true;
  }
  else
    qDebug("KomposeTask::updateScreenshot() - Window NOT iconified... easy...");

  activateOrRaise();

  if ( !isOnCurrentDesktop() )
  {
    qDebug("KomposeTask::updateScreenshot() - Error: !isOnCurrentDesktop()");
  }
  if ( !isActive() )
  {
    qDebug("KomposeTask::updateScreenshot() - Error: !isActive()");
  }

  QWidget *rootWin = qApp->desktop();
  QRect geom = windowInfo.geometry;

  struct timespec req, rem;
  req.tv_sec = 0;
  req.tv_nsec = KomposeSettings::instance()->getScreenshotGrabDelay();
  while(nanosleep(&req, &rem))
    req = rem;

  // TODO: Find a function that grabs windows and stores them as QImage. so conversion won't be needed
  //   pm_screenshot = QPixmap::grabWindow( rootWin->winId(), geom.x(), geom.y(), geom.width(), geom.height() );
  pm_screenshot = QPixmap::grabWindow( windowID );
  imageNeedsUpdate = true;
  //   while(nanosleep(&req, &rem))
  //     req = rem;

  if ( iconifyLater )
    QTimer::singleShot( 1000, this, SLOT( iconify() ) );

  qDebug("KomposeTask::updateScreenshot() - Created Screenshot: x:%d y:%d size:%dx%d", geom.x(), geom.y(), geom.width(), geom.height() );
}


int KomposeTask::getHeightForWidth ( int w ) const
{
  qDebug("KomposeTask::getHeightForWidth(%d) - Screenshot w: %d, h: %d", w, pm_screenshot.width(), pm_screenshot.height());
  return ((double)w / (double)pm_screenshot.width()) * pm_screenshot.height();
}

int KomposeTask::getWidthForHeight ( int h ) const
{
  qDebug("KomposeTask::getWidthForHeight(%d) - Screenshot w: %d, h: %d", h, pm_screenshot.width(), pm_screenshot.height());
  return ((double)h / (double)pm_screenshot.height()) * pm_screenshot.width();
}

double KomposeTask::getAspectRatio()
{
  qDebug("KomposeTask::getAspectRatio() - Screenshot w: %d, h: %d", pm_screenshot.width(), pm_screenshot.height());
  return (double)(pm_screenshot.width()) / (double)(pm_screenshot.height());
}

QImage& KomposeTask::getScreenshotImg()
{
  if ( pm_screenshot.isNull() )
    return img_screenshot;

  if ( imageNeedsUpdate )
  {
    // Check if update is needed
    img_screenshot = KomposeSettings::instance()->getPixmapIO()->convertToImage( pm_screenshot );
  }
  return img_screenshot;
}

#include "komposetask.moc"
