//
// C++ Implementation: komposesettings
//
// Description:
//
//
// Author: Hans Oischinger <oisch@users.berlios.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposesettings.h"

#include "komposeviewmanager.h"
#include "komposetaskmanager.h"
#include "komposepreferences.h"

#include <qcolor.h>

#include <kglobalaccel.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kiconloader.h>

static KomposeSettings* settingsInstance = 0;


/*
 * KomposeSettings is a singleton
 */
KomposeSettings* KomposeSettings::instance()
{
  if ( !settingsInstance )
  {
    // qDebug("KomposeSettings::instance() - Creating Singleton instance"); // FIXME: How can this apper multiple times??
    settingsInstance = new KomposeSettings();
  }
  return settingsInstance;
}


KomposeSettings::KomposeSettings(QObject *parent, const char *name)
    : QObject(parent, name),
    windowTitleFontMetrics(0)
{
  
  // Init global shortcut object
  globalAccel = new KGlobalAccel( this );
  globalAccel->insert( "showDefaultView", i18n("Show Komposé (default view)"),
                       i18n("Displays the view you have configured as default"),
                       KKey::QtWIN+Key_Tab, KKey::QtWIN+CTRL+SHIFT+Key_Tab,
                       KomposeViewManager::instance(), SLOT(createDefaultView()) );
  globalAccel->insert( "showWorldView", i18n("Show Komposé (ungrouped)"),
                       i18n("Displays all windows unsorted"),
                       CTRL+SHIFT+Key_J, KKey::QtWIN+CTRL+SHIFT+Key_J,
                       KomposeViewManager::instance(), SLOT(createWorldView()) );
  globalAccel->insert( "showVirtualDesktopView", i18n("Show Komposé (grouped by virtual desktops)"),
                       i18n("Displays all windows sorted by virtual desktops"),
                       CTRL+SHIFT+Key_I, KKey::QtWIN+CTRL+SHIFT+Key_I,
                       KomposeViewManager::instance(), SLOT(createVirtualDesktopView()) );
  globalAccel->insert( "showCurrentDesktopView", i18n("Show Komposé (current virtual desktop)"),
                       i18n("Displays all windows on the current desktop"),
                       CTRL+SHIFT+Key_K, KKey::QtWIN+CTRL+SHIFT+Key_K,
                       KomposeViewManager::instance(), SLOT(createCurrentDesktopView()) );

  // read Settings from cfg file
  readConfig();
  
  settingsInstance = this;
}


KomposeSettings::~KomposeSettings()
{
  delete globalAccel;
  delete settingsInstance;
}


void KomposeSettings::readConfig()
{
  qDebug("KomposeSettings::readConfig()");
  // Read Shortcut Settings from config
  globalAccel->readSettings();
  globalAccel->updateConnections();

  kapp->config()->setGroup("Main window");

  defaultView = kapp->config()->readNumEntry("defaultView", KOMPOSEDISPLAY_VIRTUALDESKS);
  
  useComposite = kapp->config()->readBoolEntry("useComposite", false);
  
  passiveScreenshots = kapp->config()->readBoolEntry("passiveScreenshots", true);
  screenshotGrabDelay = kapp->config()->readNumEntry("screenshotGrabDelay", 400000000);

  dynamicVirtDeskLayout = kapp->config()->readBoolEntry("dynamicVirtDeskLayout", true);
  
  imageEffects = kapp->config()->readBoolEntry("imageEffects", true);
  tintVirtDesks = kapp->config()->readBoolEntry("tintVirtDesks", false);
  tintVirtDesksColor = kapp->config()->readColorEntry("tintVirtDesksColor", new QColor(Qt::blue) );

  showIcons = kapp->config()->readBoolEntry("showIcons", true);
  iconSize = kapp->config()->readNumEntry("iconSize", 3);
  
  showWindowTitles = kapp->config()->readBoolEntry("showWindowTitles", true );
  windowTitleFont = kapp->config()->readFontEntry("windowTitleFont", new QFont( "arial", 11, QFont::Bold ) );
  windowTitleFontColor = kapp->config()->readColorEntry("windowTitleFontColor", new QColor(Qt::black) );
  showWindowTitleShadow = kapp->config()->readBoolEntry("showWindowTitleShadow", true );
  windowTitleFontShadowColor = kapp->config()->readColorEntry("windowTitleFontShadowColor", new QColor(Qt::lightGray) );

  desktopTitleFont = kapp->config()->readFontEntry("desktopTitleFont", new QFont( "arial", 10 ) );
  desktopTitleFontColor = kapp->config()->readColorEntry("desktopTitleFontColor", new QColor(Qt::gray) );
  desktopTitleFontHighlightColor = kapp->config()->readColorEntry("desktopTitleFontHighlightColor", new QColor(Qt::black) );

  cacheScaledPixmaps = kapp->config()->readBoolEntry("cacheScaledPixmaps", true);
    
  activateOnTopLeftCorner = kapp->config()->readBoolEntry("activateOnTopLeftCorner", false );
  activateOnTopRightCorner = kapp->config()->readBoolEntry("activateOnTopRightCorner", false );
  activateOnBottomLeftCorner = kapp->config()->readBoolEntry("activateOnBottomLeftCorner", false );
  activateOnBottomRightCorner = kapp->config()->readBoolEntry("activateOnBottomRightCorner", false );
  autoLockDelay = kapp->config()->readNumEntry("autoLockDelay", 1000);
  
  calcFontMetrics();
  emit settingsChanged();
}

void KomposeSettings::writeConfig()
{
  qDebug("KomposeSettings::writeConfig()");
  
  globalAccel->writeSettings();
  globalAccel->updateConnections();

  // Read from config file
  kapp->config()->setGroup("Main window");

  kapp->config()->writeEntry("defaultView", defaultView );
  
  kapp->config()->writeEntry("useComposite", useComposite );
  
  kapp->config()->writeEntry("passiveScreenshots", passiveScreenshots );
  kapp->config()->writeEntry("screenshotGrabDelay", screenshotGrabDelay);

  kapp->config()->writeEntry("dynamicVirtDeskLayout", dynamicVirtDeskLayout);
  
  kapp->config()->writeEntry("imageEffects", imageEffects);
  kapp->config()->writeEntry("tintVirtDesks", tintVirtDesks);
  kapp->config()->writeEntry("tintVirtDesksColor", tintVirtDesksColor);
  
  kapp->config()->writeEntry("showIcons", showIcons);
  kapp->config()->writeEntry("iconSize", iconSize);
  
  kapp->config()->writeEntry("showWindowTitles", showWindowTitles);
  kapp->config()->writeEntry("windowTitleFont", windowTitleFont);
  kapp->config()->writeEntry("windowTitleFontColor", windowTitleFontColor);
  kapp->config()->writeEntry("showWindowTitleShadow", showWindowTitleShadow);
  kapp->config()->writeEntry("windowTitleFontShadowColor", windowTitleFontShadowColor);
  
  kapp->config()->writeEntry("desktopTitleFont", desktopTitleFont);
  kapp->config()->writeEntry("desktopTitleFontColor", desktopTitleFontColor);
  kapp->config()->writeEntry("desktopTitleFontHighlightColor", desktopTitleFontHighlightColor);
  
  kapp->config()->writeEntry("cacheScaledPixmaps", cacheScaledPixmaps);
  
  kapp->config()->writeEntry("activateOnTopLeftCorner", activateOnTopLeftCorner);
  kapp->config()->writeEntry("activateOnTopRightCorner", activateOnTopRightCorner);
  kapp->config()->writeEntry("activateOnBottomLeftCorner", activateOnBottomLeftCorner);
  kapp->config()->writeEntry("activateOnBottomRightCorner", activateOnBottomRightCorner);
  kapp->config()->writeEntry("autoLockDelay", autoLockDelay);
  
  kapp->config()->sync();

  qDebug("KomposeSettings::writeConfig() - Settings saved to cfg file");

  calcFontMetrics();
  emit settingsChanged();
}


void KomposeSettings::showPreferencesDlg()
{
  // popup preference dialog
  KomposePreferences *dlg = new KomposePreferences();

  dlg->exec();

  delete dlg;
}

int KomposeSettings::getIconSizePixels()
{
  switch( iconSize )
  {
    case 0:
      return 16;
    case 1:
      return 32;
    case 2:
      return 64;
    case 3:
      return -1;
  }
  return 32;
}

void KomposeSettings::calcFontMetrics()
{
  if (windowTitleFontMetrics != 0)
    delete windowTitleFontMetrics;
  windowTitleFontMetrics = new QFontMetrics( windowTitleFont );
  windowTitleFontAscent = windowTitleFontMetrics->ascent();
  windowTitleFontHeight = windowTitleFontMetrics->height();
}

#include "komposesettings.moc"
