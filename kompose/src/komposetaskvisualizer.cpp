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

#include "komposetaskvisualizer.h"

KomposeTaskVisualizer::KomposeTaskVisualizer(KomposeTask *parent, const char *name)
    : QObject(parent, name),
    task(parent),
    windowBackingPix(0),
    validBackingPix(false),
    scaledScreenshotDirty(false)
{
  screenshot = new QPixmap();
  scaledScreenshot = new QPixmap();
  screenshot->setOptimization( QPixmap::BestOptim );
  scaledScreenshot->setOptimization( QPixmap::BestOptim );

  connect( KomposeViewManager::instance(), SIGNAL(viewClosed()), this, SLOT(clearCached()) );

  if ( KomposeGlobal::instance()->hasXcomposite() )
  {
    initXComposite();
    connect( task, SIGNAL(x11ConfigureNotify()), this, SLOT(updateXCompositeNamedPixmap()));
    XSelectInput( dpy, task->wmFrame(), StructureNotifyMask );
  }

}

KomposeTaskVisualizer::~KomposeTaskVisualizer()
{}


/**
 * Called from outside to retrieve a screenshot
 * @param pix The pixmap the screenshot will be rendered onto
 */
void KomposeTaskVisualizer::renderOnPixmap(QPixmap* pix)
{
  if ( scaledScreenshotDirty || scaledScreenshot->isNull() || scaledScreenshot->size() != pix->size() )
    renderScaledScreenshot( pix->size() );
  copyBlt ( pix, 0, 0, scaledScreenshot, 0, 0, pix->width(), pix->height() );

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
  qDebug("KomposeTaskVisualizer::renderScaledScreenshot() (%d)", task->window());

  scaledScreenshot->resize( newSize );

  if ( KomposeGlobal::instance()->hasXcomposite() )
  {
    if ( !validBackingPix )
    {
      // When we get here we have never referenced a backingpix...
      // FIXME: Currently it seems that there is no way to retrieve unmapped backing pixmaps,
      // even switching desktops won't work due to the latency of XComposite :(
      // Return a empty pixmap
      scaledScreenshot->fill(white);
      return;
    }

    // Create a Screenshot qpixmap
    screenshot->resize( task->getGeometry().size() );

#ifdef COMPOSITE
    Picture picture = XRenderCreatePicture( dpy, windowBackingPix, format, CPSubwindowMode, &pa );
    XRenderComposite( dpy,
                      hasAlpha ? PictOpOver : PictOpSrc,
                      picture,
                      None,
                      screenshot->x11RenderHandle(),
                      0, 0, 0, 0,
                      0, 0, screenshot->width(), screenshot->height() );
    XRenderFreePicture (dpy, picture);
#endif

  }
  /*  if ( KomposeGlobal::instance()->hasXcomposite() )
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
  imlib_context_set_drawable( screenshot->handle() );
  Imlib_Image imgOrig = imlib_create_image_from_drawable((Pixmap)0, 0, 0, screenshot->width(), screenshot->height(), 1);
  imlib_context_set_image( imgOrig );
  Imlib_Image img = imlib_create_cropped_scaled_image(0, 0, screenshot->width(), screenshot->height(), newSize.width(), newSize.height());
  imlib_free_image();
  imlib_context_set_image( img );
  imlib_context_set_drawable( scaledScreenshot->handle() );
  imlib_render_image_on_drawable_at_size(0, 0, newSize.width(), newSize.height());
  imlib_free_image();
  //   }
  scaledScreenshotDirty = false;
}



/**
 * Called whenever the Window has been activated
 */
void KomposeTaskVisualizer::slotTaskActivated()
{
  if ( KomposeGlobal::instance()->hasXcomposite() )
  {
    return;
  }

  // Grab a Passive Screenshot
  if ( KomposeSettings::instance()->getPassiveScreenshots() &&
       !KomposeViewManager::instance()->hasActiveView() )
  {
    qDebug("KomposeTaskVisualizer::slotTaskActivated() (WId %d) - Screenshot already exists, but passive mode on - Grabbing new one.", task->window());
    // Use a timer to make task switching feel more responsive
    //QTimer::singleShot( 20, this, SLOT( captureScreenshot() ) );
    captureScreenshot_GrabWindow();
  }
}


/**
 * Called whenever Kompose needs a screenshot to display (normally before a view is shown)
 */
void KomposeTaskVisualizer::slotUpdateScreenshot()
{
#ifdef COMPOSITE
  if ( KomposeGlobal::instance()->hasXcomposite() )
  {
    if ( !validBackingPix )
    {
      qDebug("KomposeTaskVisualizer::slotUpdateScreenshot() (WId %d) - No backing pixmap referenced. Bad :(", task->window() );
      // When we get here we have never referenced a backingpix...
      // FIXME: Currently it seems that there is no way to retrieve unmapped backing pixmaps,
      // even switching desktops won't work due to the latency of XComposite :(
    }
    return;
  }
#endif

  // If no screenshot exists grab one via activate/raise & capture
  if ( screenshot->isNull() )
  {
    bool  iconifyLater = false;

    if ( task->isIconified() == true )
    {
      qDebug("KomposeTaskVisualizer::slotUpdateScreenshot() (WId %d) - Window iconified... we have to raise it and iconify it again later.", task->window());
      iconifyLater = true;
    }

    qDebug("KomposeTaskVisualizer::slotUpdateScreenshot() (WId %d) - Forcing activation (no screenshot exists)", task->window());
    task->activateOrRaise();

    // Wait until window is fully redrawn
    struct timespec req, rem;
    req.tv_sec = 0;
    req.tv_nsec = KomposeSettings::instance()->getScreenshotGrabDelay();
    while(nanosleep(&req, &rem))
      req = rem;

    // Finally capture!
    captureScreenshot_GrabWindow();

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
  if( !task->isOnCurrentDesktop() )
  {
    qDebug("KomposeTaskVisualizer::updateXCompositeNamedPixmap() (WId %d) - Not reallocationg (unmapped)", task->window());
    return;
  }

  qDebug("KomposeTaskVisualizer::updateXCompositeNamedPixmap() (WId %d) - Reallocating backing pixmap", task->window());
  if ( validBackingPix )
    XFreePixmap(dpy, windowBackingPix);

  windowBackingPix = XCompositeNameWindowPixmap(dpy, task->wmFrame() );

  validBackingPix = true;
  scaledScreenshotDirty = true;
#endif
}


/**
 * Initialise Composite backing store for this window
 */
void KomposeTaskVisualizer::initXComposite()
{
#ifdef COMPOSITE
  dpy = QPaintDevice::x11AppDisplay();

  XWindowAttributes attr;
  XGetWindowAttributes( dpy, task->wmFrame(), &attr );
  format = XRenderFindVisualFormat( dpy, attr.visual );
  hasAlpha = ( format->type == PictTypeDirect && format->direct.alphaMask ); //FIXME: move this to komposetask
  //   int x                     = attr.x;
  //   int y                     = attr.y;
  //   int width                 = attr.width;
  //   int height                = attr.height;
  pa.subwindow_mode = IncludeInferiors; // Don't clip child widgets
  updateXCompositeNamedPixmap();

  //FIXME: make this usable
  qDebug("KomposeTaskVisualizer::initXComposite() (WId %d) - Setting up Damage extension", task->window());
  // Create a damage handle for the window, and specify that we want an event whenever the
  // damage state changes from not damaged to damaged.
  Damage damage = XDamageCreate( dpy, task->window(), XDamageReportNonEmpty );
#endif
}


/**
 * Grabs a screenshot the old fashioned way
 */
void KomposeTaskVisualizer::captureScreenshot_GrabWindow()
{
  // QWidget *rootWin = qApp->desktop();
  // screenshot = QPixmap::grabWindow( rootWin->winId(), geom.x(), geom.y(), geom.width(), geom.height() );
  *screenshot = QPixmap::grabWindow( task->window() );

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

  //qDebug("KomposeTaskVisualizer::captureScreenshot() - Created Screenshot: x:%d y:%d size:%dx%d", geom.x(), geom.y(), screenshot->originalWidth(), screenshot->originalHeight() );
}

void KomposeTaskVisualizer::clearCached()
{
  scaledScreenshot->resize(0,0);
}


#include "komposetaskvisualizer.moc"
