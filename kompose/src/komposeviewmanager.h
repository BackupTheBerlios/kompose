//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSEVIEWMANAGER_H
#define KOMPOSEVIEWMANAGER_H

#include <dcopobject.h>

#include "komposedcopiface.h"
#include "komposefullscreenwidget.h"

class KomposeTask;

/**
@author Hans Oischinger
*/
class KomposeViewManager : public QObject, virtual public KomposeDcopIface
{
  Q_OBJECT
protected:
  KomposeViewManager();
  ~KomposeViewManager();

public:
  static KomposeViewManager *instance();

  KomposeFullscreenWidget* getViewWidget() { return viewWidget; }

public slots:
  void createView( int type = -1 ); // -1 means the user's default
  void createVirtualDesktopView();
  void createWorldView();
  void createDefaultView();

  void closeCurrentView();
  bool hasActiveView() { return activeView; }
  
  void setCurrentDesktop( int desknum );
  void activateTask( KomposeTask* task );

signals:
  void viewClosed();

private:
  KomposeFullscreenWidget *viewWidget;    // the widget where all action takes place
  bool activeView;        // used to check if a view is active
  int deskBeforeSnaps;    // the virtual desk we were on befor screenshots were taken

};

#endif
