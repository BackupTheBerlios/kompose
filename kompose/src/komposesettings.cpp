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
    qDebug("KomposeSettings::instance() - Creating Singleton instance");
    settingsInstance = new KomposeSettings();
  }
  return settingsInstance;
}


KomposeSettings::KomposeSettings(QObject *parent, const char *name)
    : QObject(parent, name)
{
  
  // Init global shortcut object
  globalAccel = new KGlobalAccel( this );
  globalAccel->insert( "showVirtualDesktopView", i18n("Show Kompose (Virtual Desktops)"),
                       i18n("Displays all Windows sorted by virtual Desktops."),
                       CTRL+SHIFT+Key_I, KKey::QtWIN+CTRL+SHIFT+Key_I,
                       KomposeTaskManager::instance(), SLOT(createVirtualDesktopView()) );
  globalAccel->insert( "showAllTasksView", i18n("Show Kompose (All Tasks)"),
                       i18n("Displays all Windows unsorted."),
                       CTRL+SHIFT+Key_J, KKey::QtWIN+CTRL+SHIFT+Key_J,
                       KomposeTaskManager::instance(), SLOT(createWorldView()) );

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
  onlyOneScreenshot = kapp->config()->readBoolEntry("onlyOneScreenshot", false);
  screenshotGrabDelay = kapp->config()->readNumEntry("screenshotGrabDelay", 400000000);

  highlightWindows = kapp->config()->readBoolEntry("highlightWindows", false);
  tintVirtDesks = kapp->config()->readBoolEntry("tintVirtDesks", false);
  tintVirtDesksColor = kapp->config()->readColorEntry("tintVirtDesksColor", new QColor(Qt::blue) );

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
  kapp->config()->writeEntry("onlyOneScreenshot", onlyOneScreenshot);
  kapp->config()->writeEntry("screenshotGrabDelay", screenshotGrabDelay);

  kapp->config()->writeEntry("highlightWindows", highlightWindows);
  kapp->config()->writeEntry("tintVirtDesks", tintVirtDesks);
  kapp->config()->writeEntry("tintVirtDesksColor", tintVirtDesksColor);
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


#include "komposesettings.moc"
