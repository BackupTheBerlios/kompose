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
#include "komposewidget.h"
#include "komposelayout.h"

#include <qtimer.h>

/*
 * The constructor accepts a custom layout, if none is specified a defaultlayout is used
 */
KomposeWidget::KomposeWidget(QWidget *parent, KomposeLayout *l, const char *name)
    : QWidget(parent, name)
{
  if (!l)
    layout = new KomposeLayout( this );
  else
    layout = l;
}


KomposeWidget::~KomposeWidget()
{
  close();
  delete layout;
}


/*
 * Whenever a child is added to the widget it's also added to the layout
 */
void KomposeWidget::childEvent ( QChildEvent * ce)
{
  if ( !ce->child()->inherits("KomposeWidget") )
    return;

  if ( ce->inserted() )
  {
    qDebug("KomposeWidget::childEvent : Added widget %s to %s", ce->child()->className(), className() );
    layout->add( dynamic_cast<KomposeWidgetInterface*>(ce->child()) );
  }
  else if ( ce->removed() )
  {
    qDebug("KomposeWidget::childEvent : Removed widget %s from %s", ce->child()->className(), className() );
    layout->remove( dynamic_cast<KomposeWidgetInterface*>(ce->child()) );
  }

  // Whenever a child is added/removed: rearrange layout
  // FIXME: sometimes widget's aren't added in time, so we have to add a short delay:
  QTimer::singleShot( 200, layout, SLOT( arrangeLayout() ) );
}

/*
 * Whenever a resize occurs rearrange the layout
 */
void KomposeWidget::resizeEvent ( QResizeEvent * e )
{
  qDebug("KomposeWidget::resizeEvent - Size:%dx%d", e->size().width() , e->size().height());
  layout->arrangeLayout();
}


// Redirect these functions to QWidget

void KomposeWidget::setGeom ( const QRect &rect )
{
  QWidget::setGeometry( rect );
}

void KomposeWidget::setGeom ( const QSize &size )
{
  QWidget::resize( size );
}

QSize KomposeWidget::getSize() const
{
  return QWidget::size();
}

QRect KomposeWidget::getRect() const
{
  return QWidget::rect();
}

KomposeWidgetInterface* KomposeWidget::getParentWidget() const
{
  if ( QWidget::parent()->inherits("KomposeWidgetInterface") )
    return (KomposeWidgetInterface*)QWidget::parent();
  else
  {
    qDebug("KomposeWidget::getParentWidget() - QWidget::parent() does not inherit (KomposeWidgetInterface)");
    return NULL;
  }
}


// int KomposeWidget::getHeightForWidth( int w ) const
// { 
//   qDebug("KomposeWidget::getHeightForWidth()");
//   return 100;
// }
// 
// int KomposeWidget::getWidthForHeight( int h ) const
// {
//   qDebug("KomposeWidget::getHeightForWidth()");
//   return 100; 
// }
// 
// double KomposeWidget::getAspectRatio()
// { 
//   qDebug("KomposeWidget::getAspectRatio()");
//   return 1.0; 
// }

#include "komposewidget.moc"
