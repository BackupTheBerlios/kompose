//
// C++ Implementation: komposesettings
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposesettings.h"

#include "komposetaskmanager.h"
#include "komposepreferences.h"

#include <qcolor.h>

#include <kglobalaccel.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kpixmapio.h>

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
    : QObject(parent, name)
{
  
  // Init global shortcut object
  globalAccel = new KGlobalAccel( this );
  globalAccel->insert( "showAllTasksView", i18n("Show Kompose (ungrouped)"),
                       i18n("Displays all Windows unsorted."),
                       CTRL+SHIFT+Key_J, KKey::QtWIN+CTRL+SHIFT+Key_J,
                       KomposeTaskManager::instance(), SLOT(createWorldView()) );
  globalAccel->insert( "showVirtualDesktopView", i18n("Show Kompose (grouped by Virtual Desktops)"),
                       i18n("Displays all Windows sorted by Virtual Desktops."),
                       CTRL+SHIFT+Key_I, KKey::QtWIN+CTRL+SHIFT+Key_I,
                       KomposeTaskManager::instance(), SLOT(createVirtualDesktopView()) );

  // read Settings from cfg file
  readConfig();
  
  // Init our Pixmap converter powered by MIT-SHM shared memory extension. great stuff, KDE people :)
  pixmapIO = new KPixmapIO();
  pixmapIO->setShmPolicy( KPixmapIO::ShmKeepAndGrow );
  pixmapIO->preAllocShm( 1024000 );
  
  settingsInstance = this;
}


KomposeSettings::~KomposeSettings()
{
  delete pixmapIO;
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
  
  passiveScreenshots = kapp->config()->readBoolEntry("passiveScreenshots", true);
//   onlyOneScreenshot = kapp->config()->readBoolEntry("onlyOneScreenshot", false);
  screenshotGrabDelay = kapp->config()->readNumEntry("screenshotGrabDelay", 400000000);

  imageEffects = kapp->config()->readBoolEntry("imageEffects", true);
  tintVirtDesks = kapp->config()->readBoolEntry("tintVirtDesks", false);
  tintVirtDesksColor = kapp->config()->readColorEntry("tintVirtDesksColor", new QColor(Qt::blue) );

  showIcons = kapp->config()->readBoolEntry("showIcons", true);
  iconSize = kapp->config()->readNumEntry("iconSize", 3);
  
  showWindowTitles = kapp->config()->readBoolEntry("showWindowTitles", true );
  windowTitleFont = kapp->config()->readFontEntry("windowTitleFont", new QFont( "arial", 11, QFont::Bold ) );
  windowTitleFontMetrics = new QFontMetrics( windowTitleFont );
  windowTitleFontAscent = windowTitleFontMetrics->ascent();
  windowTitleFontHeight = windowTitleFontMetrics->height();
  windowTitleFontColor = kapp->config()->readColorEntry("windowTitleFontColor", new QColor(Qt::black) );
  showWindowTitleShadow = kapp->config()->readBoolEntry("showWindowTitleShadow", true );
  windowTitleFontShadowColor = kapp->config()->readColorEntry("windowTitleFontShadowColor", new QColor(Qt::lightGray) );
  
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
  
  kapp->config()->writeEntry("passiveScreenshots", passiveScreenshots );
//   kapp->config()->writeEntry("onlyOneScreenshot", onlyOneScreenshot);
  kapp->config()->writeEntry("screenshotGrabDelay", screenshotGrabDelay);

  kapp->config()->writeEntry("imageEffects", imageEffects);
  kapp->config()->writeEntry("tintVirtDesks", tintVirtDesks);
  kapp->config()->writeEntry("tintVirtDesksColor", tintVirtDesksColor);
  
  kapp->config()->writeEntry("showIcons", showIcons);
  kapp->config()->writeEntry("iconSize", iconSize);
  
  kapp->config()->writeEntry("showWindowTitles", showWindowTitles);
  kapp->config()->writeEntry("windowTitleFont", windowTitleFont);
  windowTitleFontMetrics = new QFontMetrics( windowTitleFont );
  kapp->config()->writeEntry("windowTitleFontColor", windowTitleFontColor);
  kapp->config()->writeEntry("showWindowTitleShadow", showWindowTitleShadow);
  kapp->config()->writeEntry("windowTitleFontShadowColor", windowTitleFontShadowColor);
  
  kapp->config()->sync();

  qDebug("KomposeSettings::writeConfig() - Settings saved to cfg file");

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
}

#include "komposesettings.moc"
