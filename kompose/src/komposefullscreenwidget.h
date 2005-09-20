//
// C++ Interface: komposefullscreenwidget
//
// Description:
//
//
// Author: Hans Oischinger <hans.oischinger@kde-mail.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSEFULLSCREENWIDGET_H
#define KOMPOSEFULLSCREENWIDGET_H


#include "abstractviewwidget.h"
#include "komposesettings.h"


class KRootPixmap;
class KomposeDesktopWidget;
class KPopupMenu;

/**
@author Hans Oischinger
*/
class KomposeFullscreenWidget : public AbstractViewWidget
{
Q_OBJECT
public:
  KomposeFullscreenWidget( int displayType = KomposeSettings::EnumView::VirtualDesktops ,KomposeLayout *l = 0 );
  virtual ~KomposeFullscreenWidget();

  int getHeightForWidth( int w ) const;
  int getWidthForHeight( int h ) const;
  double getAspectRatio();

protected:
  void initView();
  void initMenu();
  void createDesktopWidgets();

  void mouseReleaseEvent (QMouseEvent *);
  void mousePressEvent (QMouseEvent *);
  void keyReleaseEvent ( QKeyEvent * e );

  void destroyChildWidgets();

private:
  KPopupMenu *m_menu;
  KRootPixmap *rootpix;
};

#endif
