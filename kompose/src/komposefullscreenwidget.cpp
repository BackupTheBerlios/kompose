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
  
  /* use showMaximized instead of setWindowState to make it compile on qt 3.1 or whatever */
  showMaximized();
  showFullScreen();

  rootpix = new KRootPixmap (this);
  initView();
}

KomposeFullscreenWidget::~KomposeFullscreenWidget()
{
//   KomposeTaskContainerWidget::destroy();
}

// void KomposeFullscreenWidget::closeEvent ( QCloseEvent * e )
// {
//   destroyChildWidgets();
//   KomposeTaskContainerWidget::closeEvent( e );
// }

void KomposeFullscreenWidget::destroyChildWidgets()
{
  setUpdatesEnabled( false );
//   blockSignals(true);
  KomposeWidgetInterface *child;
  QPtrListIterator<KomposeWidgetInterface> it( *(layout->getManagedWidgets()));
  while ( (child = it.current()) != 0 )
  {
    ++it;
    layout->remove(child);
    dynamic_cast<QWidget*>(child)->deleteLater();
  }
  setUpdatesEnabled( true );
}

void KomposeFullscreenWidget::initView()
{
  setCursor( Qt::WaitCursor );
  
  destroyChildWidgets();

  if ( type == KOMPOSEDISPLAY_VIRTUALDESKS )
  {
    layout->setType( TLAYOUT_TASKCONTAINERS );
    createDesktopWidgets();
    disconnect( KomposeTaskManager::instance(), SIGNAL( newTask( KomposeTask* ) ), this, SLOT( createTaskWidget( KomposeTask* ) ) );
  }
  else if ( type == KOMPOSEDISPLAY_WORLD )
  {
    layout->setType( TLAYOUT_GENERIC );
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
    KomposeDesktopWidget *desktop = new KomposeDesktopWidget(i, this );
    connect(desktop, SIGNAL(contentsChanged()), layout, SLOT(arrangeLayout()) );
    desktop->show();
  }
}

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

void KomposeFullscreenWidget::keyReleaseEvent ( QKeyEvent * e )
{
  if ( e->key() == Qt::Key_Escape )
  {
    qDebug("KomposeFullscreenWidget::keyReleaseEvent - Esc key pressed - Closing view");
    KomposeTaskManager::instance()->closeCurrentView();
    e->accept();
  }
  else
  {
    qDebug("KomposeFullscreenWidget::keyReleaseEvent - ignored...");
    e->ignore();
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
