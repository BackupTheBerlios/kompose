//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposeviewmanager.h"

#include "komposetaskmanager.h"
#include "komposeglobal.h"
#include "komposesettings.h"

#include <qtimer.h>

#include <kwin.h>
#include <kapplication.h>


static KomposeViewManager* viewManagerInstance = 0;

/**
 * Viewmanager is a singleton
 */
KomposeViewManager* KomposeViewManager::instance()
{
  if ( !viewManagerInstance )
  {
    qDebug("KomposeViewManager::instance() - Creating Singleton instance");
    viewManagerInstance = new KomposeViewManager();
  }
  return viewManagerInstance;
}

KomposeViewManager::KomposeViewManager():
    QObject(),
    DCOPObject( "KomposeDcopIface" ),
    viewWidget(),
    activeView(0),
    blockScreenshots(0)
{
  viewManagerInstance = this;
}


KomposeViewManager::~KomposeViewManager()
{}


void KomposeViewManager::createView( int type )
{
  if (type == -1)
    type = KomposeSettings::instance()->getDefaultView();

  switch(type)
  {
  case KOMPOSEDISPLAY_VIRTUALDESKS:
    createVirtualDesktopView();
    break;
  case KOMPOSEDISPLAY_WORLD:
    createWorldView();
    break;
  }
}


void KomposeViewManager::createVirtualDesktopView()
{
  // Set activeView to false during this funcion as it will be checked by the layout
  int tmp_activeview = activeView;
  activeView = false;

  if ( !tmp_activeview  )
  {
    // Remember current desktop
    deskBeforeSnaps = KWin::currentDesktop();
    // Update screenshot of the current window to be more up2date
    //     slotUpdateScreenshot( kwinmodule->activeWindow() );
    // Update all other
    KomposeTaskManager::instance()->slotUpdateScreenshots();
  }

  qDebug("KomposeViewManager::createVirtualDesktopView - Creating View");

  if ( !tmp_activeview  )
    viewWidget = new KomposeFullscreenWidget( KOMPOSEDISPLAY_VIRTUALDESKS );
  else
    viewWidget->setType( KOMPOSEDISPLAY_VIRTUALDESKS );

  KWin::forceActiveWindow( viewWidget->winId() );

  activeView = true;
}


void KomposeViewManager::createWorldView()
{
  // Set activeView to false during this funcion as it will be checked by the layout
  int tmp_activeview = activeView;
  activeView = false;

  if ( !tmp_activeview )
  {
    // Remember current desktop
    deskBeforeSnaps = KWin::currentDesktop();
    // Update screenshot of the current window to be more up2date
    //     slotUpdateScreenshot( kwinmodule->activeWindow() );
    // Update all other
    KomposeTaskManager::instance()->slotUpdateScreenshots();
  }

  qDebug("KomposeViewManager::createWorldView - Creating View");

  if ( !tmp_activeview )
    viewWidget = new KomposeFullscreenWidget( KOMPOSEDISPLAY_WORLD );
  else
    viewWidget->setType( KOMPOSEDISPLAY_WORLD );

  KWin::forceActiveWindow( viewWidget->winId() );

  activeView = true;
}


void KomposeViewManager::closeCurrentView()
{
  if ( !activeView )
    return;
  
  blockScreenshots = true;
  activeView = false;

  viewWidget->setUpdatesEnabled( false );
  viewWidget->close(true);
  viewWidget = 0;

  emit viewClosed();

  if ( KomposeGlobal::instance()->getSingleShot() )
    kapp->quit();

  // Reset old Desktop
  KWin::setCurrentDesktop( deskBeforeSnaps );
  
  // A short delay until we allow screenshots again (would cause overlapping else
  QTimer::singleShot( 1500, this, SLOT( toggleBlockScreenshots() ) );
}

void KomposeViewManager::toggleBlockScreenshots()
{
  blockScreenshots = !blockScreenshots;
}

void KomposeViewManager::setCurrentDesktop( int desknum )
{
  closeCurrentView();
  KWin::setCurrentDesktop(desknum+1);
}

void KomposeViewManager::activateTask( KomposeTask *task )
{
  closeCurrentView();
  task->activate();
}

void KomposeViewManager::createDefaultView()
{
  createView();
}


#include "komposeviewmanager.moc"