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

#include <qpoint.h>
#include <dcopobject.h>

#include "komposedcopiface.h"
#include "komposefullscreenwidget.h"

class KomposeTask;
class QTimer;

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
  void createVirtualDesktopView() { createView( KOMPOSEDISPLAY_VIRTUALDESKS ); }
  void createCurrentDesktopView() { createView( KOMPOSEDISPLAY_CURRENTDESK ); }
  void createWorldView() { createView( KOMPOSEDISPLAY_WORLD ); }
  void createDefaultView() { createView(); }

  void closeCurrentView();
  bool hasActiveView() { return activeView; }
  bool getBlockScreenshots() { return blockScreenshots; }
  int getDesktopBeforeSnaps() { return deskBeforeSnaps - 1; }  
  
  void setCurrentDesktop( int desknum );
  void activateTask( KomposeTask* task );

protected slots:
  void toggleBlockScreenshots();
  void checkCursorPos();
  
  void uglyQtHackInitFunction();
  void slotStartCursorUpdateTimer();
    
signals:
  void viewClosed();

private:
  KomposeFullscreenWidget *viewWidget;    // the widget where all action takes place
  bool activeView;        // used to check if a view is active
  bool blockScreenshots;   // no screenshots when true
  int deskBeforeSnaps;    // the virtual desk we were on befor screenshots were taken

  QTimer *cursorUpdateTimer;
  // The 4 corners
  QPoint topLeftCorner;
  QPoint topRightCorner;
  QPoint bottomLeftCorner;
  QPoint bottomRightCorner;
};

#endif
