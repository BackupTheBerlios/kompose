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
#include "komposeviewmanager.h"
#include "komposesettings.h"
#include "komposesystray.h"

#include <qtimer.h>

#include <kapplication.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kaccel.h>
#include <kkeydialog.h>
#include <kaboutapplication.h>
#include <ksharedpixmap.h>

#include <dcopclient.h>

// #include those AFTER Qt-includes!
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Imlib2.h>

#ifdef COMPOSITE
#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xrender.h>
#endif

static KomposeGlobal* globalInstance = 0;
Display *disp;

/**
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
    singleShot( false ),
    xcomposite(0),
    damageEvent(0)
{
  globalInstance = this;

  desktopBgPixmap = new KSharedPixmap;
  connect(desktopBgPixmap, SIGNAL(done(bool)), SLOT(slotDone(bool)));
  initSharedPixmaps();
  initCompositeExt();
  initImlib();

  connect( KomposeSettings::instance(), SIGNAL(settingsChanged()), this, SLOT(initCompositeExt()) );
}

KomposeGlobal::~KomposeGlobal()
{
  delete systray;
  delete actionCollection;
  delete actConfigGlobalShortcuts;
  delete actPreferencesDialog;
  delete actShowVirtualDesktopView;
  delete actShowCurrentDesktopView;
  delete actShowWorldView;
  delete actAboutDlg;
  delete actQuit;
  delete desktopBgPixmap;
}

/**
 * Gives us control about when the GUI should appear. Called from outside
 * Called from outside as KkomposeGlobal is a singleton that can be instantiated at any time
 */
void KomposeGlobal::initGui()
{
  // Initialise the Singleton instances
  KomposeSettings::instance();
  KomposeViewManager::instance();
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
  }
  else
    qDebug("KomposeGlobal::initGui() - Hiding systray icon");

  if ( singleShot )
  {
    qDebug("KomposeGlobal::initGui() - SingleShot has been selected");
    KomposeViewManager::instance()->createView();
  }
}


/**
 * Initialise Kompose's global actions. Retrieve these via the getters in KomposeGlobal
 */
void KomposeGlobal::initActions()
{
  actionCollection = new KActionCollection( (QWidget*)0 );

  // Actions
  actQuit = KStdAction::quit( kapp, SLOT(quit()), actionCollection );
  actShowWorldView = new KAction(i18n(QString::fromUtf8("Komposé (ungrouped)").utf8()), "kompose",
                                 0,
                                 KomposeViewManager::instance(), SLOT(createWorldView()),
                                 actionCollection, "showWorldView");
  actShowVirtualDesktopView = new KAction(i18n(QString::fromUtf8("Komposé (grouped by virtual desktops)").utf8()), "kompose",
                                          0,
                                          KomposeViewManager::instance(), SLOT(createVirtualDesktopView()),
                                          actionCollection, "showVirtualDesktopView");
  actShowCurrentDesktopView = new KAction(i18n(QString::fromUtf8("Komposé (current virtual desktop)").utf8()), "kompose",
                                          0,
                                          KomposeViewManager::instance(), SLOT(createCurrentDesktopView()),
                                          actionCollection, "showCurrentDesktopView");

  actPreferencesDialog      = KStdAction::preferences( KomposeSettings::instance(), SLOT(showPreferencesDlg()), actionCollection );
  
  actConfigGlobalShortcuts  = KStdAction::keyBindings(this, SLOT(showGlobalShortcutsSettingsDialog()),
                              actionCollection, "options_configure_global_keybinding");
  actConfigGlobalShortcuts->setText(i18n("Configure &Global Shortcuts..."));
  actAboutDlg      = new KAction(i18n(QString::fromUtf8("About Komposé").utf8()), "kompose",
                                 0,
                                 this, SLOT(showAboutDlg()),
                                 actionCollection, "showAboutDlg");
}

void KomposeGlobal::initSharedPixmaps()
{
  enablePixmapExports();
  // When Kompose is started by session management the bg shared pixmap may not be available yet
  if (!desktopBgPixmap->isAvailable( pixmapName(1) ))
  {
    qWarning("KomposeGlobal::initSharedPixmaps() - Pixmap not available");
    //enablePixmapExports();
    QTimer::singleShot( 1000, this, SLOT( initSharedPixmaps() ) );
    //initSharedPixmaps();
    //return;
  }

  qDebug("KomposeGlobal::initSharedPixmaps()");
  // To simplify things we take the background of the first desktop
  desktopBgPixmap->loadFromShared( pixmapName(1) );
}

QString KomposeGlobal::pixmapName(int desk)
{
  // To simplify things we take the background of the first desktop
  QString pattern = QString("DESKTOP%1");
  int screen_number = DefaultScreen(qt_xdisplay());
  if (screen_number)
  {
    pattern = QString("SCREEN%1-DESKTOP").arg(screen_number) + "%1";
  }
  return pattern.arg( desk );
}

void KomposeGlobal::slotDone(bool success)
{
  if (!success)
  {
    qDebug("KomposeGlobal::slotDone() - loading of desktop background failed.\n");
    //QTimer::singleShot( 1000, this, SLOT( initSharedPixmaps() ) );
  }
}

void KomposeGlobal::enablePixmapExports()
{
#ifdef Q_WS_X11
  qDebug("KomposeGlobal::enablePixmapExports()");
  DCOPClient *client = kapp->dcopClient();
  if (!client->isAttached())
    client->attach();
  QByteArray data;
  QDataStream args( data, IO_WriteOnly );
  args << 1;

  QCString appname( "kdesktop" );
  int screen_number = DefaultScreen(qt_xdisplay());
  if ( screen_number )
    appname.sprintf("kdesktop-screen-%d", screen_number );

  client->send( appname, "KBackgroundIface", "setExport(int)", data );
#endif
}

/**
 * Show Global Shortcuts Dialog for Kompose
 */
void KomposeGlobal::showGlobalShortcutsSettingsDialog()
{
  KKeyDialog::configure( KomposeSettings::instance()->getGlobalAccel() );
  KomposeSettings::instance()->writeConfig();
}

/**
 * Show About Dialog for Kompose
 */
void KomposeGlobal::showAboutDlg()
{
  KAboutApplication* kabout = new KAboutApplication();
  kabout->show();
}

/**
 * Initialise Imlib2.
 * Should only be called by the KomposeGlobal constructor
 */
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
  qDebug("KomposeGlobal::initImlib() - Imlib2 support enabled.");
}

/**
 * Detect and initialise the XComposite extension.
 * Should only be called by the KomposeGlobal constructor
 */
void KomposeGlobal::initCompositeExt()
{
  if ( !(!xcomposite && KomposeSettings::instance()->getUseComposite()) )
    return;

  xcomposite = false;
#ifdef COMPOSITE
  // Check for XComposite
  Display *dpy = QPaintDevice::x11AppDisplay();

  int event_base, error_base;
  if ( XCompositeQueryExtension( dpy, &event_base, &error_base ) )
  {
    // If we get here the server supports the extension
    xcomposite = true;

    // XComposite version check
    int major = 1, minor = 1; // The highest version we support
    XCompositeQueryVersion( dpy, &major, &minor );
    // major and minor will now contain the version the server supports.
    if (!(major > 0 || minor >= 2))
    {
      qDebug("KomposeGlobal::initCompositeExt() - XComposite doesn't allow NamePixmap requests! - Disabling XComposite support");
      // TODO: create a namewindowpixbool to make it work with composite 0.1
      xcomposite = false;
    }

    // XRender version check
    int renderEvent, renderError;
    if (!XRenderQueryExtension (dpy, &renderEvent, &renderError))
    {
      qDebug("KomposeGlobal::initCompositeExt() - XRender not available! - Disabling XComposite support");
    }

    //XDamage version check
    //int damageEvent, damageError; // The event base is important here
    if (!XDamageQueryExtension( dpy, &damageEvent, &damageError ))
    {
      qDebug("KomposeGlobal::initCompositeExt() - XDamage is not available! - Disabling XComposite support");
      xcomposite = false;
    }

  }
  else
    xcomposite = false;
#endif

  if ( xcomposite )
    qDebug("KomposeGlobal::initCompositeExt() - XComposite extension found and enabled.");
}

#include "komposeglobal.moc"
