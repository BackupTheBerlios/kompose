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


KomposeLayout::KomposeLayout( KomposeWidgetInterface *parent, int dist, const char *name )
    : QObject(),
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
  QPtrListIterator<KomposeWidgetInterface> it( list );
  int sizeOfList = it.count();

  if ( !KomposeTaskManager::instance()->hasActiveView() )
    return;

  // Check or empty list
  if (sizeOfList == 0)
  {
    qDebug("KomposeLayout::rearrangeContents() - empty list... skipping!");
    return;
  }
  // Calculate grid's rows & cols
  double dsqrt = sqrt( (double)sizeOfList );
  int isqrt = sqrt( sizeOfList );
  int rows = sqrt(sizeOfList);
  int columns = rows;

  if ( dsqrt - (double)isqrt != 0.0 )
  {
    if ( sizeOfList <= 2 )
    {
      rows = 1;
      columns = 2;
    }
    else if ( sizeOfList <= 4 )
    {
      rows = 2;
      columns = 2;
    }
    else if ( sizeOfList <= 6 )
    {
      rows = 2;
      columns = 3;
    }
    else if ( sizeOfList <= 9 )
    {
      rows = 3;
      columns = 3;
    }
    else if ( sizeOfList <= 12 )
    {
      rows = 3;
      columns = 4;
    }
    else if ( sizeOfList <= 16 )
    {
      rows = 4;
      columns = 4;
    }
    else
    {
      rows = 5;
      columns = 5;
    }
  }


  qDebug("KomposeLayout::rearrangeContents() - Relayouting %d child widgets with %d rows & %d columns", it.count(), rows, columns);

  // Calculate width & height
  int w = (parentWidget->getRect().width() - (columns+1) * spacing ) / columns;
  int h = (parentWidget->getRect().height() - (rows+1) * spacing ) / rows;
  //   Q_CHECK_PTR(parentWidget);
  qDebug("KomposeLayout::rearrangeContents() - Parent size: %dx%d", w, h);

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
      // qDebug("KomposeLayout::rearrangeContents() - Relayouting %s", ((QObject*)task)->className() );

      // Calculate width and height of widget
      double ratio = task->getAspectRatio();

      int widgetw = 100;
      int widgeth = 100;

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


      // Set the Widget's size
      QRect geom( parentWidget->getRect().x() + j * (w + spacing) + spacing,
                  parentWidget->getRect().y() + i * (h + spacing) + spacing,
                  widgetw, widgeth );
      qDebug("KomposeLayout::rearrangeContents() - Put item %s at x: %d y: %d with size: %dx%d",
             (dynamic_cast<QObject*>(task))->className(), geom.x(), geom.y(), widgetw, widgeth );
      task->setGeom( geom );

      ++it;
    }
  }

  currentSize = parentWidget->getSize();
  widgetsChanged = false;
}

#include "komposelayout.moc"
