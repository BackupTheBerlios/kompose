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

KomposeTaskContainerWidget::KomposeTaskContainerWidget( int desk, QWidget *parent, KomposeLayout *l, const char *name)
    : KomposeWidget(parent, l, name),
    desktop( desk )
{}


KomposeTaskContainerWidget::~KomposeTaskContainerWidget()
{}

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
    taskwidget->show();
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
