//
// C++ Interface: komposeglwidget
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSEGLFULLSCREENWIDGET_H
#define KOMPOSEGLFULLSCREENWIDGET_H

#include <qgl.h>

#include "komposewidgetinterface.h"

#define MAX_DESKTOPS 16

class KomposeLayout;
class KomposeGLDesktopWidget;

/**
@author Hans Oischinger
*/
class KomposeGLFullscreenWidget : public QGLWidget, public KomposeWidgetInterface
{
  Q_OBJECT
public:
  KomposeGLFullscreenWidget(QWidget *parent = 0, const char *name = 0);

  ~KomposeGLFullscreenWidget();

  // KomposeWidgetInterface (we have to double some code here as we cannot inherit from KomposeGLWidget,
  // both are QObject and  Qt has a silly limitation when using multiple inheritance
  virtual void removeChildWidget( KomposeWidgetInterface* obj );
  virtual void setGeom( const QRect & );
  virtual void setGeom( const QSize & );
  virtual QSize getSize() const;
  virtual QRect getRect() const;
  // unused overridden functions (just for the sake of it)
  virtual KomposeWidgetInterface* getParentWidget() const { return 0; }
  virtual int getHeightForWidth( int w ) const { return 1; }
  virtual int getWidthForHeight( int h ) const { return 1; }
  virtual double getAspectRatio() { return 1.0; }

protected:
  void paintGL();
  void initializeGL();
  void resizeGL( int w, int h );

  void createDesktopWidgets();
  void setOrthographicProjection();
  
private:
  KomposeLayout *layout;
  KomposeGLDesktopWidget *desktop[MAX_DESKTOPS];
};

#endif
