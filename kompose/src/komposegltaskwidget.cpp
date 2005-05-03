//
// C++ Implementation: komposegltaskwidget
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposegltaskwidget.h"

#include "komposetaskmanager.h"
#include "komposetask.h"
#include "komposelayout.h"
#include <kdebug.h>

#include <qgl.h>
#include <qimage.h>



KomposeGLTaskWidget::KomposeGLTaskWidget(KomposeTask *t, QObject *parent, KomposeLayout *l, const char *name)
    : KomposeGLWidget(parent, l),
    task(t)
{
  QImage img = task->getScreenshot().convertToImage();
  img = img.smoothScale( 256, 256, QImage::ScaleMin );
  tex = QGLWidget::convertToGLFormat( img );
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  /*  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // cheap scaling when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // cheap scaling when image smalled than texture*/
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

  glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

  glTexImage2D( GL_TEXTURE_2D, 0, 3, 256, 256, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );

                

  
  connect( t, SIGNAL( closed() ), this, SLOT( slotTaskDestroyed() ) );
  connect( t, SIGNAL( stateChanged() ), this, SLOT( draw() ) );
}


KomposeGLTaskWidget::~KomposeGLTaskWidget()
{}

void KomposeGLTaskWidget::slotTaskDestroyed()
{
  disconnect( task, SIGNAL( closed() ), this, SLOT( slotTaskDestroyed() ) );
  disconnect( task, SIGNAL( stateChanged() ), this, SLOT( draw() ) );
  if (KomposeTaskManager::instance()->hasActiveView())
  {
    this->parent()->removeChild( this );
  }
}

int KomposeGLTaskWidget::getHeightForWidth ( int w ) const
{
  qDebug("KomposeGLTaskWidget::getHeightForWidth()");
  return task->getHeightForWidth(w);
}

int KomposeGLTaskWidget::getWidthForHeight ( int h ) const
{
  qDebug("KomposeGLTaskWidget::getWidthForHeight()");
  return task->getWidthForHeight(h);
}

double KomposeGLTaskWidget::getAspectRatio()
{
  qDebug("KomposeGLTaskWidget::getAspectRatio()");
  return task->getAspectRatio();
}

void KomposeGLTaskWidget::draw()
{
  KomposeGLWidget::draw();
  qDebug("KomposeGLTaskWidget::draw() - %d,%d %dx%d", getRect().x(), getRect().y(), getRect().width(), getRect().height());
  glColor3f(1.0f, 0.0f, 0.0f);

  glBindTexture(GL_TEXTURE_2D, texture);

  /*  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );*/
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glEnable( GL_TEXTURE_2D );

  //   glTexImage2D( GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
  //                 GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );
  //  qDebug("KomposeGLTaskWidget::draw() - tex: %dx%d", tex.width(), tex.height() );

  glBegin( GL_QUADS );
  glTexCoord2f(0.0, 0.0);
  glVertex2i( getRect().x(), getRect().y() );
  glTexCoord2f(0.0, 1.0);
  glVertex2i( getRect().x(), getRect().y() + getRect().height() );
  glTexCoord2f(1.0, 1.0);
  glVertex2i( getRect().x() + getRect().width(), getRect().y() + getRect().height() );
  glTexCoord2f(1.0, 0.0);
  glVertex2i( getRect().x() + getRect().width(), getRect().y() );
  glEnd();

  glDisable( GL_TEXTURE_2D );
/*
glRasterPos2i (getRect().x(), getRect().y());
  glPixelZoom( (GLfloat)(getSize().width())/(GLfloat)(tex.width()), (GLfloat)(getSize().height())/(GLfloat)(tex.height()) );
  glDrawPixels(tex.width(), tex.height(), GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() ); */
}

#include "komposegltaskwidget.moc"
