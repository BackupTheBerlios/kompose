//
// C++ Implementation: komposesystray
//
// Description:
//
//
// Author: Hans Oischinger <oisch@users.berlios.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposesystray.h"

#include "komposeviewmanager.h"
#include "komposefullscreenwidget.h"
#include "komposesettings.h"
#include "komposeglobal.h"

#include <kapplication.h>
#include <kaction.h>
#include <kpopupmenu.h>


KomposeSysTray::KomposeSysTray(QWidget *parent, const char *name)
    : KSystemTray(parent, name)
{
  // Create Menu
  menu = contextMenu();
  move( -1000, -1000 );
  // Fill Menu
  KomposeGlobal::instance()->getActShowWorldView()->plug(menu);
  KomposeGlobal::instance()->getActShowVirtualDesktopView()->plug(menu);
  KomposeGlobal::instance()->getActShowCurrentDesktopView()->plug(menu);
  menu->insertSeparator();
  KomposeGlobal::instance()->getActPreferencesDialog()->plug(menu);
  KomposeGlobal::instance()->getActConfigGlobalShortcuts()->plug(menu);
  KomposeGlobal::instance()->getActAboutDlg()->plug(menu);
}


KomposeSysTray::~KomposeSysTray()
{}

void KomposeSysTray::mouseReleaseEvent (QMouseEvent * e)
{}

void KomposeSysTray::mousePressEvent ( QMouseEvent * e )
{
  if ( !rect().contains( e->pos() ) )
    return;

  switch ( e->button() )
  {
  case LeftButton:
    KomposeViewManager::instance()->createView( KomposeSettings::instance()->getDefaultView() );
    break;
  case MidButton:
    // fall through
  case RightButton:
    contextMenuAboutToShow( menu );
    menu->popup( e->globalPos() );
    break;
  default:
    // nothing
    break;
  }
}


#include "komposesystray.moc"
