//
// C++ Implementation: komposeimage
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposeimage.h"
#include "komposesettings.h"
#include <kdebug.h>



KomposeImage::KomposeImage( Imlib_Image& im )
    : QObject( 0L, 0L ),
    bhasImage(true),
    bhasSourcePixmap(false),
    lasteffect( IEFFECT_NONE ),
    myWidth(-1),
    myHeight(-1)
{
  myIm       = im;
  imlib_context_set_image( myIm );
  myWidth    = imlib_image_get_width();
  myHeight   = imlib_image_get_height();
  myIsDirty  = true;

  init();
}


KomposeImage::KomposeImage()
    : QObject( 0L, 0L ),
    bhasImage(false),
    bhasSourcePixmap(false),
    lasteffect( IEFFECT_NONE ),
    myWidth(-1),
    myHeight(-1)
{
  myIsDirty  = false;
  init();
}

KomposeImage::~KomposeImage()
{
  clearCached();
}

/*
 * Call this function when the Imlib image and the qpixmap that is drawn is not needed anymore
 * (normally when Komposé view is closed)
 */
void KomposeImage::clearCached()
{
  if ( bhasSourcePixmap && bhasImage)
  {
    // Delete image as it takes a lot of ram
    bhasImage = false;
    imlib_context_set_image( myIm );
    imlib_free_image();
  }
  
  if ( !KomposeSettings::instance()->getCacheScaledPixmaps() )
  {
    myQPixmap.resize(0,0);
    myIsDirty = true;
  }
}

void KomposeImage::init()
{
  myOrigWidth  = myWidth;
  myOrigHeight = myHeight;

  myQPixmap.setOptimization( QPixmap::BestOptim );

  // Create highlight color modifier
  cmHighlight = imlib_create_color_modifier();
  imlib_context_set_color_modifier(cmHighlight);
  imlib_modify_color_modifier_brightness(0.13);

  cmMinimized = imlib_create_color_modifier();
  imlib_context_set_color_modifier(cmMinimized);
  imlib_modify_color_modifier_brightness(-0.13);
  imlib_context_set_color_modifier(0);
}

void KomposeImage::setImage( Imlib_Image &im )
{
  if (bhasImage)
  {
    imlib_context_set_image( myIm );
    imlib_free_image();
  }

  myIm = im;
  if ( !bhasSourcePixmap )  // SourcePixmap overrides Image
  {
    imlib_context_set_image( myIm );
    myOrigWidth    = imlib_image_get_width();
    myOrigHeight   = imlib_image_get_height();
    qDebug("KomposeTask::setImage() - Set Image: size:" << myWidth << "x" << myHeight << endl;
  }

  myIsDirty  = true;
  bhasImage = true;
}

void KomposeImage::setImage( QPixmap &pm )
{
  mySourceQPixmap = pm;
  bhasSourcePixmap = true;

  myOrigWidth  = pm.width();
  myOrigHeight = pm.height();
  
  myIsDirty = true;
}

void KomposeImage::createImageFromPixmap()
{
  if ( !bhasSourcePixmap )
    return;

  qDebug("KomposeImage::createImageFromPixmap()");
  imlib_context_set_drawable( mySourceQPixmap.handle() );

  if ( !KomposeSettings::instance()->getImageEffects() )
  {
    Imlib_Image img = imlib_create_image_from_drawable((Pixmap)0, 0, 0, mySourceQPixmap.width(), mySourceQPixmap.height(), 1);

    setImage( img );
  }
  else
  {
    Imlib_Image imgOrig = imlib_create_image_from_drawable((Pixmap)0, 0, 0, mySourceQPixmap.width(), mySourceQPixmap.height(), 1);

    imlib_context_set_anti_alias(1);

    imlib_context_set_image( imgOrig );
    Imlib_Image img = imlib_create_cropped_scaled_image(0, 0, mySourceQPixmap.width(), mySourceQPixmap.height(), myWidth, myHeight);
    imlib_free_image();

    setImage( img );
  }
}

void KomposeImage::resize( int width, int height )
{
  if ( myWidth == width && myHeight == height )
    return;

  if (bhasImage)
  {
    bhasImage = false;
    imlib_context_set_image( myIm );
    imlib_free_image();
  }

  myWidth   = width;
  myHeight  = height;
  myIsDirty = true;
}

QPixmap* KomposeImage::qpixmap( int effect )
{
  if ( KomposeSettings::instance()->getImageEffects() && (lasteffect != effect ) )
  {
    myIsDirty = true;
    lasteffect = effect;
  }

  if ( myIsDirty)
    renderPixmap();

  return &myQPixmap;
}


void KomposeImage::renderPixmap()
{
  // Is image dirty? has a width been set?
  if ( !myIsDirty || myWidth==-1 )
    return;

  emit startRendering();

  if ( !bhasImage && bhasSourcePixmap )
    createImageFromPixmap();
  else if ( !bhasImage && !bhasSourcePixmap )
    return;

  qDebug("KomposeImage::renderPixmap() - Creating %dx%d pixmap", myWidth, myHeight);

  imlib_context_set_image( myIm );
  imlib_image_set_has_alpha(0);

  if ( KomposeSettings::instance()->getImageEffects() )
    applyEffect();

  myQPixmap.resize( myWidth, myHeight);

  imlib_context_set_drawable(myQPixmap.handle());
  imlib_context_set_anti_alias(1);
  imlib_render_image_on_drawable_at_size(0, 0, myWidth, myHeight);

  emit stoppedRendering();

  myIsDirty = false;
}

void KomposeImage::applyEffect()
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
    imlib_context_set_image(myIm);
    imlib_image_fill_color_range_rectangle(0, 0, myWidth, KomposeSettings::instance()->getWindowTitleFontAscent() * 3, -180.0);
    /* free it */
    imlib_free_color_range();
  }

}

#include "komposeimage.moc"
