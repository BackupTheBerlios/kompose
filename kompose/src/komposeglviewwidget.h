//
// C++ Interface: komposeglviewwidget
//
// Description: 
//
//
// Author: Hans Oischinger <hans.oischinger@kde-mail.net>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSEGLVIEWWIDGET_H
#define KOMPOSEGLVIEWWIDGET_H

#include "abstractviewwidget.h"

class QGLWidget;
class KPopupMenu;
class KomposeGLWidget;
/**
@author Hans Oischinger
*/
class KomposeGLViewWidget : public AbstractViewWidget
{
public:
  KomposeGLViewWidget( int displayType = KOMPOSEDISPLAY_VIRTUALDESKS ,KomposeLayout *l = 0 );

  ~KomposeGLViewWidget();

  int getHeightForWidth( int w ) const;
  int getWidthForHeight( int h ) const;
  double getAspectRatio();

protected:
  void resizeEvent ( QResizeEvent * );
  void mousePressEvent (QMouseEvent *);
  void keyReleaseEvent ( QKeyEvent * e );

private:
  KomposeGLWidget* m_glWidget;
  KPopupMenu* m_menu;
};

#endif
