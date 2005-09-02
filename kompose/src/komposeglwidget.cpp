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
#include <ksharedpixmap.h>

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
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  kdDebug() << "KomposeGLWidget::initializeGL() - creating textures..." << endl;
  TaskList tl = KomposeTaskManager::instance()->getTasks();
  QPtrListIterator<KomposeTask> it( tl );
  KomposeTask *task;
  while ( (task = it.current()) != 0 )
  {
    ++it;
    Q_CHECK_PTR(task);
    QPixmap *pm = task->getVisualizer()->getOrigPixmap();
    bindTexture( pm, task->getVisualizer()->m_glTexID );
    delete pm;
  }
  bindTexture( KomposeGlobal::instance()->getDesktopBgPixmap(), m_BgTexID );
  kdDebug() << "KomposeGLWidget::initializeGL() - all textures created" << endl;
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

  glBindTexture(GL_TEXTURE_2D, m_BgTexID);
  glColor4f(0.6, 0.6, 0.6, 1.0);
  drawTextureRect( geometry(), 0.0 );

  glColor4f(1.0, 1.0, 1.0, 1.0);
  float z = 0.0;
  TaskList tl = KomposeTaskManager::instance()->getTasks();
  QPtrListIterator<KomposeTask> it( tl );
  it.toLast();
  KomposeTask *task;
  while ( (task = it.current()) != 0 )
  {
    --it;
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


    drawTextureRect( currentGeom, z+0.001 );
  }
  glFlush();
}

void KomposeGLWidget::drawTextureRect(QRect pos, float zIndex)
{
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPushAttrib(GL_CURRENT_BIT);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);

  glBegin(GL_QUADS);
  {
    glTexCoord2f(0.0, 0.0);
    glVertex3d(pos.x(), pos.y(), zIndex);

    glTexCoord2f(1.0, 0.0);
    glVertex3d(pos.x() + pos.width(), pos.y(), zIndex);

    glTexCoord2f(1.0, 1.0);
    glVertex3d(pos.x() + pos.width(), pos.y() + pos.height(), zIndex);

    glTexCoord2f(0.0, 1.0);
    glVertex3d(pos.x(), pos.y() + pos.height(), zIndex);
  }
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glPopAttrib();
}

void KomposeGLWidget::bindTexture( const QPixmap* pixmap, uint& texIDStorage )
{
  kdDebug() << k_funcinfo << endl;
  QImage tx;
  QImage image;
  int tx_w = nearest_gl_texture_size(pixmap->width()) /2;
  int tx_h = nearest_gl_texture_size(pixmap->height()) /2;
  unsigned char* screenshot_data = new unsigned char [tx_w * tx_h * 4];

  imlib_context_set_anti_alias(1);
  imlib_context_set_drawable( pixmap->handle() );
  Imlib_Image imgOrig = imlib_create_image_from_drawable((Pixmap)0, 0, 0, pixmap->width(), pixmap->height(), 1);
  imlib_context_set_image( imgOrig );
  if (tx_w != pixmap->width() || tx_h != pixmap->height())
  {
    Imlib_Image img = imlib_create_cropped_scaled_image(0, 0, pixmap->width(), pixmap->height(), tx_w, tx_h);
    imlib_free_image();
    imlib_context_set_image( img );
    QPixmap scaledScreenshot(tx_w, tx_h);
    imlib_context_set_drawable( scaledScreenshot.handle() );
    imlib_render_image_on_drawable_at_size(0, 0, tx_w, tx_h);

    convert_imlib_image_to_opengl_data( tx_w, tx_h, img, screenshot_data );
    imlib_context_set_image( img );
    imlib_free_image();
  }
  else
  {
    convert_imlib_image_to_opengl_data( tx_w, tx_h, imgOrig, screenshot_data );
    imlib_context_set_image( imgOrig );
    imlib_free_image();
  }

  glGenTextures(1, &texIDStorage);
  glBindTexture(GL_TEXTURE_2D, texIDStorage);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


//     gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tx_w, tx_h, GL_RGBA, GL_UNSIGNED_BYTE, screenshot_data);

  glTexImage2D(GL_TEXTURE_2D, 0, 3, tx_w, tx_h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, screenshot_data);

  delete screenshot_data;
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
  }
  else
  {
    m_scale = diff / (float)ANIM_DURATION;
  }
  updateGL();
}

void KomposeGLWidget::showEvent( QShowEvent * )
{
  KomposeTaskManager::instance()->orderListByStacking();

  m_scale = 0.;
  m_animProgress->start();
  m_animTimer->start(0);
}

/**
 * Taken from the desk3d project: http://desk3d.sourceforge.net
 * @param texture_size 
 * @param imlib_img 
 * @param out_buff 
 */
void KomposeGLWidget::convert_imlib_image_to_opengl_data(int texture_width, int texture_height, Imlib_Image imlib_img, unsigned char *out_buff)
{
  imlib_context_set_image(imlib_img);
  Imlib_Image imgfinal = imlib_create_cropped_scaled_image(0, 0,
                         imlib_image_get_width(),
                         imlib_image_get_height(),
                         texture_width,
                         texture_height);

  imlib_context_set_image(imgfinal);
  unsigned int *tmp = imlib_image_get_data();

  int w, h;
  int x, y;
  int offset;
  int img_offset;

  w = texture_width;
  h = texture_height;

  for (y = 0; y < h; y++)
  {
    for (x = 0; x < w; x++)
    {
      offset = (y * w + x) * 4;
      img_offset = (y * w + x);

      out_buff[offset]     = (tmp[img_offset] >> 16) & 0xff;
      out_buff[offset + 1] = (tmp[img_offset] >> 8) & 0xff;
      out_buff[offset + 2] =  tmp[img_offset] & 0xff;
      out_buff[offset + 3] = (tmp[img_offset] >> 24) & 0xff;
    }
  }
  imlib_image_put_back_data(tmp);
  imlib_free_image();
}
