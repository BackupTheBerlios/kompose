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
#ifndef KOMPOSELAYOUT_H
#define KOMPOSELAYOUT_H

#include <qobject.h>
#include <qptrlist.h>

#include "komposewidgetinterface.h"

class QRect;

enum LayoutType { TLAYOUT_GENERIC,  // layouts all KomposeWidgetInterface classes
                  TLAYOUT_TASKCONTAINERS  // layouts containers only
                };

/**
The main layout class.
QLayout just didn't fit :(
 
@author Hans Oischinger
*/
class KomposeLayout : public QObject
{
  Q_OBJECT
public:
  KomposeLayout( KomposeWidgetInterface *parent, int type = TLAYOUT_GENERIC, int dist = 10, const char *name = 0 );

  ~KomposeLayout();

  void add( KomposeWidgetInterface *w );
  void remove( KomposeWidgetInterface *w );
  void setType( int t) { layoutType = t; }
  int getType() { return layoutType; }
  
  const QPtrList<KomposeWidgetInterface> *getManagedWidgets() { return &list; }
  int getNumofChilds() { return list.count(); }

public slots:
  void arrangeLayout();

protected:
  void rearrangeContents();
  void rearrangeContents( const QRect& availRect, const QPtrList<KomposeWidgetInterface> widgets, int rows = -1, int columns = -1 );

private:
  QPtrList<KomposeWidgetInterface> list;
  QSize currentSize;
  int layoutType;
  int spacing;
  bool widgetsChanged;

  KomposeWidgetInterface* parentWidget;
};

#endif
