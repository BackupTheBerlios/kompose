//
// C++ Implementation: komposefullscreenwidget
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposefullscreenwidget.h"

#include "komposedesktopwidget.h"
#include "komposetaskwidget.h"
#include "komposetaskmanager.h"

#include <qcursor.h>

#include <krootpixmap.h>
#include <kwin.h>
#include <kapplication.h>


KomposeFullscreenWidget::KomposeFullscreenWidget( KomposeLayout *l )
    : KomposeWidget( 0, l )
{
  //   if ( QT_VERSION < 0x030300 )
  //   {
  setWindowState(Qt::WindowMaximized | Qt::WindowActive);
  showFullScreen();
  //   } else {
  //     setWindowState(Qt::WindowFullScreen | Qt::WindowActive);
  //   }

  rootpix = new KRootPixmap (this);

  setCursor( Qt::WaitCursor );

  createDesktopWidgets();

  rootpix->start();
  
  unsetCursor();
}


void KomposeFullscreenWidget::createDesktopWidgets()
{
  // Create a Widget for every desktop
  for (int i=0; i < KomposeTaskManager::instance()->getNumDesktops(); ++i)
  {
    int row = i / 2;
    int col = i % 2;
    //qDebug("rc %d %d", row, col);
    desktop[i] = new KomposeDesktopWidget(i, this);
    desktop[i]->show();
  }
}

KomposeFullscreenWidget::~KomposeFullscreenWidget()
{}

int KomposeFullscreenWidget::getHeightForWidth ( int w ) const
{
  return ((double)w / (double)width()) * (double)height();
}

int KomposeFullscreenWidget::getWidthForHeight ( int h ) const
{
  return ((double)h / (double)height()) * (double)width();
}

double KomposeFullscreenWidget::getAspectRatio()
{
  return (double)width() / (double)height();
}


#include "komposefullscreenwidget.moc"
