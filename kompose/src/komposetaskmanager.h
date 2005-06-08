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
#ifndef KOMPOSETASKMANAGER_H
#define KOMPOSETASKMANAGER_H

#include <qobject.h>
#include <qptrlist.h>


class KWinModule;
class KomposeTaskManager;
class KomposeTask;

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
  static KomposeTaskManager *instance();

  bool isOnTop(const KomposeTask* task );

  int getNumDesktops() const { return numDesks; }
  QString getDesktopName(int desk) const;
  int getCurrentDesktopNum();

  TaskList getTasks() const { return tasklist; }

public slots:
  bool processX11Event( XEvent *event );
  void slotUpdateScreenshots( bool switchDesktops=true );
  void simulatePasvScreenshotEvent();

  void slotStartWindowListeners();

protected slots:
  void slotTaskActivated(WId);
  void slotWindowAdded( WId w );
  void slotWindowRemoved( WId w );
  void slotWindowChanged( WId, unsigned int );
  void slotDesktopCountChanged(int);
  void slotCurrentDesktopChanged(int);
  void callCompositeRedirect();

signals:
  void newTask( KomposeTask* task );
  void taskDesktopChanged( KomposeTask* task, int fromDesktop, int toDesktop );

protected:
  KomposeTask* findTask(WId w, bool wmFrameIds = false);

private:
  KWinModule* kwin_module;
  TaskList tasklist;      // list of tasks handled by the WM

  int numDesks;           // total num of desks
};

#endif
