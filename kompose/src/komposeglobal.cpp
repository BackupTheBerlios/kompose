//
// C++ Implementation: komposeglobal
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposeglobal.h"


#include "komposetaskmanager.h"
#include "komposesettings.h"

#include <kapplication.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kaccel.h>
#include <kkeydialog.h>
#include <kaboutapplication.h>

#include <dcopclient.h> 

// #include those AFTER Qt-includes!
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Imlib2.h>


static KomposeGlobal* globalInstance = 0;
  Display *disp;
  
/*
 * KomposeSettings is a singleton
 */
KomposeGlobal* KomposeGlobal::instance()
{
  if ( !globalInstance )
  {
    qDebug("KomposeSettings::instance() - Creating Singleton instance");
    KomposeGlobal *settiglobalInstance = new KomposeGlobal();
  }
  return globalInstance;
}


KomposeGlobal::KomposeGlobal(QObject *parent, const char *name)
    : QObject(parent, name),
    hideSystray( false ),
    singleShot( false )
{
  globalInstance = this;
}

void KomposeGlobal::initGui()
{
  initImlib();
  
  // Initialise the Singleton instances
  KomposeSettings::instance();
  KomposeTaskManager::instance();

  // Create DCop Client
  kapp->dcopClient()->setDefaultObject( "KomposeTaskMgrDcopIface" );
  
  initActions();

  if ( !hideSystray && !singleShot )
  {
    // Create systray
    systray = new KomposeSysTray();
    kapp->setMainWidget( systray );
    systray->setPixmap( systray->loadIcon( "kompose" ) );
    
    actionCollection->setWidget( systray );
    systray->show();
  } else
    qDebug("KomposeGlobal::initGui() - Hiding systray icon");
  
  if ( singleShot )
  {
    qDebug("KomposeGlobal::initGui() - SingleShot has been selected");
    KomposeTaskManager::instance()->createView();
  }
}

void KomposeGlobal::initActions()
{
  actionCollection = new KActionCollection( (QWidget*)0 );
  
  // Actions
  actQuit = KStdAction::quit( kapp, SLOT(quit()), actionCollection );
  actShowWorldView = new KAction(i18n(QString("Komposé (ungrouped)").utf8()), "kompose",
                                 0,
                                 KomposeTaskManager::instance(), SLOT(createWorldView()),
                                 actionCollection, "showWorldView");
  actShowVirtualDesktopView = new KAction(i18n(QString("Komposé (grouped by Virtual Desktops)").utf8()), "kompose",
                                          0,
                                          KomposeTaskManager::instance(), SLOT(createVirtualDesktopView()),
                                          actionCollection, "showVirtualDesktopView");
  actPreferencesDialog      = KStdAction::preferences( KomposeSettings::instance(), SLOT(showPreferencesDlg()), actionCollection );

  actConfigGlobalShortcuts  = KStdAction::keyBindings(this, SLOT(showGlobalShortcutsSettingsDialog()),
                              actionCollection, "options_configure_global_keybinding");
  actConfigGlobalShortcuts->setText(i18n("Configure &Global Shortcuts..."));
  actAboutDlg      = new KAction(i18n(QString("About Komposé").utf8()), "kompose",
                                 0,
                                 this, SLOT(showAbutDlg()),
                                 actionCollection, "showAbutDlg");
}


KomposeGlobal::~KomposeGlobal()
{
}


void KomposeGlobal::showGlobalShortcutsSettingsDialog()
{
  KKeyDialog::configure( KomposeSettings::instance()->getGlobalAccel() );
  KomposeSettings::instance()->writeConfig();
}


void KomposeGlobal::showAbutDlg()
{
  KAboutApplication* kabout = new KAboutApplication();
  kabout->show();
}

void KomposeGlobal::initImlib()
{
  Display *disp;
  Visual *vis;
  Colormap cm;
  int screen;
  disp = QPaintDevice::x11AppDisplay();
  vis   = DefaultVisual(disp, DefaultScreen(disp));
  cm    = DefaultColormap(disp, DefaultScreen(disp));
  //     context = imlib_context_new();

  //     imlib_context_push(d->context);
  imlib_set_cache_size(0);
  imlib_set_color_usage(128);
  imlib_context_set_dither(1);
  imlib_context_set_display(disp);
  imlib_context_set_visual(vis);
  imlib_context_set_colormap(cm);
}

#include "komposeglobal.moc"
