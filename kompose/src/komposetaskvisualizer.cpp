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

#include "komposeglobal.h"
#include "komposesettings.h"
#include "komposeviewmanager.h"
#include "komposetaskmanager.h"

#include <qpixmap.h>
#include <qtimer.h>
#include <kwin.h>
#include <netwm.h>
#include <qapplication.h>
#include <kdebug.h>

#include <time.h>
#include <math.h>

#include "komposetaskvisualizer.h"


KomposeTaskVisualizer::KomposeTaskVisualizer(KomposeTask *parent, const char *name)
    : QObject(parent, name),
    task(parent),
    scaledScreenshotDirty(false),
    screenshotSuspended(false),
    screenshotBlocked(false),
    lasteffect( IEFFECT_NONE ),
    m_glTexID(0)
{
#ifdef COMPOSITE
  validBackingPix = false;
  compositeInit = false;
#endif

  screenshot.setOptimization( QPixmap::BestOptim );
  scaledScreenshot.setOptimization( QPixmap::BestOptim );

  // Create highlight color modifier
  cmHighlight = imlib_create_color_modifier();
  imlib_context_set_color_modifier(cmHighlight);
  imlib_modify_color_modifier_brightness(0.13);

  cmMinimized = imlib_create_color_modifier();
  imlib_context_set_color_modifier(cmMinimized);
  imlib_modify_color_modifier_brightness(-0.13);
  imlib_context_set_color_modifier(0);

  //   if ( !KomposeSettings::self()->cacheScaledPixmaps() )
  //   {
  //       // clear cached pixmaps on viewclose
  //       connect( KomposeViewManager::self(), SIGNAL(viewClosed()), this, SLOT(clearCached()) );
  //   }

  initXComposite();
  connect( KomposeGlobal::self(), SIGNAL(settingsChanged()), this, SLOT(initXComposite()) );
}

KomposeTaskVisualizer::~KomposeTaskVisualizer()
{
#ifdef COMPOSITE
  if ( compositeInit )
    XDamageDestroy( dpy, damage);
#endif
  scaledScreenshot.resize(0,0);
  screenshot.resize(0,0);
}


/**
 * Called from outside to retrieve a screenshot
 * @param pix The pixmap the screenshot will be rendered onto
 */
void KomposeTaskVisualizer::renderOnPixmap(QPixmap* pix, int effect)
{
  if ( scaledScreenshotDirty || scaledScreenshot.isNull() || scaledScreenshot.size() != pix->size() ||
       KomposeSettings::self()->imageEffects() && (lasteffect != effect ) )
  {
    lasteffect = effect;
    renderScaledScreenshot( pix->size() );
  }

  copyBlt ( pix, 0, 0, &scaledScreenshot, 0, 0, pix->width(), pix->height() );

  //   QPainter p( pix );
  //   p.drawPixmap(0 ,0 , *scaledScreenshot, 0, 0, pix->width(), pix->height() );
  //   p.end();
}


/**
 * Renders a scaled version of screenshot and stores it as scaledScreenshot
 * @param newSize 
 */
void KomposeTaskVisualizer::renderScaledScreenshot( QSize newSize )
{
  kdDebug() << "KomposeTaskVisualizer::renderScaledScreenshot() (" << task->window() << ") " << newSize.width() << "x" << newSize.height() << endl;

  scaledScreenshot.resize( newSize );

  if ( KomposeGlobal::self()->hasXcomposite() && KomposeSettings::self()->composite() )
  {
#ifdef COMPOSITE
    if ( !validBackingPix )
    {
      // When we get here we have never referenced a backingpix...
      // FIXME: Currently it seems that there is no way to retrieve unmapped backing pixmaps,
      // even switching desktops won't work due to the latency of XComposite :(
      // Return a empty pixmap
      scaledScreenshot.fill(white);
      return;
    }

    // Create a Screenshot qpixmap
    screenshot.resize( task->getGeometry().size() );

    Picture picture = XRenderCreatePicture( dpy, windowBackingPix, format, CPSubwindowMode, &pa );
    XRenderComposite( dpy,
                      hasAlpha ? PictOpOver : PictOpSrc,
                      picture,
                      None,
                      screenshot.x11RenderHandle(),
                      task->getGeometry().x() - task->getFrameGeometry().x(),
                      task->getGeometry().y() - task->getFrameGeometry().y(),
                      0, 0, 0, 0,
                      screenshot.width(), screenshot.height() );
    XRenderFreePicture (dpy, picture);
#endif

  }
  /*  if ( KomposeGlobal::self()->hasXcomposite() && KomposeSettings::self()->composite() )
    {
      // The XComposite way
  #ifdef COMPOSITE
      Picture picture = XRenderCreatePicture( dpy, windowBackingPix, format, CPSubwindowMode, &pa );
      QRect geom = task->getGeometry();
   
      double scale = (double)pix->width() / (double)geom.width();
      XRenderSetPictureFilter( dpy, picture, FilterBilinear, 0, 0 );
      // Scaling matrix
      XTransform xform = {{
                            { XDoubleToFixed( 1 ), XDoubleToFixed( 0 ), XDoubleToFixed(     0 ) },
                            { XDoubleToFixed( 0 ), XDoubleToFixed( 1 ), XDoubleToFixed(     0 ) },
                            { XDoubleToFixed( 0 ), XDoubleToFixed( 0 ), XDoubleToFixed( scale ) }
                          }};
   
      XRenderSetPictureTransform( dpy, picture, &xform );
   
      XRenderComposite( QPaintDevice::x11AppDisplay(),
                        hasAlpha ? PictOpOver : PictOpSrc,
                        picture,
                        None,
                        pix->x11RenderHandle(),
                        0, 0, 0, 0,
                        0, 0, pix->width(), pix->height() );
  #endif
   
    }
    else
    {*/
  // Scale and render screenshot on scaledScreenshot
  imlib_context_set_anti_alias(1);
  imlib_context_set_drawable( screenshot.handle() );
  Imlib_Image imgOrig = imlib_create_image_from_drawable((Pixmap)0, 0, 0, screenshot.width(), screenshot.height(), 1);
  imlib_context_set_image( imgOrig );
  Imlib_Image img = imlib_create_cropped_scaled_image(0, 0, screenshot.width(), screenshot.height(), newSize.width(), newSize.height());
  imlib_free_image();
  imlib_context_set_image( img );
  applyEffect();
  imlib_context_set_drawable( scaledScreenshot.handle() );
  imlib_render_image_on_drawable_at_size(0, 0, newSize.width(), newSize.height());
  imlib_free_image();
  //   }
  scaledScreenshotDirty = false;
}

/**
 * Renders a scaled version of screenshot and stores it as scaledScreenshot
 * @param newSize 
 */
QPixmap* KomposeTaskVisualizer::getOrigPixmap()
{
  QPixmap* pm = new QPixmap( task->getGeometry().size() );

  if ( KomposeGlobal::self()->hasXcomposite() && KomposeSettings::self()->composite() )
  {
#ifdef COMPOSITE
    if ( !validBackingPix )
    {
      // When we get here we have never referenced a backingpix...
      // FIXME: Currently it seems that there is no way to retrieve unmapped backing pixmaps,
      // even switching desktops won't work due to the latency of XComposite :(
      // Return a empty pixmap
      return pm;
    }

    Picture picture = XRenderCreatePicture( dpy, windowBackingPix, format, CPSubwindowMode, &pa );
    XRenderComposite( dpy,
                      hasAlpha ? PictOpOver : PictOpSrc,
                      picture,
                      None,
                      pm->x11RenderHandle(),
                      task->getGeometry().x() - task->getFrameGeometry().x(),
                      task->getGeometry().y() - task->getFrameGeometry().y(),
                      0, 0, 0, 0,
                      pm->width(), pm->height() );
    XRenderFreePicture (dpy, picture);

    return pm;
#endif

  }
  else
  {
    *pm = screenshot;
    return pm;
  }
}

/**
 * Called whenever the Window has been activated
 */
void KomposeTaskVisualizer::slotTaskActivated()
{
  if ( KomposeGlobal::self()->hasXcomposite() && KomposeSettings::self()->composite() )
  {
    return;
  }

  if ( KomposeViewManager::self()->getBlockScreenshots() && !screenshotSuspended )
  {
    // Retry 1 sec later
    screenshotSuspended = true;
    QTimer::singleShot( 500, this, SLOT( slotTaskActivated() ) );
  }
  screenshotSuspended = false;

  // Grab a Passive Screenshot
  if ( KomposeSettings::self()->passiveScreenshots() &&
       !KomposeViewManager::self()->hasActiveView() &&
       !KomposeViewManager::self()->getBlockScreenshots() )
  {
    kdDebug() << "KomposeTaskVisualizer::slotTaskActivated() (WId " << task->window() << ") - Screenshot already exists, but passive mode on - Grabbing new one." << endl;
    // Use a timer to make task switching feel more responsive
    QTimer::singleShot( 300, this, SLOT( captureScreenshot_GrabWindow() ) );
    //captureScreenshot_GrabWindow();
  }
}


/**
 * Called whenever Kompose needs a screenshot to display (normally before a view is shown)
 */
void KomposeTaskVisualizer::slotUpdateScreenshot()
{
#ifdef COMPOSITE
  if ( KomposeGlobal::self()->hasXcomposite() && KomposeSettings::self()->composite() )
  {
    if ( !validBackingPix )
    {
      kdDebug() << "KomposeTaskVisualizer::slotUpdateScreenshot() (WId " << task->window() << ") - No backing pixmap referenced. Bad :(" << endl;
      // When we get here we have never referenced a backingpix...
      // FIXME: Currently it seems that there is no way to retrieve unmapped backing pixmaps,
      // even switching desktops won't work due to the latency of XComposite :(
    }
    return;
  }
#endif

  // If no screenshot exists grab one via activate/raise & capture
  if ( screenshot.isNull() )
  {
    bool  iconifyLater = false;

    if ( task->isIconified() == true )
    {
      kdDebug() << "KomposeTaskVisualizer::slotUpdateScreenshot() (WId " << task->window() << ") - Window iconified... we have to raise it and iconify it again later." << endl;
      iconifyLater = true;
    }

    kdDebug() << "KomposeTaskVisualizer::slotUpdateScreenshot() (WId " << task->window() << ") - Forcing activation (no screenshot exists)" << endl;

    task->activate();
    QApplication::flushX();
    QApplication::syncX();

    // Wait until window is fully redrawn
    // Thanks to desk3d (http://desk3d.sourceforge.net) for the non-blocking sleep
    struct timeval tm;
    int ms = KomposeSettings::self()->windowCaptureDelay();
    tm.tv_sec = (time_t)floor(ms / 1000);
    tm.tv_usec = (ms - (tm.tv_sec * 1000)) * 1000;
    select (0, 0, 0, 0, &tm);

    QApplication::flushX();
    //task->refresh();

    // Finally capture!
    screenshot = QPixmap::grabWindow( task->wmFrame() );
    //captureScreenshot_GrabWindow();

    // Restore if formerly iconified
    if ( iconifyLater )
      QTimer::singleShot( 1000, task, SLOT( iconify() ) );

    scaledScreenshotDirty = true;
  }
}


/**
 * This should be called whenever the window is unmapped as XComposite will reallocate
 * or the backing pixmap (on resize, minimize, virt desk change, etc)
 */
void KomposeTaskVisualizer::updateXCompositeNamedPixmap()
{
#ifdef COMPOSITE
  if ( compositeInit &&
       KomposeGlobal::self()->hasXcomposite() && KomposeSettings::self()->composite())
  {
    if( !task->isOnCurrentDesktop() )
    {
      kdDebug() << "KomposeTaskVisualizer::updateXCompositeNamedPixmap() (WId " << task->window() << ") - Not reallocationg (unmapped)" << endl;
      return;
    }

    kdDebug() << "KomposeTaskVisualizer::updateXCompositeNamedPixmap() (WId " << task->window() << ") - Reallocating backing pixmap" << endl;
    if ( validBackingPix )
      XFreePixmap(dpy, windowBackingPix);

    windowBackingPix = XCompositeNameWindowPixmap(dpy, task->wmFrame() );

    validBackingPix = true;
    scaledScreenshotDirty = true;
  }
#endif
}


/**
 * Initialise Composite backing store for this window
 */
void KomposeTaskVisualizer::initXComposite()
{
#ifdef COMPOSITE
  if ( !compositeInit && KomposeGlobal::self()->hasXcomposite() && KomposeSettings::self()->composite())
  {
    dpy = QPaintDevice::x11AppDisplay();

    connect( task, SIGNAL(x11ConfigureNotify()), this, SLOT(updateXCompositeNamedPixmap()));
    XSelectInput( dpy, task->wmFrame(), StructureNotifyMask );
    connect( task, SIGNAL( x11DamageNotify() ), SLOT( setScaledScreenshotDirty() ) );

    XWindowAttributes attr;
    XGetWindowAttributes( dpy, task->wmFrame(), &attr );
    format = XRenderFindVisualFormat( dpy, attr.visual );
    hasAlpha = ( format->type == PictTypeDirect && format->direct.alphaMask ); //FIXME: move this to komposetask
    //   int x                     = attr.x;
    //   int y                     = attr.y;
    //   int width                 = attr.width;
    //   int height                = attr.height;
    pa.subwindow_mode = IncludeInferiors; // Don't clip child widgets
    compositeInit = true;
    updateXCompositeNamedPixmap();

    kdDebug() << "KomposeTaskVisualizer::initXComposite() (WId " << task->window() << ") - Setting up Damage extension" << endl;
    // Create a damage handle for the window, and specify that we want an event whenever the
    // damage state changes from not damaged to damaged.
    damage = XDamageCreate( dpy, task->window(), XDamageReportNonEmpty );
  }
  else
  {
    disconnect( task, SIGNAL(x11ConfigureNotify()), this, SLOT(updateXCompositeNamedPixmap()));
    disconnect( task, SIGNAL( x11DamageNotify() ), this, SLOT( setScaledScreenshotDirty() ) );
    if ( compositeInit )
    {
      XDamageDestroy( dpy, damage);
      compositeInit = false;
    }
  }
#endif
}


/**
 * Grabs a screenshot the old fashioned way
 */
void KomposeTaskVisualizer::captureScreenshot_GrabWindow()
{
  if ( screenshotBlocked || ( !(task->isActive() && task->isOnTop()) ) )
  {
    kdDebug() << "KomposeTaskVisualizer::captureScreenshot_GrabWindow() (WId " << task->window() << ") - Could not grab screenshot." << endl;
    return;
  }
  //task->activate();


  // QWidget *rootWin = qApp->desktop();
  // screenshot = QPixmap::grabWindow( rootWin->winId(), geom.x(), geom.y(), geom.width(), geom.height() );

  screenshot = QPixmap::grabWindow( task->wmFrame() );
  scaledScreenshotDirty = true;

  // We've just grabbed a screenshot and don't want this to happen again in the next 3?! seconds
  screenshotBlocked = true;
  QTimer::singleShot( 3000, this, SLOT( enablePasvScreenshots() ) );

  kdDebug() << "KomposeTaskVisualizer::captureScreenshot_GrabWindow() (WId " << task->window() << ") - Grabbed screenshot." << endl;

  // Code to create a screenshot directly as an Imlib image

  //     QRect geom = windowInfo.geometry();
  //     Display *disp;
  //     Visual *vis;
  //     Colormap cm;
  //     int screen;
  //
  //     //get display information
  //     disp = XOpenDisplay(0);
  //     screen = DefaultScreen(disp);
  //     vis = DefaultVisual(disp, screen);
  //     cm = DefaultColormap(disp, screen);
  //
  //     //set imlib properties
  //     imlib_context_set_display(disp);
  //     imlib_context_set_visual(vis);
  //     imlib_context_set_colormap(cm);
  //     imlib_context_set_drawable(RootWindow(disp, screen));
  //     imlib_context_set_anti_alias(1);
  //     imlib_context_set_blend(0);
  //
  //     Imlib_Image img = imlib_create_image_from_drawable((Pixmap)0,geom.x(), geom.y(), geom.width(), geom.height(),1);
  //
  //
  //     screenshot->setImage( img );
  //
  //     XCloseDisplay(disp);

  //kdDebug() << "KomposeTaskVisualizer::captureScreenshot() - Created Screenshot: x:%d y:%d size:%dx%d", geom.x(), geom.y(), screenshot->originalWidth(), screenshot->originalHeight() );
}

void KomposeTaskVisualizer::enablePasvScreenshots()
{
  screenshotBlocked = false;
}

void KomposeTaskVisualizer::clearCached()
{
  scaledScreenshot.resize(0,0);
}


void KomposeTaskVisualizer::applyEffect()
{
  imlib_context_set_color_modifier(0);

  if ( lasteffect == IEFFECT_MINIMIZED || lasteffect == IEFFECT_MINIMIZED_AND_TITLE )
  {
    //FIXME: maybe there is a faster tint filter?!
    imlib_context_set_color_modifier(cmMinimized);
  }

  if ( lasteffect == IEFFECT_HIGHLIGHT )
  {
    //FIXME: maybe there is a faster tint filter?!
    imlib_context_set_color_modifier(cmHighlight);
  }

  if ( lasteffect == IEFFECT_TITLE || lasteffect == IEFFECT_MINIMIZED_AND_TITLE )
  {
    /* we can blend stuff now */
    imlib_context_set_blend(1);
    /* our color range */
    Imlib_Color_Range range;

    /* draw a gradient on top of things at the top left of the window */
    /* create a range */
    range = imlib_create_color_range();
    imlib_context_set_color_range(range);
    imlib_context_set_color(255, 255, 255, 0);
    imlib_add_color_to_color_range(0);
    imlib_context_set_color(255, 255, 255, 255);
    imlib_add_color_to_color_range(1000);
    /* draw the range */
    //imlib_context_set_image(myIm);
    QFontMetrics fm( KomposeSettings::self()->windowTitleFont() );
    imlib_image_fill_color_range_rectangle(0, 0, scaledScreenshot.width(), fm.ascent() * 3, -180.0);
    /* free it */
    imlib_free_color_range();
  }

}

#include "komposetaskvisualizer.moc"
