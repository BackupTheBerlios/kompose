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
#include "komposetask.h"
#include "komposetaskprefswidget.h"
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

#include <kimageeffect.h>


KomposeTaskWidget::KomposeTaskWidget(KomposeTask *t, QWidget *parent, KomposeLayout *l, const char *name)
    : KomposeWidget(parent, l, name),
    task(t),
    scaledScreenshot(0),
    scaledMinimizedScreenshot(0),
    scaledSelectedScreenshot(0),
    highlight(false)
{
  setWFlags( WStaticContents | WRepaintNoErase | WResizeNoErase );  // avoid redraw errors
  setBackgroundMode( Qt::NoBackground );  // avoid flickering

  QToolTip::add( this, task->visibleNameWithState() );

  prefWidget = new KomposeTaskPrefsWidget( this, "Task prefs" );
  prefWidget->hide();

  //connect( t, SIGNAL( destroyed() ), this, SLOT( slotTaskDestroyed() ) );
  connect( t, SIGNAL( closed() ), this, SLOT( slotTaskDestroyed() ) );
  connect( t, SIGNAL( stateChanged() ), this, SLOT( repaint() ) );

  setFocusPolicy(QWidget::StrongFocus);
}


KomposeTaskWidget::~KomposeTaskWidget()
{
}

void KomposeTaskWidget::slotTaskDestroyed()
{
  disconnect( task, SIGNAL( closed() ), this, SLOT( slotTaskDestroyed() ) );
  disconnect( task, SIGNAL( stateChanged() ), this, SLOT( repaint() ) );
  if (KomposeTaskManager::instance()->hasActiveView())
  {
    parent()->removeChild( this );
    close();
  }
}

void KomposeTaskWidget::scaleScreenshot()
{
  if ( width() == scaledScreenshot.width() && height() == scaledScreenshot.height() )
  {
    qDebug("KomposeTaskWidget::scaleScreenshot() - no change in size... won't scale %s", name());
    return;
  }

  qDebug("KomposeTaskWidget::scaleScreenshot() - scaling... %s to %dx%d", name(), width(), height());
  //scaledScreenshot = task->getScreenshotImg().copy();
  scaledScreenshot = task->getScreenshotImg().smoothScale( size() );
  //ImageEffect::scale( scaledScreenshot, width(), height() );

  scaledMinimizedScreenshot = scaledScreenshot.copy();
  KImageEffect::intensity( scaledMinimizedScreenshot, -0.3 );

  if ( KomposeSettings::instance()->getHighlightWindows() )
  {
    scaledSelectedScreenshot = scaledScreenshot.copy();
    KImageEffect::selectedImage( scaledSelectedScreenshot, Qt::white );
  }

  show();
  repaint();
}


void KomposeTaskWidget::resizeEvent ( QResizeEvent * e )
{
  if ( e->oldSize() != e->size())
  {
    // Delaying scaling as we want it to block everything
    QTimer::singleShot( 200, this, SLOT( scaleScreenshot() ) );
    //scaleScreenshot();
    prefWidget->move(width() - prefWidget->width() - 3, 3);
  }
}


void KomposeTaskWidget::paintEvent ( QPaintEvent * e )
{
  if ( !KomposeTaskManager::instance()->hasActiveView() )
    return;

  //QWidget::paintEvent(e);
  QPainter p;

  int xpos = ( width() - scaledScreenshot.width() ) / 2;
  int ypos = ( height() - scaledScreenshot.height() ) / 2;

  p.begin( this );
  if ( highlight && KomposeSettings::instance()->getHighlightWindows() )
  {
    //qDebug("KomposeTaskWidget::paintEvent - Painting highlighted window");
    p.drawImage( xpos,ypos, scaledSelectedScreenshot);
  }
  else
  {
    if ( task->isIconified() )
    {
      // qDebug("KomposeTaskWidget::paintEvent - Painting minimized window");
      p.drawImage( xpos,ypos, scaledMinimizedScreenshot);
    }
    else
    {
      // qDebug("KomposeTaskWidget::paintEvent - Painting normal window");
      p.drawImage( xpos,ypos, scaledScreenshot);
    }
  }
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
  KomposeTaskManager::instance()->activateTask( task );
}

void KomposeTaskWidget::mouseMoveEvent ( QMouseEvent * e )
{
  if ( e->state() == Qt::LeftButton )
    startDrag();
}

void KomposeTaskWidget::mouseDoubleClickEvent ( QMouseEvent * e )
{
  if ( !rect().contains( e->pos() ) )
    return;
  KomposeTaskManager::instance()->activateTask( task );
}

void KomposeTaskWidget::keyReleaseEvent ( QKeyEvent * e )
{
  if ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Space )
  {
    qDebug("KomposeTaskWidget::keyReleaseEvent - activating Task!");
    KomposeTaskManager::instance()->activateTask( task );
    e->accept();
  }
  else if ( e->key() == Qt::Key_C )
  {
    qDebug("KomposeTaskWidget::keyReleaseEvent - closing Task!");
    KomposeTaskManager::instance()->activateTask( task );
    e->accept();
  }
  else if ( e->key() == Qt::Key_M )
  {
    qDebug("KomposeTaskWidget::keyReleaseEvent - toggling state!");
    KomposeTaskManager::instance()->activateTask( task );
    e->accept();
  }
  else
  {
    qDebug("KomposeTaskWidget::keyReleaseEvent - ignored...");
    e->ignore();
  }

}

void KomposeTaskWidget::leaveEvent ( QEvent * e )
{
  highlight = false;
  unsetCursor();
  repaint();

  prefWidget->hide();

}

void KomposeTaskWidget::enterEvent ( QEvent * e )
{
  setFocus();
  setCursor( Qt::PointingHandCursor );
  highlight = true;
  repaint();

  prefWidget->show();
}

void KomposeTaskWidget::focusInEvent ( QFocusEvent * e)
{
  highlight = true;
  repaint();
}

void KomposeTaskWidget::focusOutEvent ( QFocusEvent * e)
{
  highlight = false;
  repaint();
}

int KomposeTaskWidget::getHeightForWidth ( int w ) const
{
  qDebug("KomposeTaskWidget::getHeightForWidth()");
  return task->getHeightForWidth(w);
}

int KomposeTaskWidget::getWidthForHeight ( int h ) const
{
  qDebug("KomposeTaskWidget::getWidthForHeight()");
  return task->getWidthForHeight(h);
}

double KomposeTaskWidget::getAspectRatio()
{
  qDebug("KomposeTaskWidget::getAspectRatio()");
  return task->getAspectRatio();
}

void KomposeTaskWidget::startDrag()
{
  // The text is just a dummy
  // FIXME: Maybe we should us a "Task"-Mimetype
  QDragObject *d = new QTextDrag( "toDesktop()", this );
  d->dragCopy();
}


#include "komposetaskwidget.moc"
