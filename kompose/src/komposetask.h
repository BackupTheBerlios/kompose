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
#ifndef KOMPOSETASK_H
#define KOMPOSETASK_H

#include <qobject.h>
#include <qimage.h>
#include <qpixmap.h>

#include <kwin.h>

class KWinModule;

/**
@author Hans Oischinger
*/
class KomposeTask : public QObject
{
Q_OBJECT
public:
  KomposeTask(WId win, KWinModule *kwinmod, QObject *parent = 0, const char *name = 0);

  ~KomposeTask();

public:
  // state
  bool isMaximized() const;
  bool isIconified() const;
  bool isShaded() const;
  bool isOnTop() const;
  int onDesktop() const;
  bool isOnCurrentDesktop() const;
  bool isOnAllDesktops() const;
  bool isAlwaysOnTop() const;
  bool isActive() const;
  bool isModified() const;

  WId window() const { return windowID; }
  QString name() const { return windowInfo.name; }
  QString visibleName() const { return windowInfo.visibleName; }
  QString visibleNameWithState() const { return windowInfo.visibleNameWithState(); }

    
  QPixmap& getScreenshotPix() { return pm_screenshot; }
  QImage& getScreenshotImg();
  double getAspectRatio();
  int getHeightForWidth( int w ) const;
  int getWidthForHeight( int h ) const;
  
protected:
  void paintEvent ( QPaintEvent * );
  void mouseReleaseEvent ( QMouseEvent * e );
  
signals:
  void stateChanged();
  void closed();
  
public slots:
  void maximize();
  void restore();
  void iconify();
  void close();
  void raise();
  void lower();
  void activate();
  void activateOrRaise();
  void minimizeOrRestore();
  
  void toDesktop(int desk);
  void toCurrentDesktop();
  void setAlwaysOnTop(bool stay);
  void toggleAlwaysOnTop();
  void setShaded(bool shade);
  void toggleShaded();

  void refresh();
  void updateScreenshot();

private:
  KWinModule* kwinmodule;
  
  WId windowID;
  KWin::Info windowInfo;

  QPixmap pm_screenshot;
  QImage img_screenshot;
  bool imageNeedsUpdate;
  
  bool active;
};

#endif
