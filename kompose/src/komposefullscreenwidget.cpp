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
#include "komposeglobal.h"
#include "komposelayout.h"

#include <qobjectlist.h>
#include <qptrlist.h>
#include <qcursor.h>

#include <krootpixmap.h>
#include <kwin.h>
#include <kapplication.h>
#include <kpopupmenu.h>

KomposeFullscreenWidget::KomposeFullscreenWidget( int displayType, KomposeLayout *l )
    : KomposeTaskContainerWidget( -1, 0, l ),
    type(displayType)
{
  //   if ( QT_VERSION < 0x030300 )
  //   {
  setWindowState(Qt::WindowMaximized | Qt::WindowActive);
  showFullScreen();
  //   } else {
  //     setWindowState(Qt::WindowFullScreen | Qt::WindowActive);
  //   }

  rootpix = new KRootPixmap (this);
  initView();
}


void KomposeFullscreenWidget::initView()
{
  setCursor( Qt::WaitCursor );
  
  setUpdatesEnabled( false );
  KomposeWidgetInterface *child;
  QPtrListIterator<KomposeWidgetInterface> it( *(layout->getManagedWidgets()));
  while ( (child = it.current()) != 0 )
  {
    ++it;
    qDebug("KomposeFullscreenWidget::initView() - Removing widget");
    removeChildWidget( child );
  }
  setUpdatesEnabled( true );


  if ( type == KOMPOSEDISPLAY_VIRTUALDESKS )
  {
    createDesktopWidgets();
  }
  else if ( type == KOMPOSEDISPLAY_WORLD )
  {
    setDesktop( -1 );
    createTaskWidgets();
    connect( KomposeTaskManager::instance(), SIGNAL( newTask( KomposeTask* ) ), this, SLOT( createTaskWidget( KomposeTask* ) ) );
  }

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
    KomposeDesktopWidget *desktop = new KomposeDesktopWidget(i, this);
    desktop->show();
  }
}

KomposeFullscreenWidget::~KomposeFullscreenWidget()
{}

void KomposeFullscreenWidget::mouseReleaseEvent (QMouseEvent * e)
{}

void KomposeFullscreenWidget::mousePressEvent ( QMouseEvent * e )
{
  if ( !rect().contains( e->pos() ) )
    return;

  switch ( e->button() )
  {
  case LeftButton:
    break;
  case MidButton:
    // fall through
  case RightButton:
    KomposeGlobal::instance()->getSysTray()->contextMenu()->popup( e->globalPos() );
    break;
  default:
    // nothing
    break;
  }
}

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
