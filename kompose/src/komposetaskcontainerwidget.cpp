//
// C++ Implementation: komposetaskcontainerwidget
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposetaskcontainerwidget.h"
#include "komposetaskmanager.h"
#include "komposelayout.h"
#include "komposesettings.h"
#include "komposetaskwidget.h"

#include <qobjectlist.h>
#include <qwidget.h>
#include <qtimer.h>

static bool controlHold = false; // is the control key pressed

KomposeTaskContainerWidget::KomposeTaskContainerWidget( int desk, QWidget *parent, KomposeLayout *l, const char *name)
    : KomposeWidget(parent, l, name),
    desktop( desk )
{
  connect(KomposeTaskManager::instance(), SIGNAL(taskDesktopChanged(KomposeTask*, int, int )), 
    SLOT(reparentTaskWidget(KomposeTask*, int, int )) );
}


KomposeTaskContainerWidget::~KomposeTaskContainerWidget()
{}

void KomposeTaskContainerWidget::reparentTaskWidget( KomposeTask* task, int fromDesktop, int toDesktop )
{
  // Ignore to all desktops
  if (toDesktop==-1)
    return;
  
  qDebug("KomposeTaskContainerWidget::reparentTaskWidget()");
  
  // Delete from current
  if ( desktop == fromDesktop -1 )
  {
    KomposeTaskWidget *child;
    QPtrListIterator<KomposeWidget> it( *(layout->getManagedWidgets()));
    while ( (child = dynamic_cast<KomposeTaskWidget*>(it.current()) ) != 0 )
    {
      ++it;
      if (child->getTask() == task)
      {
        removeChild( task );
        layout->remove(child);
        //child->deleteLater();
        child->close(true);
        //QTimer::singleShot( 200, layout, SLOT( arrangeLayout() ) );
        return;
      }
    }
  }
  
  // Delete from current
  if ( desktop == toDesktop -1 )
  {
    createTaskWidget( task );
    QTimer::singleShot( 200, layout, SLOT( arrangeLayout() ) );
//        layout->arrangeLayout();
  }
}

void KomposeTaskContainerWidget::keyPressEvent ( QKeyEvent * e )
{
  if ( e->key() == Qt::Key_Control )
  {
    controlHold = true;
    e->accept();
  }
}

void KomposeTaskContainerWidget::keyReleaseEvent ( QKeyEvent * e )
{
  if ( e->key() == Qt::Key_Control )
  {
    controlHold = false;
    e->accept();
    return;
  }
  
  if ( e->key() == Qt::Key_Right || e->key() == Qt::Key_D || e->key() == Qt::Key_H ||
       e->key() == Qt::Key_Left  || e->key() == Qt::Key_A || e->key() == Qt::Key_J ||
       e->key() == Qt::Key_Up    || e->key() == Qt::Key_W || e->key() == Qt::Key_K ||
       e->key() == Qt::Key_Down  || e->key() == Qt::Key_S || e->key() == Qt::Key_L )
  {
    if ( controlHold && desktop != -1 )
    {
      e->ignore();
      return;
    }
    
    qDebug("KomposeTaskContainerWidget::keyReleaseEvent - %s, Movement key pressed", className() );
    // Map keys to directions
    int direction = DLAYOUT_RIGHT;
    switch( e->key() )
    {
    case Qt::Key_Right:
      direction = DLAYOUT_RIGHT;
      break;
    case Qt::Key_D:
      direction = DLAYOUT_RIGHT;
      break;
    case Qt::Key_L:
      direction = DLAYOUT_RIGHT;
      break;
    case Qt::Key_Left:
      direction = DLAYOUT_LEFT;
      break;
    case Qt::Key_A:
      direction = DLAYOUT_LEFT;
      break;
    case Qt::Key_H:
      direction = DLAYOUT_LEFT;
      break;
    case Qt::Key_Up:
      direction = DLAYOUT_TOP;
      break;
    case Qt::Key_W:
      direction = DLAYOUT_TOP;
      break;
    case Qt::Key_K:
      direction = DLAYOUT_TOP;
      break;
    case Qt::Key_Down:
      direction = DLAYOUT_BOTTOM;
      break;
    case Qt::Key_S:
      direction = DLAYOUT_BOTTOM;
      break;
    case Qt::Key_J:
      direction = DLAYOUT_BOTTOM;
      break;
    }
    
    focusNeighbourChild( direction );
    e->accept();
    
    return;
  }
  
  e->ignore();
}


bool KomposeTaskContainerWidget::focusNeighbourChild( int direction )
{
  bool successfull = false;
  if ( !children()->containsRef(focusWidget()) )
  {
    qDebug("KomposeTaskContainerWidget::keyReleaseEvent - No widget focussed. Focussing first" );
    const QObjectList *lst = children();

    if ( lst )
    {
      QObjectListIterator it( *lst );
      QWidget *widget;
      while ( widget = (QWidget*)it.current() )
      {
        if (widget->inherits("KomposeTaskWidget") || widget->inherits("KomposeDesktopWidget"))
        {
          qDebug("KomposeTaskContainerWidget::keyReleaseEvent - Focussing %s", widget->className() );
          widget->setFocus();
          successfull = true;
          break;
        }
        ++it;
      }
    }
  }
  else
  {
    KomposeWidget *widget;
    if ( ( widget = layout->getNeighbour( dynamic_cast<KomposeWidget*>(focusWidget()), direction, WLAYOUT_BOTH  ) ) != 0 )
    {
      qDebug("KomposeTaskContainerWidget::keyReleaseEvent - Focussing %s", widget->className() );
      widget->setFocus();
      successfull = true;
    }
  }

  return successfull;
}


void KomposeTaskContainerWidget::createTaskWidgets()
{
  TaskList tl = KomposeTaskManager::instance()->getTasks();
  QPtrListIterator<KomposeTask> it( tl );
  KomposeTask *task;
  while ( (task = it.current()) != 0 )
  {
    ++it;
    Q_CHECK_PTR(task);
    createTaskWidget( task);
  }
}


void KomposeTaskContainerWidget::createTaskWidget( KomposeTask* task )
{
  if ( desktop == -1 || desktop == task->onDesktop()-1 || task->onDesktop()==-1)
  {
    qDebug("KomposeTaskContainerWidget::createTaskWidget() (Container: %s, WId: %d, onDesktop: %d)",
     this->className(), task->window(), task->onDesktop() );
    KomposeTaskWidget *taskwidget = new KomposeTaskWidget( task, this );
    taskwidget->show();
    connect( taskwidget, SIGNAL(requestRemoval(KomposeWidget*)), SLOT(requestRemoval(KomposeWidget*)) );
  }
}

int KomposeTaskContainerWidget::getHeightForWidth( int w ) const
{
  return -1;
}

int KomposeTaskContainerWidget::getWidthForHeight( int h ) const
{
  return -1;
}

double KomposeTaskContainerWidget::getAspectRatio()
{
  return -1;
}

void KomposeTaskContainerWidget::requestRemoval( KomposeWidget *obj )
{
  layout->remove(obj);
  // removeChild( obj );  // FIXME: This causes segfaults although it would
                          // be the correct way (ChildRemoveEvents to rearrange the layout...)
  !obj->close(true);
  layout->arrangeLayout();
}

  
#include "komposetaskcontainerwidget.moc"
