//
// C++ Implementation: komposefullscreenwidget
//
// Description:
//
//
// Author: Hans Oischinger <hans.oischinger@kde-mail.net>, (C) 2004
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
#include <qdesktopwidget.h>

#include <kwin.h>
#include <kapplication.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <ksharedpixmap.h>
#include <kcursor.h>
#include <kglobalsettings.h>
#include <kdebug.h>
#include <krootpixmap.h>


KomposeFullscreenWidget::KomposeFullscreenWidget( int displayType, KomposeLayout *l )
    : AbstractViewWidget( 0, l )
{
  //   if ( QT_VERSION < 0x030300 )

  /* use showMaximized instead of setWindowState to make it compile on qt 3.1 or whatever */
  //   showMaximized();
  //   KWin::setState( winId(), NET::KeepAbove );

  // Set Desktop background as our background
  setBackgroundMode( Qt::FixedPixmap );
//   setBackgroundPixmap(*(KomposeGlobal::self()->getDesktopBgPixmap()));
  rootpix = new KRootPixmap (this);
  rootpix->start();
  m_menu = KomposeGlobal::self()->getViewMenu();
  initView();

  // Alternate showFullscreen
  setWindowState(windowState() | WindowFullScreen);

  if (KomposeSettings::self()->viewScreen() == -1)
    setGeometry( KGlobalSettings::desktopGeometry( this ) );
  else
  {
    QDesktopWidget deskwidget;
    QRect deskRect = deskwidget.screenGeometry(KomposeSettings::self()->viewScreen());
    setGeometry(deskRect);
    kdDebug() << deskRect << endl;
  }

  if (!isTopLevel())
    QApplication::sendPostedEvents(this, QEvent::ShowFullScreen);
  setActiveWindow();

  KWin::setOnAllDesktops( winId(), true );
}

KomposeFullscreenWidget::~KomposeFullscreenWidget()
{}

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
  kdDebug() << "KomposeFullscreenWidget::destroyChildWidgets() - all children destroyed" << endl;
}

void KomposeFullscreenWidget::initView()
{
  setCursor( KCursor::waitCursor() );

  destroyChildWidgets();

  if ( type == KomposeSettings::EnumView::VirtualDesktops )
  {
    disconnect( KomposeTaskManager::self(), SIGNAL( newTask( KomposeTask* ) ), this, SLOT( createTaskWidget( KomposeTask* ) ) );
    layout->setType( TLAYOUT_TASKCONTAINERS );
    setDesktop( -2 );
    createDesktopWidgets();
  }
  else if ( type == KomposeSettings::EnumView::World )
  {
    layout->setType( TLAYOUT_GENERIC );
    setDesktop( -1 );
    createTaskWidgets();
    connect( KomposeTaskManager::self(), SIGNAL( newTask( KomposeTask* ) ), this, SLOT( createTaskWidget( KomposeTask* ) ) );
  }
  else if ( type == KomposeSettings::EnumView::CurrentDesktop )
  {
    layout->setType( TLAYOUT_GENERIC );
    setDesktop( KomposeViewManager::self()->getDesktopBeforeSnaps() );
    createTaskWidgets();
    connect( KomposeTaskManager::self(), SIGNAL( newTask( KomposeTask* ) ), this, SLOT( createTaskWidget( KomposeTask* ) ) );
  }

  unsetCursor();
}

void KomposeFullscreenWidget::createDesktopWidgets()
{
  kdDebug() << "KomposeFullscreenWidget::createDesktopWidgets()" << endl;
  // Create a Widget for every desktop
  for (int i=0; i < KomposeTaskManager::self()->getNumDesktops(); ++i)
  {
    //int row = i / 2;
    //int col = i % 2;
    //kdDebug() << "rc %d %d", row, col);
    KomposeDesktopWidget *desktop = new KomposeDesktopWidget(i, this );
    connect(desktop, SIGNAL(contentsChanged()), layout, SLOT(arrangeLayout()) );
    desktop->show();
  }
}

void KomposeFullscreenWidget::mouseReleaseEvent (QMouseEvent * )
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
    m_menu->popup( e->globalPos() );
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
    kdDebug() << "KomposeFullscreenWidget::keyReleaseEvent - Esc key pressed - Closing view" << endl;
    KomposeViewManager::self()->closeCurrentView();
    e->accept();
  }
  else
  {
    kdDebug() << "KomposeFullscreenWidget::keyReleaseEvent - ignored..." << endl;
    e->ignore();
  }
  KomposeTaskContainerWidget::keyReleaseEvent(e);
}

int KomposeFullscreenWidget::getHeightForWidth ( int w ) const
{
  return (int)(((double)w / (double)width()) * (double)height());
}

int KomposeFullscreenWidget::getWidthForHeight ( int h ) const
{
  return (int)(((double)h / (double)height()) * (double)width());
}

double KomposeFullscreenWidget::getAspectRatio()
{
  return (double)width() / (double)height();
}


#include "komposefullscreenwidget.moc"
