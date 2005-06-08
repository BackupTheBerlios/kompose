//
// C++ Implementation: komposetaskcontainerwidget
//
// Description:
//
//
// Author: Hans Oischinger <hans.oischinger@kde-mail.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposetaskcontainerwidget.h"
#include "komposetaskmanager.h"
#include "komposeviewmanager.h"
#include "komposelayout.h"
#include "komposesettings.h"
#include "komposetaskwidget.h"

#include <qobjectlist.h>
#include <qwidget.h>
#include <qtimer.h>
#include <kdebug.h>

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
  // noops
  if ( (fromDesktop==-1 && ((toDesktop==-1) || desktop == toDesktop-1)) || desktop==-2)
    return;

  kdDebug() << "KomposeTaskContainerWidget::reparentTaskWidget()" << endl;

  // Delete from current
  if ( (toDesktop!= -1) && (desktop == fromDesktop-1 || fromDesktop==-1) )
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
        return;
      }
    }
  }

  // Add to new
  if ( desktop == toDesktop -1 || (toDesktop == -1 && fromDesktop-1!=desktop) )
  {
    createTaskWidget( task, true );
    //QTimer::singleShot( 200, layout, SLOT( arrangeLayout() ) );
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

    kdDebug() << "KomposeTaskContainerWidget::keyReleaseEvent - " << className() << ", Movement key pressed" << endl;
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
    kdDebug() << "KomposeTaskContainerWidget::keyReleaseEvent - No widget focussed. Focussing first" << endl;
    const QObjectList *lst = children();

    if ( lst )
    {
      QObjectListIterator it( *lst );
      QWidget *widget;
      while ( (widget = (QWidget*)it.current() ) )
      {
        if (widget->inherits("KomposeTaskWidget") || widget->inherits("KomposeDesktopWidget"))
        {
          kdDebug() << "KomposeTaskContainerWidget::keyReleaseEvent - Focussing " << widget->className() << endl;
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
      kdDebug() << "KomposeTaskContainerWidget::keyReleaseEvent - Focussing " << widget->className() << endl;
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


void KomposeTaskContainerWidget::createTaskWidget( KomposeTask* task, bool manualShow )
{
  if ( desktop == -1 || desktop == task->onDesktop()-1 || task->onDesktop()==-1)
  {
    kdDebug() << "KomposeTaskContainerWidget::createTaskWidget() (Container: " << this->className() << ", WId: " << task->window() << ", onDesktop: " << task->onDesktop() << ")" << endl;
    KomposeTaskWidget *taskwidget = new KomposeTaskWidget( task, this );
    if (manualShow)
      taskwidget->show();
    connect( taskwidget, SIGNAL(requestRemoval(KomposeWidget*)), SLOT(requestRemoval(KomposeWidget*)) );
  }
}

int KomposeTaskContainerWidget::getHeightForWidth( int ) const
{
  return -1;
}

int KomposeTaskContainerWidget::getWidthForHeight( int ) const
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

void KomposeTaskContainerWidget::childEvent( QChildEvent * ce)
{
  KomposeWidget::childEvent(ce);
  // ReLayout when we are in a active view and a new window appeared somewhere
  if ( KomposeViewManager::instance()->hasActiveView() )
    layout->arrangeLayout();
}

#include "komposetaskcontainerwidget.moc"
