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
#include <qrect.h>
#include <kwin.h>
//typedef unsigned long Drawable; //FIXME!!!

class KWinModule;
class KomposeTaskVisualizer;

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
  bool isValid() const { return windowInfo.valid(); }

  WId window() const { return windowID; }
  WId wmFrame() const { return wmframeID; }
  QString name() const { return windowInfo.name(); }
  QString visibleName() const { return windowInfo.visibleName(); }
  QString visibleNameWithState() const { return windowInfo.visibleNameWithState(); }
  QRect getGeometry() const { return windowInfo.geometry(); }
  
  QPixmap getIcon( int size );
  double getAspectRatio();
  int getHeightForWidth( int w ) const;
  int getWidthForHeight( int h ) const;
  
  KomposeTaskVisualizer* getVisualizer() { return visualizer; }
  
protected:
  void paintEvent ( QPaintEvent * );
  void mouseReleaseEvent ( QMouseEvent * e );
  void findWmFrame();
  
signals:
  void stateChanged();
  void x11ConfigureNotify();
  void x11DamageNotify();
  void closed();
  
public slots:
  void maximize();
  void restore();
  void iconify();
  void close();
  void raise();
  void lower();
  void activate();
//   void activateOrRaise();
  void minimizeOrRestore();
  
  void toDesktop(int desk);
  void toCurrentDesktop();
  void setAlwaysOnTop(bool stay);
  void toggleAlwaysOnTop();
  void setShaded(bool shade);
  void toggleShaded();

  void refresh();
  void slotX11ConfigureNotify();
  void slotX11DamageNotify();
  void slotActivated();
  void slotUpdateScreenshot();

protected slots:
  void unBlockDamageEvents() { blockDamageEvents = false; }
  
private:
  KWinModule* kwinmodule;
  WId windowID;
  WId wmframeID;
  KWin::WindowInfo windowInfo;
  
  bool blockDamageEvents;
  KomposeTaskVisualizer* visualizer;
};

#endif
