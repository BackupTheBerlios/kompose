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

static KomposeGlobal* globalInstance = 0;


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
 : QObject(parent, name)
{
  globalInstance = this;
  
  // Initialise the Singleton instances
  KomposeSettings::instance();
  KomposeTaskManager::instance();

  // Create systray
  systray = new KomposeSysTray();
  kapp->setMainWidget( systray );
  systray->setPixmap( systray->loadIcon( "background" ) );
  systray->show();
}


KomposeGlobal::~KomposeGlobal()
{
}


#include "komposeglobal.moc"
