//
// C++ Implementation: komposesystray
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposesystray.h"

#include "komposetaskmanager.h"
#include "komposefullscreenwidget.h"
#include "komposesettings.h"

#include <kapplication.h>
#include <kaction.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kiconloader.h>
#include <kaccel.h>
#include <kkeydialog.h>


KomposeSysTray::KomposeSysTray(QWidget *parent, const char *name)
    : KSystemTray(parent, name)
{
  // Actions
  actQuit = KStdAction::quit( kapp, SLOT(quit()), actionCollection() );
  actShowVirtualDesktopView = new KAction(i18n("&Show Kompose"), "background",
                                          0,
                                          KomposeTaskManager::instance(), SLOT(createVirtualDesktopView()),
                                          actionCollection(), "showVirtualDesktopView");
  actPreferencesDialog      = KStdAction::preferences( KomposeSettings::instance(), SLOT(showPreferencesDlg()),
                              actionCollection() );

  actConfigGlobalShortcuts  = KStdAction::keyBindings(this, SLOT(showGlobalShortcutsSettingsDialog()),
                              actionCollection(), "options_configure_global_keybinding");
  actConfigGlobalShortcuts->setText(i18n("Configure &Global Shortcuts..."));


  // Create Menu
  menu = new KPopupMenu( this );
  menu->insertTitle( SmallIcon("desktop"), kapp->caption() );  // FIXME: use kapp->miniIcon()
  move( -1000, -1000 );
  // Fill Menu
  menu->insertSeparator();
  actShowVirtualDesktopView->plug(menu);
  actPreferencesDialog->plug(menu);
  actConfigGlobalShortcuts->plug(menu);
  actQuit->plug(menu);

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
    KomposeTaskManager::instance()->createVirtualDesktopView();
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

void KomposeSysTray::showGlobalShortcutsSettingsDialog()
{
  KKeyDialog::configure( KomposeSettings::instance()->getGlobalAccel() );
  KomposeSettings::instance()->writeConfig();
}


#include "komposesystray.moc"
