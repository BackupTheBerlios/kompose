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
#include <qcursor.h>
#include <qdesktopwidget.h>

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
    DCOPObject( "KomposeDcopIface" ),
    QObject(),
    viewWidget(),
    activeView(0),
    blockScreenshots(0)
{
  viewManagerInstance = this;

  // Setup cursorupdate timer to check for mouse moves into corner
  cursorUpdateTimer = new QTimer();
  slotStartCursorUpdateTimer();

  // dirtyy hack. see sickNothingWorksAndIamDrunkAnywayInitFunction()
  QTimer::singleShot( 1000, this, SLOT( sickNothingWorksAndIamDrunkAnywayInitFunction() ) );
}


KomposeViewManager::~KomposeViewManager()
{
  delete cursorUpdateTimer;
}


/**
 * This is a hack that should be called by a timer as this connect won't work in the constructor
 */
void KomposeViewManager::sickNothingWorksAndIamDrunkAnywayInitFunction()
{
  connect( KomposeSettings::instance(), SIGNAL( settingsChanged() ), SLOT( slotStartCursorUpdateTimer() ) );
}
 
/**
 * Starts the corner check timer which polls QCursor::pos() every second
 @see checkCursorPos()
 */
void KomposeViewManager::slotStartCursorUpdateTimer()
{
  disconnect( cursorUpdateTimer, SIGNAL( timeout() ), this, SLOT( checkCursorPos() ) );

  if ( KomposeSettings::instance()->getActivateOnBottomLeftCorner() ||
       KomposeSettings::instance()->getActivateOnBottomRightCorner() ||
       KomposeSettings::instance()->getActivateOnTopLeftCorner() ||
       KomposeSettings::instance()->getActivateOnTopRightCorner() )
  {
    qDebug("KomposeViewManager::slotStartCursorUpdateTimer() - QCursor::pos() checks enabled");
    QDesktopWidget *deskwidget = new QDesktopWidget();
    QRect deskRect = deskwidget->screenGeometry();
    delete deskwidget;

    topLeftCorner = deskRect.topLeft();
    topRightCorner = deskRect.topRight();
    bottomLeftCorner = deskRect.topLeft();
    bottomRightCorner = deskRect.bottomRight();

    connect( cursorUpdateTimer, SIGNAL( timeout() ), SLOT( checkCursorPos() ) );
    cursorUpdateTimer->start( 1000, false );
  }
}


/**
 * Checks if cursor hovered over a corner that activates Kompose
 */
void KomposeViewManager::checkCursorPos()
{
  if (
    ( KomposeSettings::instance()->getActivateOnTopLeftCorner() &&
      !activeView && QCursor::pos() == topLeftCorner ) ||
    ( KomposeSettings::instance()->getActivateOnTopRightCorner() &&
      !activeView && QCursor::pos() == topRightCorner ) ||
    ( KomposeSettings::instance()->getActivateOnBottomLeftCorner() &&
      !activeView && QCursor::pos() == bottomLeftCorner ) ||
    ( KomposeSettings::instance()->getActivateOnBottomRightCorner() &&
      !activeView && QCursor::pos() == bottomRightCorner )
  )
    createView();
}


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
    // FIXME: make this configurable for faster Komposé show?
    //KomposeTaskManager::instance()->simulatePasvScreenshotEvent();
    // Update all other
    blockScreenshots = true;
    KomposeTaskManager::instance()->slotUpdateScreenshots();
    blockScreenshots = false;
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
    //KomposeTaskManager::instance()->simulatePasvScreenshotEvent();
    // Update all other
    blockScreenshots = true;
    KomposeTaskManager::instance()->slotUpdateScreenshots();
    blockScreenshots = false;
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
  QTimer::singleShot( 500, this, SLOT( toggleBlockScreenshots() ) );
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
