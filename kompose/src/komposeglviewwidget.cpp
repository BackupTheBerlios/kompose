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
#include <kdebug.h>

KomposeGLViewWidget::KomposeGLViewWidget( int displayType , KomposeLayout *l )
 : AbstractViewWidget( 0, l, 0 )
{
  m_menu = KomposeGlobal::instance()->getViewMenu();

  m_glWidget = new KomposeGLWidget(this,displayType,l);

  QDesktopWidget deskwidget;
  if (KomposeSettings::instance()->getViewScreen() == -1)
  {
    setGeometry( deskwidget.availableGeometry( this ) );
  } else {
    QRect deskRect = deskwidget.availableGeometry(KomposeSettings::instance()->getViewScreen());
    setGeometry(deskRect);
  }

  setActiveWindow();
  // Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool | Qt::WPaintUnclipped | Qt::WNoAutoErase
  KWin::setType(winId(), NET::Dock);
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
