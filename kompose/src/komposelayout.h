/***************************************************************************
 *   Copyright (C) 2004 by Hans Oischinger                                 *
 *   oisch@users.berlios.de                                                 *
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

#include "komposewidget.h"

class QRect;

enum LayoutType { TLAYOUT_GENERIC,  // layouts all KomposeWidget classes
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
  KomposeLayout( KomposeWidget *parent, int type = TLAYOUT_GENERIC, int dist = 10, const char *name = 0 );

  ~KomposeLayout();

  void add( KomposeWidget *w );
  void remove( KomposeWidget *w );
  void setType( int t );
int getType() { return layoutType; }

  const QPtrList<KomposeWidget> *getManagedWidgets() { return &list; }
  int getNumofChilds() { return list.count(); }
  KomposeWidget* getNeighbour( const KomposeWidget* widget,
                               int direction = DLAYOUT_RIGHT,
                               int wrap = WLAYOUT_NONE );

public slots:
  void arrangeLayout();

protected:
  KomposeWidget* getNeighbour( QPtrList<KomposeWidget> listToSearch,
                               const KomposeWidget* widget,
                               int direction = DLAYOUT_RIGHT,
                               int wrap = WLAYOUT_NONE );
  void rearrangeContents();
  void rearrangeContents( const QRect& availRect,
                          const QPtrList<KomposeWidget> widgets,
                          int rows = -1,
                          int columns = -1,
                          bool setMemberRowsCols = true );

private:
  // List of all managed childs
  QPtrList<KomposeWidget> list;
  // List of container childs seperated by empty/full
  QPtrList<KomposeWidget> filledContainers;
  QPtrList<KomposeWidget> emptyContainers;

  QSize currentSize;
  int layoutType;
  int spacing;
  bool widgetsChanged;
  int currentRows;
  int currentColumns;

  KomposeWidget* parentWidget;
};

#endif
