//
// C++ Interface: komposeglwidget
//
// Description:
//
//
// Author: Hans Oischinger <hans.oischinger@kde-mail.net>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSEGLWIDGET_H
#define KOMPOSEGLWIDGET_H

# include <GL/gl.h>
# include <GL/glu.h>
#include <qgl.h>
#include "komposesettings.h"

class KomposeLayout;
class KomposeTask;
class QTimer;
/**
@author Hans Oischinger
*/
class KomposeGLWidget : public QGLWidget
{
  Q_OBJECT
public:
  KomposeGLWidget( QWidget* parent, int displayType = KOMPOSEDISPLAY_VIRTUALDESKS ,KomposeLayout *l = 0 );
  virtual ~KomposeGLWidget();

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);
  void showEvent ( QShowEvent * );

  void bindTexture(KomposeTask* t);
  void drawTextureRect(QRect pos, float scale);

  void rearrangeContents( const QRect& availRect );

protected slots:
  void scaleOneStep();

private:
  QTimer* m_animTimer;
  QTime* m_animProgress;
  double m_scale;
double z;
};

#endif
