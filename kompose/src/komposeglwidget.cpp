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

#include <math.h>
#define ANIM_DURATION 300

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
    : QGLWidget(parent),
    m_scale(0.),
    m_activateLaterTask(0)
{
  m_animTimer = new QTimer(this);
  m_animProgress = new QTime();
  connect(m_animTimer, SIGNAL(timeout()), this, SLOT(scaleOneStep()));
  m_target = GL_TEXTURE_RECTANGLE_NV;
  // GL_TEXTURE_2D
}


KomposeGLWidget::~KomposeGLWidget()
{
  delete m_animProgress;

  makeCurrent();
  QPtrListIterator<KomposeTask> it( m_orderedTasks );
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
  //   glEnable(GL_DEPTH_TEST);
  //   glShadeModel(GL_FLAT);
  //   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  //   glMatrixMode(GL_PROJECTION);
  //   glLoadIdentity();
  //   glOrtho(0.0,0.0,1280,800, -1.0, 1.0);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  kdDebug() << "KomposeGLWidget::initializeGL() - creating textures..." << endl;
  TaskList tl = KomposeTaskManager::self()->getTasks();
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
  bindTexture( KomposeGlobal::self()->getDesktopBgPixmap(), m_BgTexID );
  kdDebug() << "KomposeGLWidget::initializeGL() - all textures created" << endl;
}

void KomposeGLWidget::resizeGL( int w, int h )
{
  glShadeModel(GL_FLAT);
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, h, 0, -999999, 999999);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (  KomposeSettings::self()->view( KomposeSettings::EnumViewMode::Default ) ==
        KomposeSettings::EnumView::VirtualDesktops )
  {
    int numDesks = KomposeTaskManager::self()->getNumDesktops();
    int spacing=10; // FIXME: MoveME
    int columns=0;
    int rows=0;

    double parentRatio = (double)rect().width() / (double)rect().height();
    // Use more columns than rows when parent's width > parent's height
    if ( parentRatio > 1 )
    {
      columns = (int)ceil( sqrt(numDesks) );
      rows = (int)ceil( (double)numDesks / (double)columns );
    }
    else
    {
      rows = (int)ceil( sqrt(numDesks) );
      columns = (int)ceil( (double)numDesks / (double)rows );
    }
    int w = (rect().width() - (columns+1) * spacing ) / columns;
    int h = (rect().height() - (rows+1) * spacing ) / rows;

    for (int i=0; i < numDesks; ++i)
    {
      int row = i / 2;
      int col = i % 2;

      //kdDebug() << "rc %d %d", row, col);
      rearrangeContents( QRect( col*w, row*h, w, h ), i );
    }
  }
  else
    rearrangeContents( rect() );
}

void KomposeGLWidget::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  glBindTexture(m_target, m_BgTexID);
  //   glColor4f(0.6, 0.6, 0.6, 1.0);
  glColor4f(1.0, 1.0, 1.0, 1.0);
  QSize bgSize = ((QPixmap*)(KomposeGlobal::self()->getDesktopBgPixmap()))->size();
  drawTextureRect( QRect(mapFromGlobal(pos()),bgSize), bgSize );

  glColor4f(1.0, 1.0, 1.0, 1.0);
  TaskList tl = KomposeTaskManager::self()->getTasks();
  QPtrListIterator<KomposeTask> it( tl );
  KomposeTask *task;
  while ( (task = it.current()) != 0 )
  {
    ++it;
    Q_CHECK_PTR(task);
    // Postpone the drawing of the yet to activate task for proper zorder
    if ( task == m_activateLaterTask )
      continue;
    paintTask(task);
  }

  // Now draw the yet to actiate task
  if ( m_activateLaterTask )
    paintTask(m_activateLaterTask);

  glFlush();
}

void KomposeGLWidget::paintTask(KomposeTask* task)
{
  if (task->getVisualizer()->m_glTexID != 0)
    glBindTexture(m_target, task->getVisualizer()->m_glTexID );

  QRect frameGeom( mapFromGlobal( task->getFrameGeometry().topLeft()),
                   task->getFrameGeometry().size() );
  QRect komposeGeom( task->getKomposeGeom() );
  QRect currentGeom;

  currentGeom.setX( task->getFrameGeometry().x() +
                    m_scale*(double)( komposeGeom.x() - frameGeom.x() ) );
  currentGeom.setY( task->getFrameGeometry().y() +
                    m_scale*(double)( komposeGeom.y() - frameGeom.y() ) );

  currentGeom.setWidth( task->getFrameGeometry().width() +
                        m_scale*(double)( komposeGeom.width() - frameGeom.width() ) );

  currentGeom.setHeight( task->getFrameGeometry().height() +
                         m_scale*(double)( komposeGeom.height() - frameGeom.height() ) );

  QRect currentGeom2( mapFromGlobal( currentGeom.topLeft()),
                      currentGeom.size() );

  drawTextureRect( currentGeom2, frameGeom.size() );
}

void KomposeGLWidget::drawTextureRect(QRect pos, QSize texSize)
{
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPushAttrib(GL_CURRENT_BIT);
  glEnable(m_target);
  glEnable(GL_BLEND);
  glMatrixMode(GL_TEXTURE);

  double tx_w;
  double tx_h;
  if (m_target == GL_TEXTURE_RECTANGLE_NV)
  {
    tx_w = texSize.width();
    tx_h = texSize.height();
  }
  else
  {
    tx_w = 1.0;
    tx_h = 1.0;
  }

  glBegin(GL_QUADS);
  {
    glTexCoord2f(0.0, 0.0);
    glVertex2d(pos.x(), pos.y());

    glTexCoord2f(tx_w, 0.0);
    glVertex2d(pos.x() + pos.width(), pos.y());

    glTexCoord2f(tx_w, tx_h);
    glVertex2d(pos.x() + pos.width(), pos.y() + pos.height());

    glTexCoord2f(0.0, tx_h);
    glVertex2d(pos.x(), pos.y() + pos.height());
  }
  glEnd();

  glDisable(m_target);
  glPopAttrib();
}

void KomposeGLWidget::bindTexture( const QPixmap* pixmap, uint& texIDStorage )
{
  kdDebug() << k_funcinfo << endl;
  QImage tx;
  QImage image;
  int tx_w;
  int tx_h;

  if (m_target == GL_TEXTURE_2D)
  {
    tx_w = nearest_gl_texture_size(pixmap->width()) /2;
    tx_h = nearest_gl_texture_size(pixmap->height()) /2;
  }
  else
  {
    tx_w = pixmap->width();
    tx_h = pixmap->height();
  }
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
  glBindTexture(m_target, texIDStorage);
  glTexParameterf(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


  //     gluBuild2DMipmaps(m_target, 3, tx_w, tx_h, GL_RGBA, GL_UNSIGNED_BYTE, screenshot_data);

  glTexImage2D(m_target, 0, 3, tx_w, tx_h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, screenshot_data);

  delete screenshot_data;
}

/**
 * availRect specifies the size&pos of the contents
 * Specify either rows or cols to set a fixed number of those (setting both won't work correctly)
 */
void KomposeGLWidget::rearrangeContents( const QRect& availRect, int desktop )
{
  kdDebug() << k_funcinfo << endl;
  TaskList tmp_tasks = KomposeTaskManager::self()->getTasks();
  TaskList tasks;

  KomposeTask* task = 0;
  QPtrListIterator<KomposeTask> tmp_it( tmp_tasks );
  while ( (task = tmp_it.current()) != 0 )
  {
    ++tmp_it;
    Q_CHECK_PTR(task);
    if ( desktop == -1 || task->onDesktop()-1 == desktop)
      tasks.append( task );
  }

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

  // Now find the right order
  createOrderedTaskList( m_orderedTasks, availRect, rows, columns );

  // Calculate width & height
  int w = (availRect.width() - (columns+1) * spacing ) / columns;
  int h = (availRect.height() - (rows+1) * spacing ) / rows;

  QPtrListIterator<KomposeTask> it( m_orderedTasks );
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

      geom.setX(availRect.x() + x);
      geom.setY(availRect.y() + y);
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
    if ( m_animMode == ANIM_OUT )
      m_scale = 0.;
    else
      m_scale = 1.;
    m_scale = 1.;
    m_animTimer->stop();
    if ( m_animMode == ANIM_OUT )
    {
      hide();
      KomposeViewManager::self()->activateTask( m_activateLaterTask );
    }
  }
  else
  {
    if ( m_animMode == ANIM_OUT )
      m_scale = 1. - diff / (float)ANIM_DURATION;
    else
      m_scale = diff / (float)ANIM_DURATION;
  }
  updateGL();
}

void KomposeGLWidget::showEvent( QShowEvent * )
{
  KomposeTaskManager::self()->orderListByStacking();
  m_animMode = ANIM_IN;
  m_scale = 0.;
  m_animProgress->start();
  m_animTimer->start(0);
}

void KomposeGLWidget::mouseReleaseEvent( QMouseEvent *e )
{
  if ( !rect().contains( e->pos() ) )
    return;

  switch ( e->button() )
  {
  case LeftButton:
    {
      QPtrListIterator<KomposeTask> it( m_orderedTasks );
      KomposeTask *task;
      while ( (task = it.current()) != 0 )
      {
        ++it;
        Q_CHECK_PTR(task);
        if ( task->getKomposeGeom().contains(e->pos()) )
        {
          m_animMode = ANIM_OUT;
          m_activateLaterTask = task;
          m_scale = 1.;
          m_animProgress->start();
          m_animTimer->start(0);
          m_activateLaterTask->activate();
          break;
        }
      }
      e->accept();
    }
    break;
  default:
    break;
  }
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

/**
 * Finds the best order for the layout
 * @param inList 
 * @param availRect 
 * @param rows 
 * @param columns 
 */
void KomposeGLWidget::createOrderedTaskList( TaskList& inList, const QRect& availRect, uint rows, uint columns )
{
  inList.clear();
  TaskList tasks = TaskList(KomposeTaskManager::self()->getTasks());
  uint cellWidth = availRect.width() / columns;
  uint cellHeight = availRect.height() / rows;

  KomposeTask* task;
  uint nearestDist = 0;
  for (uint r = 0; r < rows; ++r)
  {
    for (uint c = 0; c < columns; ++c)
    {
      uint x = c * cellWidth + cellWidth / 2;
      uint y = r * cellHeight + cellHeight / 2;

      // Find the shortest diagonale between cell and window
      if ( tasks.count() == 0)
        break;
      nearestDist = 0;
      KomposeTask* nearestWindow = 0;
      QPtrListIterator<KomposeTask> it( tasks );
      while ( (task = it.current()) != 0 )
      {
        ++it;
        Q_CHECK_PTR(task);

        int xdist = task->getFrameGeometry().x() - x +
                    task->getFrameGeometry().width() / 2;
        int ydist = task->getFrameGeometry().y() - y +
                    task->getFrameGeometry().height() / 2;
        int dist = sqrt( xdist*xdist + ydist*ydist );
        if ( nearestDist == 0 || dist < nearestDist )
        {
          nearestDist = dist;
          nearestWindow = task;
        }
      }

      inList.append(nearestWindow);
      tasks.remove(nearestWindow);
    }
  }
}
