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

enum LayoutDirections { DLAYOUT_LEFT, DLAYOUT_RIGHT, DLAYOUT_TOP, DLAYOUT_BOTTOM };

enum WrapDirections { WLAYOUT_NONE, WLAYOUT_HORIZONTAL, WLAYOUT_VERTICAL, WLAYOUT_BOTH };

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
  void setType( int t );
  int getType() { return layoutType; }

  const QPtrList<KomposeWidgetInterface> *getManagedWidgets() { return &list; }
  int getNumofChilds() { return list.count(); }
  KomposeWidgetInterface* getNeighbour( const KomposeWidgetInterface* widget,
                                        int direction = DLAYOUT_RIGHT,
                                        int wrap = WLAYOUT_NONE );

public slots:
  void arrangeLayout();

protected:
  KomposeWidgetInterface* getNeighbour( QPtrList<KomposeWidgetInterface> listToSearch,
                                        const KomposeWidgetInterface* widget,
                                        int direction = DLAYOUT_RIGHT,
                                        int wrap = WLAYOUT_NONE );
  void rearrangeContents();
  void rearrangeContents( const QRect& availRect,
                          const QPtrList<KomposeWidgetInterface> widgets,
                          int rows = -1,
                          int columns = -1,
                          bool setMemberRowsCols = true );

private:
  // List of all managed childs
  QPtrList<KomposeWidgetInterface> list;
  // List of container childs seperated by empty/full
  QPtrList<KomposeWidgetInterface> filledContainers;
  QPtrList<KomposeWidgetInterface> emptyContainers;

  QSize currentSize;
  int layoutType;
  int spacing;
  bool widgetsChanged;
  int currentRows;
  int currentColumns;

  KomposeWidgetInterface* parentWidget;
};

#endif
