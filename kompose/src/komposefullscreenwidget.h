//
// C++ Interface: komposefullscreenwidget
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSEFULLSCREENWIDGET_H
#define KOMPOSEFULLSCREENWIDGET_H


#include "komposewidget.h"

#define MAX_DESKTOPS 16

class KRootPixmap;
class KomposeDesktopWidget;

/**
@author Hans Oischinger
*/
class KomposeFullscreenWidget : public KomposeWidget
{
Q_OBJECT
public:
  KomposeFullscreenWidget( KomposeLayout *l = 0);

  ~KomposeFullscreenWidget();

  int getHeightForWidth( int w ) const;
  int getWidthForHeight( int h ) const;
  double getAspectRatio();
  
protected:
  void createDesktopWidgets();

private:
  KRootPixmap *rootpix;
  
  KomposeDesktopWidget *desktop[MAX_DESKTOPS];  // Virtual Desktops representation

};

#endif
