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
#ifndef KOMPOSEWIDGET_H
#define KOMPOSEWIDGET_H

#include <qwidget.h>

class KomposeLayout;

/**
This base class for all Kompose Widgets glues the Layout and the widget together:
@author Hans Oischinger
*/
class KomposeWidget : public QWidget
{
Q_OBJECT
public:
  KomposeWidget( QWidget *parent = 0, KomposeLayout *l = 0, const char *name = 0);

  virtual ~KomposeWidget();

  // KomposeWidgetInterface:
  virtual KomposeWidget* getParentWidget() const;
  virtual int getHeightForWidth( int w ) const = 0;
  virtual int getWidthForHeight( int h ) const = 0;
  virtual double getAspectRatio() = 0;
  
  KomposeLayout* getLayout() { return layout; }
  
protected:
  void childEvent( QChildEvent * ce);
  void resizeEvent ( QResizeEvent * e );
  
protected:
  KomposeLayout *layout;
};

#endif
