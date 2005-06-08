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
#ifndef KOMPOSETASKWIDGET_H
#define KOMPOSETASKWIDGET_H

#include "komposewidget.h"
#include "komposetaskprefswidget.h"

#include <qimage.h>
#include <qpixmap.h>


class KomposeTask;
class KomposeLayout;
class KPopupMenu;
class KAction;
class KActionCollection;

/**
@author Hans Oischinger
*/
class KomposeTaskWidget : public KomposeWidget
{
  Q_OBJECT
public:
  KomposeTaskWidget(KomposeTask *t, QWidget *parent = 0, KomposeLayout *l = 0, const char *name = 0);

  ~KomposeTaskWidget();

  int getHeightForWidth( int w ) const;
  int getWidthForHeight( int h ) const;
  double getAspectRatio();

  KomposeTask* getTask() const { return task; };
  KAction* getActMinimizeRestoreTask() { return actMinimizeRestoreTask; }
  KAction* getActCloseTask() { return actCloseTask; }

signals:
  void requestRemoval(KomposeWidget*);

protected:
  void paintEvent ( QPaintEvent * );
  void mouseReleaseEvent ( QMouseEvent * e );
  void mousePressEvent ( QMouseEvent * e );
  void mouseMoveEvent ( QMouseEvent * e );
  void leaveEvent ( QEvent * );
  void enterEvent ( QEvent * );
  void resizeEvent ( QResizeEvent * );
  void focusInEvent ( QFocusEvent * );
  void focusOutEvent ( QFocusEvent * );
  void keyReleaseEvent ( QKeyEvent * e );

  void startDrag();
  void initActions();
  void initMenu();

protected slots:
  void slotTaskDestroyed();
  void drawWidget();
  void drawWidgetAndRepaint();
  void initFonts();
  void setGeometry( const QRect &rect );
  void reInitMenu();

  void slotMinimizeRestoreToggled();
  void setActionIcons( bool inverse=false );

private:
  QPixmap pm_dbBackground;

  bool highlight; // Highlight widget?
  QFont titleFont;

  KomposeTask* task;
  KomposeTaskPrefsWidget *prefWidget;
  KPopupMenu *menu;

  KActionCollection *taskActionCollection;
  KAction *actMinimizeRestoreTask;
  KAction *actCloseTask;
};

#endif
