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
#include "komposedesktopwidget.h"
#include "komposelayout.h"
#include "komposetask.h"
#include "komposetaskwidget.h"
#include "komposetaskmanager.h"
#include "komposesettings.h"

#include <qdesktopwidget.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qdragobject.h>
#include <qcursor.h>

#include <krootpixmap.h>
#include <kwin.h>


KomposeDesktopWidget::KomposeDesktopWidget(int desktop, QWidget *parent, KomposeLayout *l, const char *name)
    : KomposeWidget( parent, l, name ),
    deskNum( desktop )
{
  // Retrieve geometry
  QDesktopWidget *deskwidget = new QDesktopWidget();
  deskRect = deskwidget->availableGeometry();
  delete deskwidget;

  rootpix = new KRootPixmap (this);
  if ( KomposeSettings::instance()->getTintVirtDesks() )
    rootpix->setFadeEffect( 0.05, KomposeSettings::instance()->getTintVirtDesksColor() );

  setAcceptDrops(TRUE);

  createTaskWidgets();
  
  rootpix->start();
  
  connect( KomposeTaskManager::instance(), SIGNAL( newTask( KomposeTask* ) ), this, SLOT( createTaskWidget( KomposeTask* ) ) );
}


KomposeDesktopWidget::~KomposeDesktopWidget()
{}


void KomposeDesktopWidget::createTaskWidgets()
{
  TaskList tl = KomposeTaskManager::instance()->getTasks();
  qDebug("KomposeDesktopWidget::createTaskWidgets() on %d tasks", tl.count());
  QPtrListIterator<KomposeTask> it( tl );
  KomposeTask *task;
  while ( (task = it.current()) != 0 )
  {
    ++it;
    Q_CHECK_PTR(task);
    createTaskWidget( task );
  }
}


void KomposeDesktopWidget::createTaskWidget( KomposeTask* task )
{
    if (deskNum == task->onDesktop()-1 )
    {
      qDebug("KomposeDesktopWidget::createTaskWidget()" );
      KomposeTaskWidget *taskwidget = new KomposeTaskWidget( task, this, 0, "kjh" );
    }
}

int KomposeDesktopWidget::getHeightForWidth( int w ) const
{
  qDebug("KomposeDesktopWidget::getHeightForWidth()");
  return ((double)w / (double)deskRect.width()) * deskRect.height();
}

int KomposeDesktopWidget::getWidthForHeight( int h ) const
{
  qDebug("KomposeDesktopWidget::getWidthForHeight()");
  return ((double)h / (double)deskRect.height()) * deskRect.width();
}

double KomposeDesktopWidget::getAspectRatio()
{
  qDebug("KomposeDesktopWidget::getAspectRatio()");
  return (double)deskRect.width() / (double)deskRect.height();
}


void KomposeDesktopWidget::leaveEvent ( QEvent * e )
{
  unsetCursor();
  repaint();
}

void KomposeDesktopWidget::enterEvent ( QEvent * e )
{
  setCursor( Qt::PointingHandCursor );
  repaint();
}

void KomposeDesktopWidget::mouseReleaseEvent ( QMouseEvent * e )
{
  KomposeTaskManager::instance()->setCurrentDesktop(deskNum);
}


void KomposeDesktopWidget::mouseDoubleClickEvent ( QMouseEvent * e )
{
  KomposeTaskManager::instance()->setCurrentDesktop(deskNum);
}


void KomposeDesktopWidget::paintEvent ( QPaintEvent * e )
{
  KomposeWidget::paintEvent(e);

  QPainter p;
  p.begin( this );
  if (!hasMouse())
    p.setPen( QColor(gray));
  else
    p.setPen( QColor(black));
  p.drawRect(rect());
  p.end();
}

void KomposeDesktopWidget::dragEnterEvent( QDragEnterEvent* e )
{
  qDebug("KomposeDesktopWidget::dragEnterEvent");
  e->accept( QTextDrag::canDecode(e) );
}

void KomposeDesktopWidget::dropEvent ( QDropEvent * e )
{
  QString text;
  if ( QTextDrag::decode(e, text) )
  {
    qDebug("KomposeDesktopWidget::dropEvent - Received Task drop");
    KomposeTaskWidget* dropWidget = dynamic_cast<KomposeTaskWidget*>(e->source());
    dropWidget->getTask()->toDesktop( deskNum + 1);
    if ( dropWidget->parentWidget() != this)
      dropWidget->reparent( this, QPoint(0,0), true );

    e->accept();
  }
}


#include "komposedesktopwidget.moc"
