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
#include "komposeglwidget.h"
#include "komposelayout.h"

#include <qtimer.h>

/*
 * The constructor accepts a custom layout, if none is specified a defaultlayout is used
 */
KomposeGLWidget::KomposeGLWidget(QObject *parent, KomposeLayout *l, const char *name)
    : QObject(parent)
{
  geometry = QRect(0,0,1,1);
  if (!l)
    layout = new KomposeLayout( this );
  else
    layout = l;
}


KomposeGLWidget::~KomposeGLWidget()
{}


/*
 * Whenever a child is added to the widget it's also added to the layout
 */
void KomposeGLWidget::childEvent ( QChildEvent * ce )
{
  if ( !ce->child()->inherits("KomposeGLWidget") )
    return;

  if ( ce->inserted() )
  {
    qDebug("KomposeGLWidget::childEvent : Added widget %s to %s", ce->child()->className(), className() );
    list.append( dynamic_cast<KomposeGLWidget*>(ce->child()) );
    layout->add( dynamic_cast<KomposeWidgetInterface*>(ce->child()) );
  }
  else if ( ce->removed() )
  {
    qDebug("KomposeGLWidget::childEvent : Removed widget %s from %s", ce->child()->className(), className() );
    list.remove( dynamic_cast<KomposeGLWidget*>(ce->child()) );
    layout->remove( dynamic_cast<KomposeWidgetInterface*>(ce->child()) );
  }

  // Whenever a child is added/removed: rearrange layout
  // FIXME: sometimes widget's aren't added in time, so we have to add a short delay:
  QTimer::singleShot( 200, layout, SLOT( arrangeLayout() ) );
}

/*
 * Main GL draw routine
 */
void KomposeGLWidget::draw()
{
  qDebug("KomposeGLWidget::draw - Arranging layout");
  layout->arrangeLayout();
}


// Redirect these functions to QWidget

void KomposeGLWidget::setGeom ( const QRect &rect )
{
  geometry = rect;
}

void KomposeGLWidget::setGeom ( const QSize &size )
{
  geometry.setSize( size );
}

QSize KomposeGLWidget::getSize() const
{
  return geometry.size();
}

QRect KomposeGLWidget::getRect() const
{
  return geometry;
}

KomposeWidgetInterface* KomposeGLWidget::getParentWidget() const
{
  if ( QObject::parent()->inherits("KomposeWidgetInterface") )
    return (KomposeWidgetInterface*)QObject::parent();
  else
  {
    qDebug("KomposeGLWidget::getParentWidget() - QWidget::parent() does not inherit (KomposeWidgetInterface)");
    return NULL;
  }
}

void KomposeGLWidget::removeChildWidget( KomposeWidgetInterface* obj )
{
  QObject::removeChild((QObject *) obj);
}

#include "komposeglwidget.moc"
