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


#include "komposetaskcontainerwidget.h"
#include "komposesettings.h"


class KRootPixmap;
class KomposeDesktopWidget;
class KPopupMenu;

/**
@author Hans Oischinger
*/
class KomposeFullscreenWidget : public KomposeTaskContainerWidget
{
Q_OBJECT
public:
  KomposeFullscreenWidget( int displayType = KOMPOSEDISPLAY_VIRTUALDESKS ,KomposeLayout *l = 0 );

  virtual ~KomposeFullscreenWidget();

  int getHeightForWidth( int w ) const;
  int getWidthForHeight( int h ) const;
  double getAspectRatio();
 
  void setType( int t ) { type = t; initView(); }
  int getType() { return type; }
  
protected:
  void initView();
  void initMenu();
  void createDesktopWidgets();

  void mouseReleaseEvent (QMouseEvent *);
  void mousePressEvent (QMouseEvent *);
  void keyReleaseEvent ( QKeyEvent * e );
//   void showEvent (QShowEvent * e);
  
  void destroyChildWidgets();
//   virtual void closeEvent ( QCloseEvent * e );
 
private:
  KRootPixmap *rootpix;
  int type;
  KPopupMenu *menu;
};

#endif
