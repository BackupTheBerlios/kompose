//
// C++ Interface: komposegldesktopwidget
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSEGLDESKTOPWIDGET_H
#define KOMPOSEGLDESKTOPWIDGET_H

#include <qobject.h>
#include <qrect.h>

#include "komposeglwidget.h"

class KomposeTask;
/**
@author Hans Oischinger
*/
class KomposeGLDesktopWidget : public KomposeGLWidget
{
  Q_OBJECT
public:
  KomposeGLDesktopWidget(int desktop, QObject *parent = 0, const char *name = 0);

  ~KomposeGLDesktopWidget();

  void draw();
    
  int getHeightForWidth( int w ) const;
  int getWidthForHeight( int h ) const;
  double getAspectRatio();
  
protected slots:
  void createTaskWidgets();
  void createTaskWidget( KomposeTask* task );
  
private:
  QRect deskRect;
  int deskNum;
};

#endif
