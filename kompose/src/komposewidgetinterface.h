//
// C++ Interface: komposewidgetinterface
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSEWIDGETINTERFACE_H
#define KOMPOSEWIDGETINTERFACE_H

#include <qrect.h>
#include <qsize.h>

/**
@author Hans Oischinger
*/
struct KomposeWidgetInterface
{
// public:
//   KomposeWidgetInterface();
// 
//   virtual ~KomposeWidgetInterface();
//   
  virtual KomposeWidgetInterface* getParentWidget() const = 0;
    
  virtual void setGeom( const QRect & ) = 0;
  virtual void setGeom( const QSize & ) = 0;

  virtual QSize getSize() const = 0;
  virtual QRect getRect() const = 0;
  
  virtual int getHeightForWidth( int w ) const = 0;
  virtual int getWidthForHeight( int h ) const = 0;
  virtual double getAspectRatio() = 0;

};

#endif
