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
#include "komposeviewmanager.h"
#include "komposetaskmanager.h"
#include "komposeglobal.h"
#include "komposelayout.h"

#include <qobjectlist.h>
#include <qptrlist.h>
#include <qpainter.h>

#include <kwin.h>
#include <kapplication.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <ksharedpixmap.h>
#include <kcursor.h>


KomposeFullscreenWidget::KomposeFullscreenWidget( int displayType, KomposeLayout *l )
    : KomposeTaskContainerWidget( -1, 0, l ),
    type(displayType)
{
  //   if ( QT_VERSION < 0x030300 )

  /* use showMaximized instead of setWindowState to make it compile on qt 3.1 or whatever */
  //   showMaximized();
  //   KWin::setState( winId(), NET::KeepAbove );

  // Set Desktop background as our background
  setBackgroundMode( Qt::FixedPixmap );
  setBackgroundPixmap(*(KomposeGlobal::instance()->getDesktopBgPixmap()));
  initMenu();
  initView();
  //showFullScreen();
  
  // Alternate showFullscreen
  setWindowState(windowState() | WindowFullScreen);
  //setWFlags(WStyle_NoBorder);
  setGeometry( QApplication::desktop()->availableGeometry() );
  
  show();
  
  if (!isTopLevel())
    QApplication::sendPostedEvents(this, QEvent::ShowFullScreen);
  setActiveWindow();

  KWin::setOnAllDesktops( winId(), true );
}

KomposeFullscreenWidget::~KomposeFullscreenWidget()
{
  menu->deleteLater();
}


void KomposeFullscreenWidget::initMenu()
{
  menu = new KPopupMenu();

  KomposeGlobal::instance()->getActShowWorldView()->plug(menu);
  KomposeGlobal::instance()->getActShowVirtualDesktopView()->plug(menu);
  KomposeGlobal::instance()->getActShowCurrentDesktopView()->plug(menu);
  menu->insertSeparator();
  KomposeGlobal::instance()->getActPreferencesDialog()->plug(menu);
  KomposeGlobal::instance()->getActConfigGlobalShortcuts()->plug(menu);
  KomposeGlobal::instance()->getActAboutDlg()->plug(menu);
  //menu->insertSeparator();
  //KomposeGlobal::instance()->getActQuit()->plug(menu);
}


void KomposeFullscreenWidget::destroyChildWidgets()
{
  setUpdatesEnabled( false );
  KomposeWidget *child;
  QPtrListIterator<KomposeWidget> it( *(layout->getManagedWidgets()));
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
  setCursor( KCursor::waitCursor() );

  destroyChildWidgets();

  if ( type == KOMPOSEDISPLAY_VIRTUALDESKS )
  {
    disconnect( KomposeTaskManager::instance(), SIGNAL( newTask( KomposeTask* ) ), this, SLOT( createTaskWidget( KomposeTask* ) ) );
    layout->setType( TLAYOUT_TASKCONTAINERS );
    createDesktopWidgets();
  }
  else if ( type == KOMPOSEDISPLAY_WORLD )
  {
    layout->setType( TLAYOUT_GENERIC );
    setDesktop( -1 );
    createTaskWidgets();
    connect( KomposeTaskManager::instance(), SIGNAL( newTask( KomposeTask* ) ), this, SLOT( createTaskWidget( KomposeTask* ) ) );
  }
  else if ( type == KOMPOSEDISPLAY_CURRENTDESK )
  {
    layout->setType( TLAYOUT_GENERIC );
    setDesktop( KomposeViewManager::instance()->getDesktopBeforeSnaps() );
    createTaskWidgets();
    connect( KomposeTaskManager::instance(), SIGNAL( newTask( KomposeTask* ) ), this, SLOT( createTaskWidget( KomposeTask* ) ) );
  }
  
  unsetCursor();
}

void KomposeFullscreenWidget::createDesktopWidgets()
{
  qDebug("KomposeFullscreenWidget::createDesktopWidgets()");
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
    menu->popup( e->globalPos() );
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
    KomposeViewManager::instance()->closeCurrentView();
    e->accept();
  }
  else
  {
    qDebug("KomposeFullscreenWidget::keyReleaseEvent - ignored...");
    e->ignore();
  }
  KomposeTaskContainerWidget::keyReleaseEvent(e);
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

void KomposeFullscreenWidget::paintEvent ( QPaintEvent * e )
{
  QPainter p( this );
  p.drawPixmap(rect(), *(KomposeGlobal::instance()->getDesktopBgPixmap()));
  p.end();
}


#include "komposefullscreenwidget.moc"
