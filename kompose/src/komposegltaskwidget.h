//
// C++ Interface: komposegltaskwidget
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSEGLTASKWIDGET_H
#define KOMPOSEGLTASKWIDGET_H

#include "komposeglwidget.h"

#include <qobject.h>
#include <qimage.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

class KomposeTask;
class KomposeLayout;


/**
@author Hans Oischinger
*/
class KomposeGLTaskWidget : public KomposeGLWidget
{
  Q_OBJECT
public:
  KomposeGLTaskWidget(KomposeTask *t, QObject *parent, KomposeLayout *l, const char *name);

  ~KomposeGLTaskWidget();

  void draw();
  
  int getHeightForWidth( int w ) const;
  int getWidthForHeight( int h ) const;
  double getAspectRatio();
  
  KomposeTask* getTask() const { return task; };

protected slots:
//   void scaleScreenshot();
  void slotTaskDestroyed();
  
private:
  KomposeTask* task;
  GLuint texture;
  QImage tex;
};

#endif
