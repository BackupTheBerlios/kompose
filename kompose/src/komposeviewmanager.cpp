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
#include "komposesystray.h"

#include "komposefullscreenwidget.h"
#include "komposeglviewwidget.h"

#include <qtimer.h>
#include <qcursor.h>
#include <qapplication.h>
#include <qptrlist.h>
#include <qwidgetlist.h>
#include <qdesktopwidget.h>

#include <kwin.h>
#include <kapplication.h>
#include <kdebug.h>


static KomposeViewManager* viewManagerInstance = 0;

/**
 * Viewmanager is a singleton
 */
KomposeViewManager* KomposeViewManager::self()
{
  if ( !viewManagerInstance )
  {
    kdDebug() << "KomposeViewManager::self() - Creating Singleton instance" << endl;
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

  // dirty hack. see uglyQtHackInitFunction()
  QTimer::singleShot( 500, this, SLOT( uglyQtHackInitFunction() ) );
}


KomposeViewManager::~KomposeViewManager()
{
  delete cursorUpdateTimer;
}


/**
 * This is a hack that should be called by a timer as this connect won't work in the constructor
 */
void KomposeViewManager::uglyQtHackInitFunction()
{
  connect( KomposeGlobal::self(), SIGNAL( settingsChanged() ), SLOT( slotStartCursorUpdateTimer() ) );
}

/**
 * Starts the corner check timer which polls QCursor::pos() every second
 @see checkCursorPos()
 */
void KomposeViewManager::slotStartCursorUpdateTimer()
{
  disconnect( cursorUpdateTimer, SIGNAL( timeout() ), this, SLOT( checkCursorPos() ) );

  if ( KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnBottomLeftCorner ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnBottomRightCorner ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnTopLeftCorner ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnTopRightCorner ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnTopEdge ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnBottomEdge ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnLeftEdge ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnRightEdge ) )
  {
    kdDebug() << "KomposeViewManager::slotStartCursorUpdateTimer() - QCursor::pos() checks enabled" << endl;
    QRect deskRect = QApplication::desktop()->screenGeometry();

    topLeftCorner = deskRect.topLeft();
    topRightCorner = deskRect.topRight();
    bottomLeftCorner = deskRect.bottomLeft();
    bottomRightCorner = deskRect.bottomRight();

    connect( cursorUpdateTimer, SIGNAL( timeout() ), SLOT( checkCursorPos() ) );
    cursorUpdateTimer->start( 200, false ); // TODO: Find out why even an interval of 1ms doesn't hit performance!
  }
}


/**
 * Checks if cursor hovered over a corner that activates Kompose
 */
void KomposeViewManager::checkCursorPos()
{
  if ( KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnBottomLeftCorner ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnBottomRightCorner ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnTopLeftCorner ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnTopRightCorner ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnTopEdge ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnBottomEdge ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnLeftEdge ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnRightEdge ) )
  {
    //cursorUpdateTimer->stop();
    QTimer::singleShot( KomposeSettings::self()->autoLockDelay(), this, SLOT( reCheckCursorPos() ) );
  }
}

/**
 * Called after the cursor was in a hotspot to check if it's still there
 */
void KomposeViewManager::reCheckCursorPos()
{
  if ( KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnBottomLeftCorner ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnBottomRightCorner ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnTopLeftCorner ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnTopRightCorner ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnTopEdge ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnBottomEdge ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnLeftEdge ) ||
       KomposeSettings::self()->view( KomposeSettings::EnumViewMode::ActivateOnRightEdge ) )
  {
    cursorUpdateTimer->stop();
    createView();
  }
}


void KomposeViewManager::createView( int type )
{
  kdDebug() << k_funcinfo << endl;
  if ( KomposeGlobal::self()->hasDialogOpen() )
  {
    kdDebug() << "KomposeViewManager::createView() - Another Kompose Dialog is open... close it first" << endl;
    return;
  }

  if (type == -1)
    type = KomposeSettings::self()->view( KomposeSettings::EnumViewMode::Default );

  kdDebug() << "KomposeViewManager::createView( type " << type << " )" << endl;

  if ( !activeView )
  {
    // Remember current desktop
    deskBeforeSnaps = KWin::currentDesktop();
    kdDebug() << "KomposeViewManager::createView() - Remembering desktop " << deskBeforeSnaps << endl;
    // Update screenshot of the current window to be more up2date
    // KomposeTaskManager::self()->simulatePasvScreenshotEvent();
    // Update all other
    blockScreenshots = true;
    if ( type == KomposeSettings::EnumView::CurrentDesktop )
      KomposeTaskManager::self()->slotUpdateScreenshots( false );
    else
      KomposeTaskManager::self()->slotUpdateScreenshots();
    blockScreenshots = false;
  }

  if ( !activeView )
    if ( KomposeSettings::self()->openGL() )
      viewWidget = new KomposeGLViewWidget( type );
    else
      viewWidget = new KomposeFullscreenWidget( type );
  else
    viewWidget->setType( type );

  viewWidget->show();
  KWin::forceActiveWindow( viewWidget->winId() );

  activeView = true;
}


void KomposeViewManager::closeCurrentView()
{
  if ( !activeView )
    return;

  blockScreenshots = true;
  activeView = false;

  //viewWidget->setUpdatesEnabled( false );
  //viewWidget->hide();
  viewWidget->close();
  viewWidget->deleteLater();
  //viewWidget = 0;

  emit viewClosed();

  if ( KomposeGlobal::self()->getSingleShot() )
    kapp->quit();

  // Reset old Desktop
  KWin::setCurrentDesktop( deskBeforeSnaps );

  // A short delay until we allow screenshots again (would cause overlapping else
  QTimer::singleShot( 400, this, SLOT( toggleBlockScreenshots() ) );

  // Restart Timer for corner checks
  slotStartCursorUpdateTimer();
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



#include "komposeviewmanager.moc"
