//
// C++ Interface: komposetaskcontainerwidget
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSETASKCONTAINERWIDGET_H
#define KOMPOSETASKCONTAINERWIDGET_H

#include "komposewidget.h"
#include "komposetask.h"

class KomposeLayout;

/**
 * This abstract class contains some functions only usable for
 * Widgets that display task widgets
 * (so I won't have to double the code for the createTaskWidgets() funxtion :) )
 *
 * @author Hans Oischinger
 */
class KomposeTaskContainerWidget : public KomposeWidget
{
  Q_OBJECT
public:
  KomposeTaskContainerWidget( int desk = 0, QWidget *parent = 0, KomposeLayout *l = 0, const char *name = 0);

  virtual ~KomposeTaskContainerWidget();

  virtual int getHeightForWidth( int w ) const;
  virtual int getWidthForHeight( int h ) const;
  virtual double getAspectRatio();
  
  void setDesktop( int desk ) { desktop = desk; }
  int getDesktop() { return desktop; }
  
protected slots:
  void createTaskWidgets();
  void createTaskWidget( KomposeTask* task);

protected:
  int desktop; // -1 for all desktops
};

#endif
