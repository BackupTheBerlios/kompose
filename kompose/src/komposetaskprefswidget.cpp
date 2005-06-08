//
// C++ Implementation: komposetaskprefswidget
//
// Description:
//
//
// Author: Hans Oischinger <hans.oischinger@kde-mail.net>, (C) 2004
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
#include <klocale.h>

#include <qiconset.h>

KomposeTaskPrefsWidget::KomposeTaskPrefsWidget(KomposeTaskWidget *parent, const char *name)
    : QDockArea(Qt::Horizontal , QDockArea::Normal, parent, name)
{
  KToolBar* pToolBar = new KToolBar( this, i18n("Task Actions") );
  pToolBar->setLabel(i18n("Task"));
  pToolBar->setResizeEnabled ( false );
  pToolBar->setMovingEnabled ( false );
  pToolBar->setHorizontallyStretchable ( false );
  pToolBar->setVerticallyStretchable ( false );
  pToolBar->setIconSize( 16 );
  
  parent->getActMinimizeRestoreTask()->plug(pToolBar);
  parent->getActCloseTask()->plug(pToolBar);
  
  lineUp(false);

  resize(pToolBar->size());
}


KomposeTaskPrefsWidget::~KomposeTaskPrefsWidget()
{
}



#include "komposetaskprefswidget.moc"
