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
#ifndef KOMPOSEDESKTOPWIDGET_H
#define KOMPOSEDESKTOPWIDGET_H

#include "komposetaskcontainerwidget.h"

class KRootPixmap;
class KomposeLayout;
class KomposeTask;

/**
@author Hans Oischinger
*/
class KomposeDesktopWidget : public KomposeTaskContainerWidget
{
  Q_OBJECT
public:
  KomposeDesktopWidget(int desktop, QWidget *parent = 0, KomposeLayout *l = 0, const char *name = 0);

  ~KomposeDesktopWidget();

//   int getHeightForWidth( int w ) const;
//   int getWidthForHeight( int h ) const;
//   double getAspectRatio();

signals:
  void contentsChanged();
  
protected:
  void paintEvent ( QPaintEvent * );
  void dragEnterEvent ( QDragEnterEvent * );
  void dropEvent ( QDropEvent * );
  void leaveEvent ( QEvent * );
  void enterEvent ( QEvent * );
  void mouseReleaseEvent ( QMouseEvent * e );
  void mouseDoubleClickEvent ( QMouseEvent * e );
  void childEvent( QChildEvent * ce);
  void focusInEvent ( QFocusEvent * );
  void focusOutEvent ( QFocusEvent * );
  void keyReleaseEvent ( QKeyEvent * e );

protected slots:
  void initFonts();
  
private:
  QRect deskRect;
  KRootPixmap *rootpix;
  QString deskName;
  bool highlight; // Highlight widget?
  QFont titleFont;
};

#endif
