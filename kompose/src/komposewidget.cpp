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

#include "komposelayout.h"

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
    layout->add( dynamic_cast<KomposeWidget*>(ce->child()) );
  }
  else if ( ce->removed() )
  {
    qDebug("KomposeWidget::childEvent : Removed widget %s from %s", ce->child()->className(), className() );
    layout->remove( dynamic_cast<KomposeWidget*>(ce->child()) );
  }
//layout->arrangeLayout();
  // Whenever a child is added/removed: rearrange layout
  // FIXME: sometimes widget's aren't added in time, so we have to add a short delay:
  QTimer::singleShot( 200, layout, SLOT( arrangeLayout() ) );
}

/*
 * Whenever a resize occurs rearrange the layout
 */
void KomposeWidget::resizeEvent ( QResizeEvent * e )
{
  qDebug("KomposeWidget(%s)::resizeEvent - Size:%dx%d", className(), e->size().width() , e->size().height());
  layout->arrangeLayout();
}


KomposeWidget* KomposeWidget::getParentWidget() const
{
  if ( QWidget::parent()->inherits("KomposeWidget") )
    return (KomposeWidget*)QWidget::parent();
  else
  {
    qDebug("KomposeWidget::getParentWidget() - QWidget::parent() does not inherit (KomposeWidget)");
    return NULL;
  }
}


#include "komposewidget.moc"
