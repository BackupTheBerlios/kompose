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
    KomposeSettings *settingsInstance = new KomposeSettings();
  }
  return settingsInstance;
}


KomposeSettings::KomposeSettings(QObject *parent, const char *name)
    : QObject(parent, name)
{
  settingsInstance = this;

  // Init global shortcut object
  globalAccel = new KGlobalAccel( this );
  globalAccel->insert( "showVirtualDesktopView", i18n("Show Kompose"),
                       i18n("Displays a komposition of all Windows on your virtual Desktops."),
                       CTRL+SHIFT+Key_I, KKey::QtWIN+CTRL+SHIFT+Key_I,
                       KomposeTaskManager::instance(), SLOT(createVirtualDesktopView()) );


  // Init our Pixmap converter powered by MIT-SHM shared memory extension. great stuff, KDE people :)
  pixmapIO = new KPixmapIO();
  pixmapIO->setShmPolicy( KPixmapIO::ShmKeepAndGrow );
  pixmapIO->preAllocShm( 1024000 );


  // read Settings from cfg file
  readConfig();
}


KomposeSettings::~KomposeSettings()
{
  delete pixmapIO;
  delete settingsInstance;
}


void KomposeSettings::readConfig()
{
  // Read Shortcut Settings from config
  globalAccel->readSettings();
  globalAccel->updateConnections();

  kapp->config()->setGroup("Main window");

  passiveScreenshots = kapp->config()->readBoolEntry("passiveScreenshots", true);
  overwriteOldScreenshots = kapp->config()->readBoolEntry("overwriteOldScreenshots", true);
  screenshotGrabDelay = kapp->config()->readNumEntry("screenshotGrabDelay", 400000000);

  highlightWindows = kapp->config()->readBoolEntry("highlightWindows", false);
  tintVirtDesks = kapp->config()->readBoolEntry("tintVirtDesks", false);
  tintVirtDesksColor = kapp->config()->readColorEntry("tintVirtDesksColor", new QColor(Qt::blue) );

  emit settingsChanged();
}

void KomposeSettings::writeConfig()
{
  globalAccel->writeSettings();
  globalAccel->updateConnections();

  // Read from config file
  kapp->config()->setGroup("Main window");

  kapp->config()->writeEntry("passiveScreenshots", passiveScreenshots );
  kapp->config()->writeEntry("overwriteOldScreenshots", overwriteOldScreenshots);
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
