//
// C++ Interface: komposetaskprefswidget
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSETASKPREFSWIDGET_H
#define KOMPOSETASKPREFSWIDGET_H

#include <qdockarea.h> 

class KomposeTaskWidget;
class KAction;

/**
@author Hans Oischinger
*/
class KomposeTaskPrefsWidget : public QDockArea
{
  Q_OBJECT
public:
  KomposeTaskPrefsWidget(KomposeTaskWidget *parent = 0, const char *name = 0);

  ~KomposeTaskPrefsWidget();

protected slots:
  void slotMinimizeRestoreToggled();
  void setActionIcons( bool inverse=false );
  
private:  
  KAction *actMinimizeRestoreTask;
  KAction *actCloseTask;
};

#endif
