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
#include <qvaluelist.h>

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
  KomposeGLWidget( QWidget* parent, int displayType = KomposeSettings::EnumView::VirtualDesktops ,KomposeLayout *l = 0 );
  virtual ~KomposeGLWidget();

  void setType( int t ) { m_type = t; }

protected:
  enum AnimMode { ANIM_IN, ANIM_OUT };
  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);
  void showEvent ( QShowEvent * );
  void mouseReleaseEvent (QMouseEvent *);

  void paintDesktop(QRect pos);
  void paintTask(KomposeTask* task);
  void bindTexture( const QPixmap* pixmap, uint& texIDStorage );
  void drawTextureRect(QRect pos, QSize texSize );

  void rearrangeContents( const QRect& availRect, int desktop=-1 );
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
  GLenum m_target;
  AnimMode m_animMode;
  KomposeTask* m_activateLaterTask;
  int m_type;
  QValueList<QRect> m_desktopRects;

};

#endif
