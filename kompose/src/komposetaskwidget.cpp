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
#include "komposetaskwidget.h"

#include "komposetaskmanager.h"
#include "komposeviewmanager.h"
#include "komposetask.h"
#include "komposetaskprefswidget.h"
#include "komposesettings.h"
#include "komposeglobal.h"
#include "komposesettings.h"

#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>
#include <qdragobject.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <qiconset.h>
#include <qtimer.h>
#include <qcolor.h>
#include <qfont.h>

#include "komposetaskvisualizer.h"

KomposeTaskWidget::KomposeTaskWidget(KomposeTask *t, QWidget *parent, KomposeLayout *l, const char *name)
    : KomposeWidget(parent, l, name),
    task(t),
    highlight(false)
{
  setWFlags( WStaticContents | WRepaintNoErase | WResizeNoErase );  // avoid redraw errors
  setBackgroundMode( Qt::NoBackground );  // avoid flickering

  QToolTip::add( this, task->visibleNameWithState() );

  pm_dbBackground.setOptimization( QPixmap::BestOptim );
  setBackgroundMode( Qt::NoBackground );
  //setBackgroundPixmap(pm_dbBackground);
  
  //   screenshot = new KomposeImage( imIface, task->getScreenshotImage() );
  prefWidget = new KomposeTaskPrefsWidget( this, "Task prefs" );
  prefWidget->hide();

  //connect( t, SIGNAL( destroyed() ), this, SLOT( slotTaskDestroyed() ) );
  connect( t, SIGNAL( closed() ), this, SLOT( slotTaskDestroyed() ) );
  connect( t, SIGNAL( stateChanged() ), this, SLOT( drawWidgetAndRepaint() ) );

#ifdef COMPOSITE
  if ( KomposeGlobal::instance()->hasXcomposite() )
  {
    connect( t, SIGNAL( x11DamageNotify() ), this, SLOT( drawWidgetAndRepaint() ) );
  }
#endif
  //setFocusPolicy(QWidget::ClickFocus);
  setFocusPolicy(QWidget::StrongFocus);

  initFonts();
  //   hide();
}


KomposeTaskWidget::~KomposeTaskWidget()
{}

void KomposeTaskWidget::initFonts()
{
  titleFont = KomposeSettings::instance()->getWindowTitleFont();
}

void KomposeTaskWidget::slotTaskDestroyed()
{
  disconnect( task, SIGNAL( closed() ), this, SLOT( slotTaskDestroyed() ) );
  disconnect( task, SIGNAL( stateChanged() ), this, SLOT( drawWidgetAndRepaint() ) );
  if (KomposeViewManager::instance()->hasActiveView())
  {
    parent()->removeChild( this );
    close();
  }
}

void KomposeTaskWidget::resizeEvent ( QResizeEvent * e )
{
  if ( e->oldSize() != e->size())
  {
    prefWidget->move(width() - prefWidget->width() - 3, 3);
    drawWidget();
  }
  KomposeWidget::resizeEvent( e );
}


void KomposeTaskWidget::paintEvent ( QPaintEvent * e )
{
  if ( pm_dbBackground.isNull() )
    drawWidget();
  bitBlt( this, 0, 0, &pm_dbBackground, 0, 0, width(), height() );
}

void KomposeTaskWidget::drawWidgetAndRepaint()
{
  drawWidget();
  repaint();
}

void KomposeTaskWidget::drawWidget()
{
  if ( !KomposeViewManager::instance()->hasActiveView() || !isShown() )
    return;

  pm_dbBackground.resize( width(), height() );
  //pm_dbBackground.fill(white);

  QPainter p( &pm_dbBackground );
  
  int effect = IEFFECT_NONE;

  if ( KomposeSettings::instance()->getShowWindowTitles() && !task->isIconified() )
    effect = IEFFECT_TITLE;
  if ( KomposeSettings::instance()->getShowWindowTitles() && task->isIconified() )
    effect = IEFFECT_MINIMIZED_AND_TITLE;
  if ( !KomposeSettings::instance()->getShowWindowTitles() && task->isIconified() )
    effect = IEFFECT_MINIMIZED;
  //   if ( highlight )               // I hate it, so I disable it!
  //     effect = IEFFECT_HIGHLIGHT;

  task->getVisualizer()->renderOnPixmap(&pm_dbBackground, effect);

  // Icon
  QPoint titleTextPos( 6, KomposeSettings::instance()->getWindowTitleFontHeight() + 1);
  if ( KomposeSettings::instance()->getShowIcons() )
  {
    QPixmap icon = task->getIcon( KomposeSettings::instance()->getIconSizePixels() );

    // Place the icon left or under the text, according to it's size
    if ( KomposeSettings::instance()->getIconSize() < 2 || icon.height() < 50 )
    {
      // Medium sized or tiny Icon
      p.drawPixmap( QPoint(5, 5), icon );
      titleTextPos.setX(icon.width() + 10);
    }
    else
    {
      // Big Icon
      p.drawPixmap( QPoint(5, 5), icon );
      //       titleTextPos.setX(icon.width());
    }
  }

  // Title
  if ( KomposeSettings::instance()->getShowWindowTitles() )
  {
    p.setFont(titleFont);
    if ( KomposeSettings::instance()->getShowWindowTitleShadow() )
    {
      p.setPen( KomposeSettings::instance()->getWindowTitleFontShadowColor() );
      p.drawText( titleTextPos, task->visibleNameWithState() );
    }
    p.setPen( KomposeSettings::instance()->getWindowTitleFontColor() );
    p.drawText( QPoint( titleTextPos.x()-2, titleTextPos.y()-2 ), task->visibleNameWithState() );
  }

  // Border
  if (!highlight)
  {
    QPen     pen( gray, 1, Qt::SolidLine );
    p.setPen( pen );
    p.drawRect(rect());
  }
  else
  {
    QPen     pen( black, 2, Qt::DotLine );
    p.setPen( pen );
    p.drawRect( QRect( 1, 1, rect().width()-1, rect().height()-1 ) );
  }

  p.end();
}

void KomposeTaskWidget::mouseReleaseEvent ( QMouseEvent * e )
{
  if ( !rect().contains( e->pos() ) )
    return;
  KomposeViewManager::instance()->activateTask( task );
}

void KomposeTaskWidget::mouseMoveEvent ( QMouseEvent * e )
{
  if ( e->state() == Qt::LeftButton )
    startDrag();
}

// void KomposeTaskWidget::mouseDoubleClickEvent ( QMouseEvent * e )
// {
//   if ( !rect().contains( e->pos() ) )
//     return;
//   KomposeTaskManager::instance()->activateTask( task );
// }

void KomposeTaskWidget::keyReleaseEvent ( QKeyEvent * e )
{
  if ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Space )
  {
    qDebug("KomposeTaskWidget::keyReleaseEvent - activating Task!");
    KomposeViewManager::instance()->activateTask( task );
    e->accept();
  }
  else if ( e->key() == Qt::Key_C )
  {
    qDebug("KomposeTaskWidget::keyReleaseEvent - closing Task!");
    KomposeViewManager::instance()->activateTask( task ); //FIXME: do sth usefull
    e->accept();
  }
  else if ( e->key() == Qt::Key_M )
  {
    qDebug("KomposeTaskWidget::keyReleaseEvent - toggling state!");
    KomposeViewManager::instance()->activateTask( task ); //FIXME: do sth usefull
    e->accept();
  }
  else
  {
    qDebug("KomposeTaskWidget::keyReleaseEvent - ignored...");
    e->ignore();
  }
  KomposeWidget::keyReleaseEvent(e);
}

void KomposeTaskWidget::leaveEvent ( QEvent * e )
{
  highlight = false;
  unsetCursor();
  drawWidgetAndRepaint();

  prefWidget->hide();
  if ( parentWidget() )
    parentWidget()->setFocus();
}

void KomposeTaskWidget::enterEvent ( QEvent * e )
{
  setFocus();
  setCursor( Qt::PointingHandCursor );
  highlight = true;
  drawWidgetAndRepaint();

  prefWidget->show();
}

void KomposeTaskWidget::focusInEvent ( QFocusEvent * e)
{
  highlight = true;
  drawWidgetAndRepaint();
}

void KomposeTaskWidget::focusOutEvent ( QFocusEvent * e)
{
  highlight = false;
  drawWidgetAndRepaint();
}

int KomposeTaskWidget::getHeightForWidth ( int w ) const
{
  //qDebug("KomposeTaskWidget::getHeightForWidth()");
  return task->getHeightForWidth(w);
}

int KomposeTaskWidget::getWidthForHeight ( int h ) const
{
  //qDebug("KomposeTaskWidget::getWidthForHeight()");
  return task->getWidthForHeight(h);
}

double KomposeTaskWidget::getAspectRatio()
{
  //qDebug("KomposeTaskWidget::getAspectRatio()");
  return task->getAspectRatio();
}

void KomposeTaskWidget::startDrag()
{
  // The text is just a dummy
  // FIXME: Maybe we should us a "Task"-Mimetype
  QDragObject *d = new QTextDrag( "toDesktop()", this );
  d->dragCopy();
}

void KomposeTaskWidget::setGeometry( const QRect &rect )
{
  int width = task->getGeometry().width();
  int height = task->getGeometry().height();
  
  // Don't scale images bigger than they actually are
  if ( rect.width() > width || rect.height() > height )
  {
    QWidget::setGeometry( QRect( rect.left(), rect.top(), width, height ) );
  } else
    QWidget::setGeometry( rect );
}



#include "komposetaskwidget.moc"
