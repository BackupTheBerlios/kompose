//
// C++ Implementation: komposeglwidget
//
// Description:
//
//
// Author: Hans Oischinger <hans.oischinger@kde-mail.net>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposeglwidget.h"

#include "komposeviewmanager.h"
#include "komposetaskmanager.h"
#include "komposetaskvisualizer.h"
#include "komposeglobal.h"
#include "komposelayout.h"

#include <qpixmap.h>
#include <qimage.h>
#include <qtimer.h>
#include <qdatetime.h>

#include <kwin.h>
#include <kglobalsettings.h>
#include <kdebug.h>

#define ANIM_DURATION 500

// returns the highest number closest to v, which is a power of 2
// NB! assumes 32 bit ints
static int nearest_gl_texture_size(int v)
{
  int n = 0, last = 0;
  for (int s = 0; s < 32; ++s)
  {
    if (((v>>s) & 1) == 1)
    {
      ++n;
      last = s;
    }
  }
  if (n > 1)
    return 1 << (last+1);
  return 1 << last;
}


KomposeGLWidget::KomposeGLWidget( QWidget* parent, int displayType, KomposeLayout *l)
    : QGLWidget(parent), m_scale(0.)
{
  kdDebug() << k_funcinfo << endl;
  m_animTimer = new QTimer(this);
  m_animProgress = new QTime();
  connect(m_animTimer, SIGNAL(timeout()), this, SLOT(scaleOneStep()));
}


KomposeGLWidget::~KomposeGLWidget()
{
  delete m_animProgress;

  makeCurrent();
  TaskList tl = KomposeTaskManager::instance()->getTasks();
  QPtrListIterator<KomposeTask> it( tl );
  KomposeTask *task;
  while ( (task = it.current()) != 0 )
  {
    ++it;
    Q_CHECK_PTR(task);
    if (task->getVisualizer()->m_glTexID != 0)
    {
      glDeleteTextures(1, &(task->getVisualizer()->m_glTexID));
      task->getVisualizer()->m_glTexID = 0;
    }
  }
}

void KomposeGLWidget::initializeGL()
{
  kdDebug() << k_funcinfo << endl;
  // Set up the rendering context, define display lists etc.:
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  //   glMatrixMode(GL_PROJECTION);
  //   glLoadIdentity();
  //   glOrtho(0.0,0.0,1280,800, -1.0, 1.0);

  TaskList tl = KomposeTaskManager::instance()->getTasks();
  QPtrListIterator<KomposeTask> it( tl );
  KomposeTask *task;
  while ( (task = it.current()) != 0 )
  {
    ++it;
    Q_CHECK_PTR(task);
    bindTexture( task );
  }
}

void KomposeGLWidget::resizeGL( int w, int h )
{
  kdDebug() << k_funcinfo << endl;
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, h, 0, -1.0l, 1.0l);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  rearrangeContents( rect() );
}

void KomposeGLWidget::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  TaskList tl = KomposeTaskManager::instance()->getTasks();
  QPtrListIterator<KomposeTask> it( tl );
  KomposeTask *task;
  while ( (task = it.current()) != 0 )
  {
    ++it;
    Q_CHECK_PTR(task);
    if (task->getVisualizer()->m_glTexID != 0)
      glBindTexture(GL_TEXTURE_2D, task->getVisualizer()->m_glTexID );

    QRect currentGeom;
    currentGeom.setX( task->getFrameGeometry().x() +
                      m_scale*(double)( task->getKomposeGeom().x() -
                                        task->getFrameGeometry().x() ) );
    currentGeom.setY( task->getFrameGeometry().y() +
                      m_scale*(double)( task->getKomposeGeom().y() -
                                        task->getFrameGeometry().y() ) );

    currentGeom.setWidth( task->getFrameGeometry().width() +
                          m_scale*(double)( task->getKomposeGeom().width() -
                                            task->getFrameGeometry().width() ) );

    currentGeom.setHeight( task->getFrameGeometry().height() +
                           m_scale*(double)( task->getKomposeGeom().height() -
                                             task->getFrameGeometry().height() ) );


    drawTextureRect( currentGeom, 1. );
  }
  glFlush();
}

void KomposeGLWidget::drawTextureRect(QRect pos, float scale)
{
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPushAttrib(GL_CURRENT_BIT);
  glColor4f(1.0, 1.0, 1.0, 1.0);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glScalef(scale, scale, 0.);
  glBegin(GL_QUADS);
  {
    glTexCoord2f(0.0, 1.0);
    glVertex2d(pos.x(), pos.y());

    glTexCoord2f(1.0, 1.0);
    glVertex2d(pos.x() + pos.width(), pos.y());

    glTexCoord2f(1.0, 0.0);
    glVertex2d(pos.x() + pos.width(), pos.y() + pos.height());

    glTexCoord2f(0.0, 0.0);
    glVertex2d(pos.x(), pos.y() + pos.height());
  }
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glPopAttrib();
}

void KomposeGLWidget::bindTexture(KomposeTask* t)
{
  QPixmap *pixmap = t->getVisualizer()->getOrigPixmap();
  QImage image = pixmap->convertToImage();
  delete pixmap;

  // Scale the pixmap if needed. GL textures needs to have the
  // dimensions 2^n+2(border) x 2^m+2(border).
  QImage tx;
  int tx_w = nearest_gl_texture_size(image.width()) /2;
  int tx_h = nearest_gl_texture_size(image.height()) /2;
  if (tx_w != image.width() || tx_h != image.height())
    tx = QGLWidget::convertToGLFormat(image.smoothScale(tx_w, tx_h));
  else
    tx = QGLWidget::convertToGLFormat(image);

  glGenTextures(1, &(t->getVisualizer()->m_glTexID));
  glBindTexture(GL_TEXTURE_2D, t->getVisualizer()->m_glTexID);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  /*  if (QGLExtensions::glExtensions & QGLExtensions::GenerateMipmap
        && GL_TEXTURE_2D == GL_TEXTURE_2D)
    {
      glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
      glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    /*}
    else
    {*/
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // }

  glTexImage2D(GL_TEXTURE_2D, 0, 3, tx.width(), tx.height(), 0, GL_RGBA,
               GL_UNSIGNED_BYTE, tx.bits());
  // this assumes the size of a texture is always smaller than the max cache size
  // int cost = tx.width()*tx.height()*4/1024;
}

#include <math.h>
/**
 * availRect specifies the size&pos of the contents
 * Specify either rows or cols to set a fixed number of those (setting both won't work correctly)
 */
void KomposeGLWidget::rearrangeContents( const QRect& availRect )
{
  kdDebug() << k_funcinfo << endl;
  TaskList tasks = KomposeTaskManager::instance()->getTasks();
  // Check or empty list
  if (tasks.count() == 0)
  {
    kdDebug() << "rearrangeContents() - empty list... skipping!" << endl;
    return;
  }

  int rows = 1;
  int columns = 1;
  int spacing = 10;

  double parentRatio = (double)availRect.width() / (double)availRect.height();
  // Use more columns than rows when parent's width > parent's height
  if ( parentRatio > 1 )
  {
    columns = (int)ceil( sqrt(tasks.count()) );
    rows = (int)ceil( (double)tasks.count() / (double)columns );
  }
  else
  {
    rows = (int)ceil( sqrt(tasks.count()) );
    columns = (int)ceil( (double)tasks.count() / (double)rows );
  }

  kdDebug() << "rearrangeContents() - Relayouting " << tasks.count() << " windows with " << rows << " rows & " << columns << " columns" << endl;

  // Calculate width & height
  int w = (availRect.width() - (columns+1) * spacing ) / columns;
  int h = (availRect.height() - (rows+1) * spacing ) / rows;

  QPtrListIterator<KomposeTask> it( tasks );
  KomposeTask *task = 0;
  int y = spacing;
  for ( int i=0; i<rows; ++i )
  {
    int x = spacing;
    for ( int j=0; j<columns; ++j )
    {
      if ( (task = it.current()) == 0 )
        break;
      ++it;

      QRect geom = task->getFrameGeometry();
      double ratio = (double)geom.width() / (double)geom.height();

      int widgetw = w;
      int widgeth = h;
      int hforw = (int)((double)w * (double)geom.height() / (double)geom.width());
      int wforh = (int)((double)geom.width() * (double)h / (double)geom.height());
      if ( (ratio >= 1.0 && hforw <= h) ||
           (ratio < 1.0 && wforh > w)   )
      {
        widgetw = w;
        widgeth = hforw;
      }
      else if ( (ratio < 1.0 && wforh <= w) ||
                (ratio >= 1.0 && hforw > h)   )
      {
        widgeth = h;
        widgetw = wforh;
      }

      geom.setX(x);
      geom.setY(y);
      geom.setWidth(widgetw);
      geom.setHeight(widgeth);
      task->setKomposeGeom(geom);
      x+=w+spacing;
    }
    y+=h+spacing;
  }
}

void KomposeGLWidget::scaleOneStep()
{
  float diff = m_animProgress->elapsed();
  if (diff > ANIM_DURATION)
  {
    m_scale = 1.;
    m_animTimer->stop();
  } else {
    m_scale = diff / (float)ANIM_DURATION;
  }
  updateGL();
}

void KomposeGLWidget::showEvent( QShowEvent * )
{
  kdDebug() << k_funcinfo << endl;
  m_scale = 0.;
  m_animProgress->start();
  m_animTimer->start(0);
}
