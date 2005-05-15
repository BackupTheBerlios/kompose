/***************************************************************************
 *   Copyright (C) 2004 by Hans Oischinger                                 *
 *   oisch@users.berlios.de                                                 *
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
#include "komposedesktopwidget.h"
#include "komposelayout.h"
#include "komposetask.h"
#include "komposetaskwidget.h"
#include "komposeviewmanager.h"
#include "komposetaskmanager.h"
#include "komposesettings.h"
#include "komposeglobal.h"

#include <qdesktopwidget.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qdragobject.h>
#include <qtooltip.h>

#include <klocale.h>
#include <kwin.h>
#include <ksharedpixmap.h>
#include <kwinmodule.h>
#include <kcursor.h>
#include <kdebug.h>


KomposeDesktopWidget::KomposeDesktopWidget(int desktop, QWidget *parent, KomposeLayout *l, const char *name)
    : KomposeTaskContainerWidget( desktop, parent, l, name ),
    highlight(false)
{
  // Retrieve geometry
  //   QDesktopWidget *deskwidget = new QDesktopWidget();
  //   deskRect = deskwidget->screenGeometry();
  //   delete deskwidget;
  initFonts();

  // Set Desktop background as our background
  setBackgroundMode( Qt::NoBackground );

  KWinModule kwinmodule( this, 1 );
  deskName = kwinmodule.desktopName(desktop+1);

  QToolTip::add( this, i18n("Desktop %1 - %2").arg(desktop).arg(deskName) );

  setAcceptDrops(TRUE);

  createTaskWidgets();

  connect( KomposeTaskManager::instance(), SIGNAL( newTask( KomposeTask* ) ), this, SLOT( createTaskWidget( KomposeTask* ) ) );

  setFocusPolicy(QWidget::ClickFocus);

}


KomposeDesktopWidget::~KomposeDesktopWidget()
{
  disconnect( KomposeTaskManager::instance(), SIGNAL( newTask( KomposeTask* ) ), this, SLOT( createTaskWidget( KomposeTask* ) ) );
}

void KomposeDesktopWidget::initFonts()
{
  titleFont = KomposeSettings::instance()->getDesktopTitleFont();
}


// int KomposeDesktopWidget::getHeightForWidth( int w ) const
// {
//   kdDebug() << "KomposeDesktopWidget::getHeightForWidth()");
//   return ((double)w / (double)deskRect.width()) * deskRect.height();
// }
//
// int KomposeDesktopWidget::getWidthForHeight( int h ) const
// {
//   kdDebug() << "KomposeDesktopWidget::getWidthForHeight()");
//   return ((double)h / (double)deskRect.height()) * deskRect.width();
// }
//
// double KomposeDesktopWidget::getAspectRatio()
// {
//   kdDebug() << "KomposeDesktopWidget::getAspectRatio()");
//   return (double)deskRect.width() / (double)deskRect.height();
// }


void KomposeDesktopWidget::leaveEvent ( QEvent * )
{
  // Unset highlight if cursor moves out of our rect
  // but not if it enters a child widget
  QRect deskRect;
  deskRect.setTopLeft(mapToGlobal( QPoint(0,0) ));
  deskRect.setWidth(width());
  deskRect.setHeight(height());
  if ( !deskRect.contains( QCursor::pos() ) )
    highlight = false;

  unsetCursor();
  repaint();
}

void KomposeDesktopWidget::enterEvent ( QEvent * )
{
  setFocus();
  setCursor( KCursor::handCursor() );
  highlight = true;
  repaint();
}

void KomposeDesktopWidget::focusInEvent ( QFocusEvent * )
{
  highlight = true;
  repaint();
}

void KomposeDesktopWidget::focusOutEvent ( QFocusEvent * )
{
  // Unset highlight if cursor moves out of our rect
  // but not if it enters a child widget
  QRect deskRect;
  deskRect.setTopLeft(mapToGlobal( QPoint(0,0) ));
  deskRect.setWidth(width());
  deskRect.setHeight(height());
  if ( !deskRect.contains( QCursor::pos() ) )
    highlight = false;

  repaint();
}

void KomposeDesktopWidget::mouseReleaseEvent ( QMouseEvent * e )
{
  if ( !rect().contains( e->pos() ) )
    return;
  KomposeViewManager::instance()->setCurrentDesktop(desktop);
}


void KomposeDesktopWidget::mouseDoubleClickEvent ( QMouseEvent * e )
{
  if ( !rect().contains( e->pos() ) )
    return;
  KomposeViewManager::instance()->setCurrentDesktop(desktop);
}


void KomposeDesktopWidget::paintEvent ( QPaintEvent * )
{
  // KomposeWidget::paintEvent(e);
  QPainter p;
  p.begin( this );
  QPoint tl = mapToGlobal(QPoint(0,0));
  p.drawPixmap(0,0, *KomposeGlobal::instance()->getDesktopBgPixmap(),
               tl.x(), tl.y(), width(), height());

  p.setFont(titleFont);

  if (highlight)
    p.setPen( KomposeSettings::instance()->getDesktopTitleFontHighlightColor() );
  else
    p.setPen( KomposeSettings::instance()->getDesktopTitleFontColor() );

  // Bounding rect
  p.drawRect(rect());

  // paint the Desktop num & name (centered if empty, bottom right if not)
  if ( layout->getNumofChilds() == 0 )
    p.drawText(QRect(QPoint(0,0), size()), Qt::AlignCenter, QString("Desktop %1 - %2").arg(desktop+1).arg(deskName));
  else
    p.drawText(QRect(QPoint(0,0), size()), Qt::AlignRight | Qt::AlignBottom, QString("Desktop %1 - %2").arg(desktop+1).arg(deskName));

  p.end();
}

void KomposeDesktopWidget::dragEnterEvent( QDragEnterEvent* e )
{
  kdDebug() << "KomposeDesktopWidget::dragEnterEvent" << endl;
  e->accept( QTextDrag::canDecode(e) );
}

void KomposeDesktopWidget::dropEvent ( QDropEvent * e )
{
  QString text;
  if ( QTextDrag::decode(e, text) )
  {
    kdDebug() << "KomposeDesktopWidget::dropEvent - Received Task drop" << endl;
    KomposeTaskWidget* dropWidget = dynamic_cast<KomposeTaskWidget*>(e->source());
    dropWidget->getTask()->toDesktop( desktop + 1);
    //if ( dropWidget->parentWidget() != this)
    //  dropWidget->reparent( this, QPoint(0,0), true );

    e->accept();
  }
}


void KomposeDesktopWidget::childEvent ( QChildEvent * ce)
{
  KomposeWidget::childEvent( ce );
  emit contentsChanged();
}


void KomposeDesktopWidget::keyReleaseEvent ( QKeyEvent * e )
{
  if ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Space )
  {
    kdDebug() << "KomposeDesktopWidget::keyReleaseEvent - Switching to Desktop!" << endl;
    KomposeViewManager::instance()->setCurrentDesktop(desktop);
    e->accept();
  }
  else
  {
    kdDebug() << "KomposeDesktopWidget::keyReleaseEvent - ignored..." << endl;
    e->ignore();
  }
  KomposeTaskContainerWidget::keyReleaseEvent(e);
}


#include "komposedesktopwidget.moc"
