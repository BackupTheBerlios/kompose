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
    spacing(dist),
    widgetsChanged(false),
    parentWidget(parent),
    currentRows(0),
    currentColumns(0)
{
  setType( type );
  currentSize = QSize( 1, 1 );
}

KomposeLayout::~KomposeLayout()
{}

void KomposeLayout::setType( int t )
{
  if ( t == TLAYOUT_TASKCONTAINERS &&
       !KomposeSettings::instance()->getDynamicVirtDeskLayout() )
    t = TLAYOUT_GENERIC;
  layoutType = t;
}


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
    filledContainers.clear();
    emptyContainers.clear();

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
    QRect filledRect( 0,
                      0,
                      parentWidget->getSize().width(),
                      parentWidget->getSize().height() - ( 40 + 2*spacing ) );
    // arrange the filled desktops taking 90% of the screen
    rearrangeContents( filledRect, filledContainers );

    // Arrange empty containers
    QRect emptyRect( 0,
                     parentWidget->getSize().height() - ( 40 + 2*spacing ),
                     parentWidget->getSize().width(),
                     ( 40 + 2*spacing ) );
    // arrange the empty widget in one row
    rearrangeContents( emptyRect, emptyContainers, 1, -1, false );


  }
  else  // default type (generic)
  {
    QRect availRect( 0,
                     0,
                     parentWidget->getSize().width(),
                     parentWidget->getSize().height());
    rearrangeContents( availRect, list );
  }


  currentSize = parentWidget->getSize();
  widgetsChanged = false;
}


/**
 * availRect specifies the size&pos of the contents
 * Specify either rows or cols to set a fixed number of those (setting both won't work correctly)
 */
void KomposeLayout::rearrangeContents( const QRect& availRect, const QPtrList<KomposeWidgetInterface> widgets, int rows, int columns, bool setMemberRowsCols )
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
    double parentRatio = (double)availRect.width() / (double)availRect.height();
    // Use more columns than rows when parent's width > parent's height
    if ( parentRatio > 1 )
    {
      columns = ceil( sqrt(widgets.count()) );
      rows = ceil( (double)widgets.count() / (double)columns );
    }
    else
    {
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

  // Sync cols/rows member vars to current cols/rows
  if ( setMemberRowsCols )
  {
    currentRows = rows;
    currentColumns = columns;
  }
}


/*
 * Search for neighbour (called from outside)
 */
KomposeWidgetInterface* KomposeLayout::getNeighbour( const KomposeWidgetInterface* widget, int direction, int wrap )
{
  qDebug("KomposeLayout::getNeighbour() - Called with list.count: %d", list.count());

  if (layoutType==TLAYOUT_TASKCONTAINERS)
  {
    KomposeWidgetInterface* neighbour;
    if ( filledContainers.containsRef(widget) )
    {
      if ( ( neighbour = getNeighbour( filledContainers, widget, direction, WLAYOUT_HORIZONTAL ) ) == 0 )
        return emptyContainers.first();
    }
    else if ( emptyContainers.containsRef(widget) )
    {
      if ( ( neighbour = getNeighbour( emptyContainers, widget, direction, WLAYOUT_HORIZONTAL ) ) == 0 )
        if ( direction == DLAYOUT_TOP )
          return filledContainers.last();
        else
          return filledContainers.first();
    }
    return neighbour;
  }
  else if (layoutType==TLAYOUT_GENERIC)
    return getNeighbour( list, widget, direction, wrap );

  qDebug("KomposeLayout::getNeighbour() - this should never happen!");
  return NULL;
}


/*
 * Search for neighbour in the given list (called from inside)
 */
KomposeWidgetInterface* KomposeLayout::getNeighbour(
  QPtrList<KomposeWidgetInterface> listToSearch,
  const KomposeWidgetInterface* widget,
  int direction,
  int wrap )
{
  QPtrListIterator<KomposeWidgetInterface> it( listToSearch );

  KomposeWidgetInterface *task;
  KomposeWidgetInterface *neighbour;
  int index = 0;

  if (widget == 0)
  {
    qDebug("KomposeLayout::getNeighbour() - NULL startWidget given. using first()");
    return listToSearch.first();
  }

  while ( (task = it.current()) != 0 )
  {
    if ( task == widget )
    {
      switch ( direction )
      {
      case DLAYOUT_RIGHT:
        ++it;
        if ( (index)%currentColumns == currentColumns-1 || ( neighbour = it.current() ) == 0 )
        {
          if (wrap == WLAYOUT_HORIZONTAL || wrap == WLAYOUT_BOTH )
            return listToSearch.at(index+1-currentColumns);
          qDebug("KomposeLayout::getNeighbour() - No valid neighbour available");
          return NULL;
        }
        return neighbour;
      case DLAYOUT_LEFT:
        --it;
        if ( index%currentColumns == 0 || ( neighbour = it.current() ) == 0 )
        {
          if (wrap == WLAYOUT_HORIZONTAL || wrap == WLAYOUT_BOTH )
            if ( index+currentColumns-1 < listToSearch.count() )
              return listToSearch.at(index+currentColumns-1);
            else
              return listToSearch.last();
          qDebug("KomposeLayout::getNeighbour() - No valid neighbour available");
          return NULL;
        }
        return neighbour;
      case DLAYOUT_TOP:
        if ( index < currentColumns || (neighbour = listToSearch.at( index - currentColumns )) == 0)
        {
          if (wrap == WLAYOUT_VERTICAL || wrap == WLAYOUT_BOTH )
            if ( listToSearch.count()%currentColumns == 0 || listToSearch.count()%currentColumns > index )
              return listToSearch.at( (currentRows-1)*currentColumns + index );
            else
              return listToSearch.at( (currentRows-2)*currentColumns + index );
          qDebug("KomposeLayout::getNeighbour() - No valid neighbour available");
          return NULL;
        }
        return neighbour;
      case DLAYOUT_BOTTOM:
        if ( listToSearch.count() <= index + currentColumns || (neighbour = listToSearch.at( index + currentColumns )) == 0)
        {
          if (wrap == WLAYOUT_VERTICAL || wrap == WLAYOUT_BOTH )
            return listToSearch.at( index%currentColumns );
          qDebug("KomposeLayout::getNeighbour() - No valid neighbour available");
          return NULL;
        }
        return neighbour;
      }
    }
    ++index;
    ++it;
  }

  qDebug("KomposeLayout::getNeighbour() - this should never happen!");
  return NULL;
}


#include "komposelayout.moc"
