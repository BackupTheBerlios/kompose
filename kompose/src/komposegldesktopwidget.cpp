//
// C++ Implementation: komposegldesktopwidget
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposegldesktopwidget.h"

#include "komposetask.h"
#include "komposegltaskwidget.h"
#include "komposetaskmanager.h"

#include <qdesktopwidget.h>
#include <qobjectlist.h>
#include <kdebug.h>

#include <GL/gl.h>

KomposeGLDesktopWidget::KomposeGLDesktopWidget(int desktop, QObject *parent, const char *name)
    : KomposeGLWidget( parent ),
    deskNum( desktop )
{
  // Retrieve geometry
  QDesktopWidget *deskwidget = new QDesktopWidget();
  deskRect = deskwidget->availableGeometry();
  delete deskwidget;

  createTaskWidgets();

  //connect( KomposeTaskManager::instance(), SIGNAL( newTask( KomposeTask* ) ), this, SLOT( createTaskWidget( KomposeTask* ) ) );
}


KomposeGLDesktopWidget::~KomposeGLDesktopWidget()
{}

void KomposeGLDesktopWidget::createTaskWidgets()
{
  TaskList tl = KomposeTaskManager::instance()->getTasks();
  qDebug("KomposeGLDesktopWidget::createTaskWidgets() on %d tasks", tl.count());
  QPtrListIterator<KomposeTask> it( tl );
  KomposeTask *task;
  while ( (task = it.current()) != 0 )
  {
    ++it;
    Q_CHECK_PTR(task);
    createTaskWidget( task );
  }
}


void KomposeGLDesktopWidget::createTaskWidget( KomposeTask* task )
{
  if (deskNum == task->onDesktop()-1 )
  {
    qDebug("KomposeGLDesktopWidget::createTaskWidget()" );
    KomposeGLTaskWidget *taskwidget = new KomposeGLTaskWidget( task, this, 0, "" );
  }
}

int KomposeGLDesktopWidget::getHeightForWidth( int w ) const
{
  qDebug("KomposeGLDesktopWidget::getHeightForWidth()");
  return ((double)w / (double)deskRect.width()) * deskRect.height();
}

int KomposeGLDesktopWidget::getWidthForHeight( int h ) const
{
  qDebug("KomposeGLDesktopWidget::getWidthForHeight()");
  return ((double)h / (double)deskRect.height()) * deskRect.width();
}

double KomposeGLDesktopWidget::getAspectRatio()
{
  qDebug("KomposeGLDesktopWidget::getAspectRatio()");
  return (double)deskRect.width() / (double)deskRect.height();
}


void KomposeGLDesktopWidget::draw()
{
  KomposeGLWidget::draw();
  qDebug("KomposeGLDesktopWidget::draw() - %d,%d %dx%d", getRect().x(), getRect().y(), getRect().width(), getRect().height());
  glColor3f(0.0f, 0.0f, 1.0f);

  glBegin( GL_QUADS );
  glVertex2i( getRect().x() + getRect().width(), getRect().y() );
  glVertex2i( getRect().x(), getRect().y() );
  glVertex2i( getRect().x(), getRect().y() + getRect().height() );
  glVertex2i( getRect().x() + getRect().width(), getRect().y() + getRect().height() );
  glEnd();

  // Draw Task Widgets
  QPtrListIterator<KomposeGLWidget> it( list );

  // Check or empty list
  if (it.count() == 0)
  {
    qDebug("KomposeGLDesktopWidget::draw() - empty list... skipping!");
    return;
  }
  
  KomposeGLWidget *widget;
  while ( (widget = (KomposeGLWidget*)it.current()) != 0 )
  {
    ++it;
    widget->draw();
  }
}

#include "komposegldesktopwidget.moc"
