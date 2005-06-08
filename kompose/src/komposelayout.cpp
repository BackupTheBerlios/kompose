/***************************************************************************
 *   Copyright (C) 2004 by Hans Oischinger                                 *
 *   hans.oischinger@kde-mail.net                                                 *
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

#include "komposewidget.h"
#include "komposeviewmanager.h"
#include "komposetaskmanager.h"
#include "komposefullscreenwidget.h"

#include <math.h>

#include <kwin.h>
#include <kdebug.h>


KomposeLayout::KomposeLayout( KomposeWidget *parent, int type, int dist, const char *name )
    : QObject(parent, name),
    spacing(dist),
    widgetsChanged(false),
    currentRows(0),
    currentColumns(0),
    parentWidget(parent)
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


void KomposeLayout::add( KomposeWidget *w )
{
  //kdDebug() << "KomposeLayout::add()@%s - Added widget to layout", parent()->className());
  //kdDebug() << "KomposeLayout::add() - Added widget to layout");
  list.append( w );
  widgetsChanged = true;
}

void KomposeLayout::remove( KomposeWidget *w )
{
  list.remove( w );
  widgetsChanged = true;
}


void KomposeLayout::arrangeLayout()
{
  //kdDebug() << "KomposeLayout::arrangeLayout()");
  rearrangeContents();
}

void KomposeLayout::rearrangeContents()
{
  // Check or empty list
  if (list.count() == 0)
  {
    kdDebug() << "KomposeLayout::rearrangeContents() - empty list... skipping!" << endl;
    return;
  }


  // Check for layout Type and do the work
  if (layoutType==TLAYOUT_TASKCONTAINERS)
  {
    filledContainers.clear();
    emptyContainers.clear();

    // Check for empty containers
    QPtrListIterator<KomposeWidget> it( list );
    KomposeWidget *task;
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
                      parentWidget->width(),
                      parentWidget->height() - ( 40 + 2*spacing ) );
    // arrange the filled desktops taking 90% of the screen
    rearrangeContents( filledRect, filledContainers );

    // Arrange empty containers
    QRect emptyRect( 0,
                     parentWidget->height() - ( 40 + 2*spacing ),
                     parentWidget->width(),
                     ( 40 + 2*spacing ) );
    // arrange the empty widget in one row
    rearrangeContents( emptyRect, emptyContainers, 1, -1, false );


  }
  else  // default type (generic)
  {
    QRect availRect( 0,
                     0,
                     parentWidget->width(),
                     parentWidget->height());
    rearrangeContents( availRect, list );
  }


  currentSize = parentWidget->size();
  widgetsChanged = false;
}


/**
 * availRect specifies the size&pos of the contents
 * Specify either rows or cols to set a fixed number of those (setting both won't work correctly)
 */
void KomposeLayout::rearrangeContents( const QRect& availRect, const QPtrList<KomposeWidget> widgets, int rows, int columns, bool setMemberRowsCols )
{
  // Check or empty list
  if (widgets.count() == 0)
  {
    kdDebug() << "KomposeLayout::rearrangeContents() - empty list... skipping!" << endl;
    return;
  }

  QPtrListIterator<KomposeWidget> it( widgets );

  // Calculate grid's rows & cols
  if ( rows != -1 )         // rows have been specified
  {
    columns = (int)ceil(widgets.count() / rows);
  }
  else if ( columns != -1 ) // columns have been specified
  {
    rows = (int)ceil(widgets.count() / columns);
  }
  else                      // neither rows nor cols have been specified
  {
    double parentRatio = (double)availRect.width() / (double)availRect.height();
    // Use more columns than rows when parent's width > parent's height
    if ( parentRatio > 1 )
    {
      columns = (int)ceil( sqrt(widgets.count()) );
      rows = (int)ceil( (double)widgets.count() / (double)columns );
    }
    else
    {
      rows = (int)ceil( sqrt(widgets.count()) );
      columns = (int)ceil( (double)widgets.count() / (double)rows );
    }
  }

  kdDebug() << "KomposeLayout::rearrangeContents() - Relayouting " << widgets.count() << " child widgets with " << rows << " rows & " << columns << " columns" << endl;

  // Calculate width & height
  int w = (availRect.width() - (columns+1) * spacing ) / columns;
  int h = (availRect.height() - (rows+1) * spacing ) / rows;

  QValueList<QRect> geometryRects;
  QValueList<int> maxRowHeights;
  // Process rows
  for ( int i=0; i<rows; ++i )
  {
    int xOffsetFromLastCol = 0;
    int maxHeightInRow = 0;
    // Process columns
    for ( int j=0; j<columns; ++j )
    {
      KomposeWidget *task;

      // Check for end of List
      if ( (task = it.current()) == 0)
        break;

      // Calculate width and height of widget
      double ratio = task->getAspectRatio();

      int widgetw = 100;
      int widgeth = 100;
      int usableW = w;
      int usableH = h;

      // use width of two boxes if there is no right neighbour
      if (it.atLast() && j!=columns-1)
      {
        usableW = 2*w;
      }
      ++it; // We need access to the neighbour in the following
      // expand if right neighbour has ratio < 1
      if (j!=columns-1 && it.current() && it.current()->getAspectRatio() < 1)
      {
        int addW = w - it.current()->getWidthForHeight(h);
        if ( addW > 0 )
        {
          usableW = w + addW;
        }
      }

      if ( ratio == -1 )
      {
        widgetw = w;
        widgeth = h;
      }
      else
      {
        double widthForHeight = task->getWidthForHeight(usableH);
        double heightForWidth = task->getHeightForWidth(usableW);
        if ( (ratio >= 1.0 && heightForWidth <= usableH) ||
             (ratio < 1.0 && widthForHeight > usableW)   )
        {
          widgetw = usableW;
          widgeth = (int)heightForWidth;
        }
        else if ( (ratio < 1.0 && widthForHeight <= usableW) ||
                  (ratio >= 1.0 && heightForWidth > usableH)   )
        {
          widgeth = usableH;
          widgetw = (int)widthForHeight;
        }
      }

      // Set the Widget's size

      int alignmentXoffset = 0;
      int alignmentYoffset = 0;
      if ( i==0 && h > widgeth )
        alignmentYoffset = h - widgeth;
      if ( j==0 && w > widgetw )
        alignmentXoffset = w - widgetw;
      QRect geom( availRect.x() + j * (w + spacing) + spacing + alignmentXoffset + xOffsetFromLastCol,
                  availRect.y() + i * (h + spacing) + spacing + alignmentYoffset,
                  widgetw, widgeth );
      geometryRects.append(geom);

      // Set the x offset for the next column
      if (alignmentXoffset==0)
        xOffsetFromLastCol += widgetw-w;
      if (maxHeightInRow < widgeth)
        maxHeightInRow = widgeth;
    }
    maxRowHeights.append(maxHeightInRow);
  }

  it.toFirst();
  QValueList<QRect>::iterator geomIt = geometryRects.begin();
  QValueList<int>::iterator maxRowHeightIt = maxRowHeights.begin();
  int topOffset = 0;
  for ( int i=0; i<rows; ++i )
  {
    // Process columns again
    for ( int j=0; j<columns; ++j )
    {
      KomposeWidget *task;
      if ( (task = it.current()) == 0)
        break;

      QRect geom = *geomIt;
      geom.setY( geom.y() + topOffset );
//       geom.setHeight( geom.height() - topOffset );
      task->setGeometry( geom );
      kdDebug() << "KomposeLayout::rearrangeContents() - Put item " << task->className() << " at x: " <<  geom.x() << " y: " <<  geom.y() << " with size: " <<  geom.width() << "x" << geom.height() << endl;
      ++geomIt;
      ++it;
    }
    if ( *maxRowHeightIt-h > 0 )
      topOffset += *maxRowHeightIt-h;
    ++maxRowHeightIt;
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
KomposeWidget* KomposeLayout::getNeighbour( const KomposeWidget* widget, int direction, int wrap )
{
  kdDebug() << "KomposeLayout::getNeighbour() - Called with list.count: " << list.count() << endl;

  if (layoutType==TLAYOUT_TASKCONTAINERS)
  {
    KomposeWidget* neighbour = 0;
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

  kdDebug() << "KomposeLayout::getNeighbour() - this should never happen!" << endl;
  return NULL;
}


/*
 * Search for neighbour in the given list (called from inside)
 */
KomposeWidget* KomposeLayout::getNeighbour(
  QPtrList<KomposeWidget> listToSearch,
  const KomposeWidget* widget,
  int direction,
  int wrap )
{
  QPtrListIterator<KomposeWidget> it( listToSearch );

  KomposeWidget *task;
  KomposeWidget *neighbour;
  int index = 0;

  if (widget == 0)
  {
    kdDebug() << "KomposeLayout::getNeighbour() - NULL startWidget given. using first()" << endl;
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
          kdDebug() << "KomposeLayout::getNeighbour() - No valid neighbour available" << endl;
          return NULL;
        }
        return neighbour;
      case DLAYOUT_LEFT:
        --it;
        if ( index%currentColumns == 0 || ( neighbour = it.current() ) == 0 )
        {
          if (wrap == WLAYOUT_HORIZONTAL || wrap == WLAYOUT_BOTH )
            if ( (uint)(index+currentColumns-1) < listToSearch.count() )
              return listToSearch.at(index+currentColumns-1);
            else
              return listToSearch.last();
          kdDebug() << "KomposeLayout::getNeighbour() - No valid neighbour available" << endl;
          return NULL;
        }
        return neighbour;
      case DLAYOUT_TOP:
        if ( index < currentColumns || (neighbour = listToSearch.at( index - currentColumns )) == 0)
        {
          if (wrap == WLAYOUT_VERTICAL || wrap == WLAYOUT_BOTH )
            if ( listToSearch.count()%currentColumns == 0 || listToSearch.count()%currentColumns > (uint)(index) )
              return listToSearch.at( (currentRows-1)*currentColumns + index );
            else
              return listToSearch.at( (currentRows-2)*currentColumns + index );
          kdDebug() << "KomposeLayout::getNeighbour() - No valid neighbour available" << endl;
          return NULL;
        }
        return neighbour;
      case DLAYOUT_BOTTOM:
        if ( listToSearch.count() <= (uint)(index + currentColumns) || (neighbour = listToSearch.at( index + currentColumns )) == 0)
        {
          if (wrap == WLAYOUT_VERTICAL || wrap == WLAYOUT_BOTH )
            return listToSearch.at( index%currentColumns );
          kdDebug() << "KomposeLayout::getNeighbour() - No valid neighbour available" << endl;
          return NULL;
        }
        return neighbour;
      }
    }
    ++index;
    ++it;
  }

  kdDebug() << "KomposeLayout::getNeighbour() - this should never happen!" << endl;
  return NULL;
}


#include "komposelayout.moc"
