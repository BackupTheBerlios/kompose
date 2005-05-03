//
// C++ Implementation: komposeglwidget
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposeglfullscreenwidget.h"

#include "komposelayout.h"
#include "komposetaskmanager.h"
#include "komposegldesktopwidget.h"
#include <kdebug.h>

#include <GL/gl.h>
#include <GL/glut.h>

KomposeGLFullscreenWidget::KomposeGLFullscreenWidget(QWidget *parent, const char *name) :
    QGLWidget(parent, name)
{
  qDebug("KomposeGLFullscreenWidget::KomposeGLFullscreenWidget()");
  layout = new KomposeLayout( this );

  setWindowState(Qt::WindowMaximized | Qt::WindowActive);
  showFullScreen();
  
  createDesktopWidgets();
}


KomposeGLFullscreenWidget::~KomposeGLFullscreenWidget()
{}

void KomposeGLFullscreenWidget::createDesktopWidgets()
{
  // Create a Widget for every desktop
  for (int i=0; i < KomposeTaskManager::instance()->getNumDesktops(); ++i)
  {
    int row = i / 2;
    int col = i % 2;
    //qDebug("rc %d %d", row, col);
    desktop[i] = new KomposeGLDesktopWidget(i, this);
    layout->add(dynamic_cast<KomposeWidgetInterface*>(desktop[i]));
  }
}

void KomposeGLFullscreenWidget::initializeGL()
{
  // Set up the rendering context, define display lists etc.:
  if( !format().hasOpenGL() )
  {
    qWarning( "KomposeGLFullscreenWidget::initializeGL() - OpenGL not supported!" );
    return;
  }

  if ( !format().doubleBuffer() )
  {
    qWarning( "KomposeGLFullscreenWidget::initializeGL() - Direct rendering enabled !" );
  }
//   glShadeModel(GL_SMOOTH);
// 
//   format().setDirectRendering( true );
//   format().setDoubleBuffer( true );
//   format().setRgba( true );
//   format().setDepth ( false );
//   format().setAccum( false );
//   format().setStencil( false );
//   format().setAlpha( true );

  // Alpha blend
//   glEnable( GL_BLEND );
//   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  // All smoothing stuff
//   glEnable( GL_POLYGON_SMOOTH );
//   glHint( GL_POLYGON_SMOOTH_HINT, GL_FASTEST );
//   glEnable( GL_POINT_SMOOTH );
//   glHint( GL_POINT_SMOOTH_HINT, GL_FASTEST );
//   glEnable( GL_LINE_SMOOTH );
//   glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST );

  // Lighting and Depth Test
  glDisable( GL_LIGHTING );
//   glDisable( GL_DEPTH_TEST );
//   glDisable( GL_NORMALIZE );

  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT );

  /*! Setup sizes */
//   glLineWidth( 1.0 );
//   glPointSize( 2.0 );

  setOrthographicProjection();
}

void KomposeGLFullscreenWidget::resizeGL( int w, int h )
{
  // setup viewport, projection etc.:
  setOrthographicProjection();
  layout->arrangeLayout();
}


/*! draw OpenGL scene ( called from Qt ) */
void KomposeGLFullscreenWidget::paintGL()
{
  qDebug("KomposeGLFullscreenWidget::paintGL()");

  glPushMatrix();
  
  // clears the color buffer (this will set the window to black)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw Desktop Widgets
  for (int i=0; i < KomposeTaskManager::instance()->getNumDesktops(); ++i)
  {
    // FIXME: Make desktop arrays dynamic or at least avoid KomposeTaskManager::instance()->getNumDesktops() here
    desktop[i]->draw();
  }

  glFlush();
  glPopMatrix();
}


void KomposeGLFullscreenWidget::setOrthographicProjection()
{
  qDebug("KomposeGLFullscreenWidget::setOrthographicProjection() - %dx%d", width(), height());
  glViewport( 0, 0, (GLint)width(), (GLint)height()  );
  // switch to projection mode
  glMatrixMode(GL_PROJECTION);
  // reset matrix
  glLoadIdentity();
  // set a 2D orthographic projection
  gluOrtho2D(0.0, (GLdouble)width(), 0.0, (GLdouble)height());
  // invert the y axis, down is positive
  glScalef(1, -1, 1);
  // mover the origin from the bottom left corner
  // to the upper left corner
  glTranslatef(0, -height(), 0);
  glMatrixMode(GL_MODELVIEW);;
}

// Redirect these functions to QGLWidget

void KomposeGLFullscreenWidget::setGeom ( const QRect &rect )
{
  QGLWidget::setGeometry( rect );
}

void KomposeGLFullscreenWidget::setGeom ( const QSize &size )
{
  QGLWidget::resize( size );
}

QSize KomposeGLFullscreenWidget::getSize() const
{
  return QGLWidget::size();
}

QRect KomposeGLFullscreenWidget::getRect() const
{
  return QGLWidget::rect();
}

void KomposeGLFullscreenWidget::removeChildWidget( KomposeWidgetInterface* obj )
{
  QGLWidget::removeChild((QObject *) obj);
}

#include "komposeglfullscreenwidget.moc"
