//
// C++ Interface: komposeimage
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSEIMAGE_H
#define KOMPOSEIMAGE_H

#include <qobject.h>
#include <qstring.h>
#include <qpixmap.h>

// #include those AFTER Qt-includes!
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Imlib2.h>

enum ImageEffect { IEFFECT_NONE, IEFFECT_TITLE, IEFFECT_MINIMIZED, IEFFECT_MINIMIZED_AND_TITLE, IEFFECT_HIGHLIGHT };

/**
@author Hans Oischinger
*/
class KomposeImage : public QObject
{
  Q_OBJECT

public:
  KomposeImage( Imlib_Image &im );
  KomposeImage();
  ~KomposeImage();

  int width() 	const { return myWidth; }
  int height()	const { return myHeight; }
  int originalWidth() const { return myOrigWidth; }
  int originalHeight() const { return myOrigHeight; }

  void resize( int width, int height );
  
  QPixmap* qpixmap( int effect );
  void renderPixmap();
  
  void setDirty( bool d ) { myIsDirty = d; }
  bool isDirty() const { return myIsDirty; }
  bool isValid() { return bhasImage || bhasSourcePixmap; }
  bool hasImage() { return bhasImage; }
  void setImage( Imlib_Image &im );
  void setImage( QPixmap &pm );
  
public slots:
  void clearCached();

protected:
  void createImageFromPixmap();
  void applyEffect();
  void init();
  
private:
  int myWidth;
  int myHeight;
  
  Imlib_Image myIm;
  QPixmap myQPixmap;
  QPixmap mySourceQPixmap;
  
  Imlib_Color_Modifier cmHighlight, cmMinimized;
  
  bool myIsDirty;
  bool bhasImage;
  bool bhasSourcePixmap;

  int myOrigWidth;
  int myOrigHeight;

  int lasteffect;
  
signals:
  void startRendering();
  void stoppedRendering();
};

#endif
