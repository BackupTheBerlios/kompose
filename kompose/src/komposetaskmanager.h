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

#include <dcopobject.h> 

#include "komposetask.h"
#include "komposefullscreenwidget.h"
#include "komposetaskmgrdcopiface.h"

class KWinModule;
class KomposeLayout;
class QWidget;
class KomposeDesktopWidget;
class KomposeTaskManager;

typedef QPtrList<KomposeTask> TaskList;


/**
@author Hans Oischinger
*/
class KomposeTaskManager : public QObject, virtual public KomposeTaskMgrDcopIface
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
  
  // Dcop functions
  void createDefaultView();
  
public slots:
  void createView( int type = -1 ); // -1 means the user's default
  void createVirtualDesktopView();
  void createWorldView();
  void closeCurrentView();
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
  void viewClosed();

protected:
  KomposeTask* findTask(WId w);
//   bool process(const QCString &fun, const QByteArray &data, QCString &replyType, QByteArray &replyData);
  
private:
  KWinModule* kwinmodule;
  KomposeFullscreenWidget *viewWidget;    // the widget where all action takes place
  bool activeView;        // used to check if a view is active
  TaskList tasklist;      // list of tasks handled by the WM

  int deskBeforeSnaps;    // the virtual desk we were on befor screenshots were taken
  int numDesks;           // total num of desks
};

#endif
