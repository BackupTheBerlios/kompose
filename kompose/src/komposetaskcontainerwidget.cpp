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

static bool controlHold = false; // is the control key pressed

KomposeTaskContainerWidget::KomposeTaskContainerWidget( int desk, QWidget *parent, KomposeLayout *l, const char *name)
    : KomposeWidget(parent, l, name),
    desktop( desk )
{
}


KomposeTaskContainerWidget::~KomposeTaskContainerWidget()
{}


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
  }
  
  if ( e->key() == Qt::Key_Right || Qt::Key_D ||
       e->key() == Qt::Key_Left  || Qt::Key_A ||
       e->key() == Qt::Key_Up    || Qt::Key_W ||
       e->key() == Qt::Key_Down  || Qt::Key_S )
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
    case Qt::Key_Left:
      direction = DLAYOUT_LEFT;
      break;
    case Qt::Key_A:
      direction = DLAYOUT_LEFT;
      break;
    case Qt::Key_Up:
      direction = DLAYOUT_TOP;
      break;
    case Qt::Key_W:
      direction = DLAYOUT_TOP;
      break;
    case Qt::Key_Down:
      direction = DLAYOUT_BOTTOM;
      break;
    case Qt::Key_S:
      direction = DLAYOUT_BOTTOM;
    }
    
    focusNeighbourChild( direction );
    e->accept();
  }
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
    if ( ( widget = dynamic_cast<KomposeWidget*>(layout->getNeighbour( dynamic_cast<KomposeWidgetInterface*>(focusWidget()), direction, WLAYOUT_BOTH ) ) ) != 0 )
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
  qDebug("KomposeTaskContainerWidget::createTaskWidgets() on %d tasks", tl.count());
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
  if ( desktop == -1 || desktop == task->onDesktop()-1 )
  {
    qDebug("KomposeTaskContainerWidget::createTaskWidget()" );
    KomposeTaskWidget *taskwidget = new KomposeTaskWidget( task, this );
//     taskwidget->show();
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


#include "komposetaskcontainerwidget.moc"
