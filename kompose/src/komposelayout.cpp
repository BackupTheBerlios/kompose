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
#include "komposelayout.h"

#include "komposewidgetinterface.h"
#include "komposetaskmanager.h"
#include "komposefullscreenwidget.h"

#include <math.h>

#include <kwin.h>


KomposeLayout::KomposeLayout( KomposeWidgetInterface *parent, int type, int dist, const char *name )
    : QObject(),
    layoutType(type),
    spacing(dist),
    widgetsChanged(false),
    parentWidget(parent)
{
  currentSize = QSize( 1, 1 );
}

KomposeLayout::~KomposeLayout()
{}

void KomposeLayout::add( KomposeWidgetInterface *w )
{
  //qDebug("KomposeLayout::add()@%s - Added widget to layout", parent()->className());
  qDebug("KomposeLayout::add() - Added widget to layout");
  list.append( w );
  widgetsChanged = true;
}

void KomposeLayout::remove( KomposeWidgetInterface *w )
{
  list.remove( w );
  widgetsChanged = true;
}


void KomposeLayout::arrangeLayout()
{
  qDebug("KomposeLayout::arrangeLayout()");
  rearrangeContents();
  //mainWidget()->setUpdatesEnabled( FALSE );
  //rearrangeContents();
  //mainWidget()->setUpdatesEnabled( TRUE );
}

void KomposeLayout::rearrangeContents()
{
  // Check if we have a valid view (so we don't rearrange when no view is shown)
  if ( !KomposeTaskManager::instance()->hasActiveView() )
    return;
  // Check or empty list
  if (list.count() == 0)
  {
    qDebug("KomposeLayout::rearrangeContents() - empty list... skipping!");
    return;
  }


  // Check for layout Type and do the work
  if (layoutType==TLAYOUT_TASKCONTAINERS)
  {
    QPtrList<KomposeWidgetInterface> filledContainers;
    QPtrList<KomposeWidgetInterface> emptyContainers;

    // Check for empty containers
    QPtrListIterator<KomposeWidgetInterface> it( list );
    KomposeWidgetInterface *task;
    while ( (task = it.current()) != 0 )
    {
      ++it;
      KomposeTaskContainerWidget *containerTask = dynamic_cast<KomposeTaskContainerWidget*>(task);

      if ( containerTask->getNumofChilds() > 0 )
      {
        filledContainers.append( containerTask );
      }
      else
      {
        emptyContainers.append( containerTask );
      }
    }

    // Arrange filled containers
    QRect filledRect( parentWidget->getRect().x(),
                      parentWidget->getRect().y(),
                      parentWidget->getRect().width(),
                      parentWidget->getRect().height() - ( 40 + 2*spacing ) );
    // arrange the filled desktops taking 90% of the screen
    rearrangeContents( filledRect, filledContainers );

    // Arrange empty containers
    QRect emptyRect( parentWidget->getRect().x(),
                     parentWidget->getRect().y() + parentWidget->getRect().height() - ( 40 + 2*spacing ),
                     parentWidget->getRect().width(),
                     ( 40 + 2*spacing ) );
    // arrange the empty widget in one row
    rearrangeContents( emptyRect, emptyContainers, 1 );


  }
  else  // default type (generic)
  {
    rearrangeContents( parentWidget->getRect(), list );
  }


  currentSize = parentWidget->getSize();
  widgetsChanged = false;
}


/**
 * availRect specifies the size&pos of the contents
 * Specify either rows or cols to set a fixed number of those (setting both won't work correctly)
 */
void KomposeLayout::rearrangeContents( const QRect& availRect, const QPtrList<KomposeWidgetInterface> widgets, int rows, int columns )
{
  // Check or empty list
  if (widgets.count() == 0)
  {
    qDebug("KomposeLayout::rearrangeContents() - empty list... skipping!");
    return;
  }
  
  QPtrListIterator<KomposeWidgetInterface> it( widgets );

  // Calculate grid's rows & cols
  if ( rows != -1 )         // rows have been specified
  {
    columns = ceil(widgets.count() / rows);
  }
  else if ( columns != -1 ) // columns have been specified
  {
    rows = ceil(widgets.count() / columns);
  }
  else                      // neither rows nor cols have been specified
  {
    double parentRatio = (double)parentWidget->getRect().width() / (double)parentWidget->getRect().height();
    // Use more columns than rows when parent's width > parent's height
    if ( parentRatio > 1 )
    {
      columns = ceil( sqrt(widgets.count()) );
      rows = ceil( (double)widgets.count() / (double)columns );
    } else {
      rows = ceil( sqrt(widgets.count()) );
      columns = ceil( (double)widgets.count() / (double)rows );
    }
  }

  qDebug("KomposeLayout::rearrangeContents() - Relayouting %d child widgets with %d rows & %d columns", widgets.count(), rows, columns);

  // Calculate width & height
  int w = (availRect.width() - (columns+1) * spacing ) / columns;
  int h = (availRect.height() - (rows+1) * spacing ) / rows;

  // Process rows
  for ( int i=0; i<rows; ++i )
  {
    // Process columns
    for ( int j=0; j<columns; ++j )
    {
      KomposeWidgetInterface *task;

      // Check for end of List
      if ( (task = it.current()) == 0)
        break;

      // Calculate width and height of widget
      double ratio = task->getAspectRatio();

      int widgetw = 100;
      int widgeth = 100;

      if ( ratio == -1 )
      {
        widgetw = w;
        widgeth = h;
      }
      else
      {
        double widthForHeight = task->getWidthForHeight(h);
        double heightForWidth = task->getHeightForWidth(w);
        if ( (ratio >= 1.0 && heightForWidth <= h) ||
             (ratio < 1.0 && widthForHeight > w)   )
        {
          widgetw = w;
          widgeth = heightForWidth;
        }
        else if ( (ratio < 1.0 && widthForHeight <= w) ||
                  (ratio >= 1.0 && heightForWidth > h)   )
        {
          widgeth = h;
          widgetw = widthForHeight;
        }
      }

      // Set the Widget's size
      QRect geom( availRect.x() + j * (w + spacing) + spacing,
                  availRect.y() + i * (h + spacing) + spacing,
                  widgetw, widgeth );
      qDebug("KomposeLayout::rearrangeContents() - Put item %s at x: %d y: %d with size: %dx%d",
             (dynamic_cast<QObject*>(task))->className(), geom.x(), geom.y(), widgetw, widgeth );
      task->setGeom( geom );

      ++it;
    }
  }
}

#include "komposelayout.moc"
