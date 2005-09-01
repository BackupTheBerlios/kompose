//
// C++ Implementation: komposeglviewwidget
//
// Description: 
//
//
// Author: Hans Oischinger <hans.oischinger@kde-mail.net>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposeglviewwidget.h"

#include "komposeviewmanager.h"
#include "komposetaskmanager.h"
#include "komposeglobal.h"
#include "komposelayout.h"
#include "komposeglwidget.h"

#include <qdesktopwidget.h>
#include <qapplication.h>

#include <kwin.h>
#include <kpopupmenu.h>
#include <kglobalsettings.h>
#include <kdebug.h>

KomposeGLViewWidget::KomposeGLViewWidget( int displayType , KomposeLayout *l )
 : AbstractViewWidget(0,l)
{
  setBackgroundMode( Qt::FixedPixmap );
  m_menu = KomposeGlobal::instance()->getViewMenu();

  m_glWidget = new KomposeGLWidget(this,displayType,l);
  setWindowState(windowState() | WindowFullScreen);

  if (KomposeSettings::instance()->getViewScreen() == -1)
    setGeometry( KGlobalSettings::desktopGeometry( this ) );
  else
  {
    QDesktopWidget deskwidget;
    QRect deskRect = deskwidget.screenGeometry(KomposeSettings::instance()->getViewScreen());
    setGeometry(deskRect);
    kdDebug() << deskRect << endl;
  }

  if (!isTopLevel())
    QApplication::sendPostedEvents(this, QEvent::ShowFullScreen);
  setActiveWindow();

  KWin::setOnAllDesktops( winId(), true );
}

KomposeGLViewWidget::~KomposeGLViewWidget()
{}


int KomposeGLViewWidget::getHeightForWidth ( int w ) const
{
  return (int)(((double)w / (double)width()) * (double)height());
}

int KomposeGLViewWidget::getWidthForHeight ( int h ) const
{
  return (int)(((double)h / (double)height()) * (double)width());
}

double KomposeGLViewWidget::getAspectRatio()
{
  return (double)width() / (double)height();
}

void KomposeGLViewWidget::resizeEvent( QResizeEvent *e )
{
  m_glWidget->resize(size());
}

void KomposeGLViewWidget::mousePressEvent ( QMouseEvent * e )
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

void KomposeGLViewWidget::keyReleaseEvent ( QKeyEvent * e )
{
  if ( e->key() == Qt::Key_Escape )
  {
    kdDebug() << "KomposeGLViewWidget::keyReleaseEvent - Esc key pressed - Closing view" << endl;
    KomposeViewManager::instance()->closeCurrentView();
    e->accept();
  }
  else
  {
    kdDebug() << "KomposeGLViewWidget::keyReleaseEvent - ignored..." << endl;
    e->ignore();
  }
  KomposeTaskContainerWidget::keyReleaseEvent(e);
}
