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
#ifndef KOMPOSETASKMANAGER_H
#define KOMPOSETASKMANAGER_H

#include <qobject.h>
#include <qptrlist.h>

#include "komposetask.h"

class KWinModule;
class KomposeLayout;
class QWidget;
class KomposeDesktopWidget;

typedef QPtrList<KomposeTask> TaskList;


/**
@author Hans Oischinger
*/
class KomposeTaskManager : public QObject
{
  Q_OBJECT
protected:
  KomposeTaskManager();

  ~KomposeTaskManager();
public:

  bool isOnTop(const KomposeTask* task );

  static KomposeTaskManager *instance();
  QWidget* getViewWidget() { return viewWidget; }
  int getNumDesktops() { return numDesks; }
  TaskList getTasks() const { return tasklist; }
  
public slots:
  void createVirtualDesktopView();
  void closeVirtualDesktopView();
  bool hasActiveView() { return activeView; }
  
  void setCurrentDesktop( int desknum );
  void activateTask( KomposeTask* task );

protected slots:
  void slotStartWindowListeners();
  
  void slotUpdateScreenshots();
  void slotUpdateScreenshot(WId);
  
  void slotWindowAdded( WId w );
  void slotWindowRemoved( WId w );
  void slotWindowChanged( WId w, unsigned int properties);
  void slotDesktopCountChanged(int d);
  
signals:
  void newTask( KomposeTask* task );

protected:
  KomposeTask* findTask(WId w);
//   void createGLVirtualDestopView();
  void createQtVirtualDestopView();
  
private:
  KWinModule* kwinmodule;
  QWidget *viewWidget;    // the widget where all action takes place
  bool activeView;        // used to check if a view is active
  TaskList tasklist;      // list of tasks handled by the WM
  
  int numDesks;          // total num of desks
};

#endif
