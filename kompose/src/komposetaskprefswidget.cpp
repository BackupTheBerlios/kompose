//
// C++ Implementation: komposetaskprefswidget
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposetaskprefswidget.h"

#include "komposetaskwidget.h"
#include "komposetask.h"
#include "komposetaskprefswidget.h"
#include "komposesettings.h"

#include <ktoolbar.h>
#include <ktoolbarbutton.h>
#include <kactioncollection.h>
#include <kaction.h>
#include <kshortcut.h>
#include <kiconloader.h>
#include <kapplication.h>

#include <qiconset.h>

KomposeTaskPrefsWidget::KomposeTaskPrefsWidget(KomposeTaskWidget *parent, const char *name)
    : QDockArea(Qt::Horizontal , QDockArea::Normal, parent, name)
{
  KToolBar* pToolBar = new KToolBar( this, "Task Actions" );
  pToolBar->setLabel("Task");
  pToolBar->setResizeEnabled ( false );
  pToolBar->setMovingEnabled ( false );
  pToolBar->setHorizontallyStretchable ( false );
  pToolBar->setVerticallyStretchable ( false );
  pToolBar->setIconSize( 16 );

  KActionCollection *actColl = new KActionCollection( this );

  actCloseTask = new KAction( trUtf8("Close"), "fileclose", Key_Delete , parent->getTask(),
                              SLOT( close() ), actColl, "closeTask" );
                              actCloseTask->setToolTip("Close");
  actMinimizeRestoreTask = new KAction( trUtf8("Minimize/Restore"), "", KShortcut() , this,
                                        SLOT( slotMinimizeRestoreToggled() ), actColl, "minimizeRestoreTask" );

  actMinimizeRestoreTask->plug(pToolBar);
  actCloseTask->plug(pToolBar);
  
  setActionIcons();

  lineUp(false);

  resize(pToolBar->size());
}


KomposeTaskPrefsWidget::~KomposeTaskPrefsWidget()
{}

/*
 * Set the toggle icons for some actions
 *
 * The inverse flag is a hack to allow toggling of the icons when the minimized/restored event
 * hasn't yet reached the Task object ( which is the case on buttonpress)
 */
void KomposeTaskPrefsWidget::setActionIcons( bool inverse )
{
  if ( ( ((KomposeTaskWidget *)parentWidget())->getTask()->isIconified() && !inverse ) ||
       ( !((KomposeTaskWidget *)parentWidget())->getTask()->isIconified() && inverse ) )
    actMinimizeRestoreTask->setIconSet( kapp->iconLoader()->loadIconSet ( "up", KIcon::NoGroup, 16 ) );
  else
    actMinimizeRestoreTask->setIconSet( kapp->iconLoader()->loadIconSet ( "bottom", KIcon::NoGroup, 16 ) );
}

void KomposeTaskPrefsWidget::slotMinimizeRestoreToggled()
{
  setActionIcons( true );
  ((KomposeTaskWidget *)parentWidget())->getTask()->minimizeOrRestore();
}


#include "komposetaskprefswidget.moc"
