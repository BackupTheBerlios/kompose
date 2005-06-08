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
#include "komposewidget.h"
#include "komposelayout.h"

#include <qtimer.h>
#include <kdebug.h>

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
  delete layout;
}


/*
 * Whenever a child is added to the widget it's also added to the layout
 */
void KomposeWidget::childEvent ( QChildEvent * ce)
{
  KomposeWidget* kwChild = 0;
  kwChild = ::qt_cast<KomposeWidget*>(ce->child());
  if ( !kwChild )
    return;

  if ( ce->inserted() )
  {
    kdDebug() << "KomposeWidget::childEvent : Added widget " << ce->child()->className() << " to " << className() << endl;
    layout->add( kwChild );
  }
  else if ( ce->removed() )
  {
    kdDebug() << "KomposeWidget::childEvent : Removed widget " << ce->child()->className() << " from " << className() << endl;
    layout->remove( kwChild );
  }
  layout->arrangeLayout();
}

/*
 * Whenever a resize occurs rearrange the layout
 */
void KomposeWidget::resizeEvent ( QResizeEvent * e )
{
  kdDebug() << "KomposeWidget(" << className() << ")::resizeEvent - Size:" << e->size().width() << "x"<< e->size().height() << endl;
  layout->arrangeLayout();
}


KomposeWidget* KomposeWidget::getParentWidget() const
{
  if ( QWidget::parent()->inherits("KomposeWidget") )
    return (KomposeWidget*)QWidget::parent();
  else
  {
    kdDebug() << "KomposeWidget::getParentWidget() - QWidget::parent() does not inherit (KomposeWidget)" << endl;
    return NULL;
  }
}

int KomposeWidget::getNumofChilds()
{
  return layout->getNumofChilds();
}

#include "komposewidget.moc"
