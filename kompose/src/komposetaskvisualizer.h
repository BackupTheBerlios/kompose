//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSETASKVISUALIZER_H
#define KOMPOSETASKVISUALIZER_H

#include <qobject.h>

#include "komposetask.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Imlib2.h>

#ifdef COMPOSITE
#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xrender.h>
#endif

class QPixmap;
class QSize;

/**
@author Hans Oischinger
*/
class KomposeTaskVisualizer : public QObject
{
  Q_OBJECT
public:
  KomposeTaskVisualizer(KomposeTask *parent, const char *name = 0);
  ~KomposeTaskVisualizer();

  void renderOnPixmap(QPixmap* pix);

protected:
  void initXComposite();

protected slots:
  void captureScreenshot_GrabWindow();
  void updateXCompositeNamedPixmap();
  void renderScaledScreenshot( QSize size );
  
  void setScaledScreenshotDirty() { scaledScreenshotDirty = true; }
  void clearCached();
  void enablePasvScreenshots();
  
public slots:
  void slotTaskActivated();
  void slotUpdateScreenshot();

private:
  KomposeTask *task;
  QPixmap screenshot;
  QPixmap scaledScreenshot;
  bool scaledScreenshotDirty;
  bool screenshotSuspended;   // suspend pasv screenshots for this task
  bool screenshotBlocked;   // dis/enable pasv screenshots for this task
  
#ifdef COMPOSITE
  Pixmap windowBackingPix;
  bool validBackingPix;
  Display *dpy;
  XRenderPictFormat *format;
  XRenderPictureAttributes pa;
  bool hasAlpha;
  Damage damage;
#endif
};

#endif
