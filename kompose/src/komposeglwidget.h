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

#include <GL/gl.h>
#include <GL/glu.h>

#include <qgl.h>

#include "komposesettings.h"
#include "komposetaskmanager.h"

typedef void *Imlib_Image;

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

  void bindTexture( const QPixmap* pixmap, uint& texIDStorage );
  void drawTextureRect(QRect pos, float zIndex);

  void rearrangeContents( const QRect& availRect );
  static void convert_imlib_image_to_opengl_data(int texture_width, int texture_height, Imlib_Image imlib_img, unsigned char *out_buff);

  void createOrderedTaskList( TaskList& inList, const QRect& availRect, uint rows, uint columns );

protected slots:
  void scaleOneStep();

private:
  QTimer* m_animTimer;
  QTime* m_animProgress;
  double m_scale;
  uint m_BgTexID;
  TaskList m_orderedTasks;
};

#endif
