//
// C++ Interface: abstractviewwidget
//
// Description: 
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ABSTRACTVIEWWIDGET_H
#define ABSTRACTVIEWWIDGET_H

#include "komposetaskcontainerwidget.h"
#include "komposesettings.h"

/**
@author Hans Oischinger
*/
class AbstractViewWidget : public KomposeTaskContainerWidget
{
public:
    AbstractViewWidget( int displayType = KOMPOSEDISPLAY_VIRTUALDESKS ,KomposeLayout *l = 0 );
    ~AbstractViewWidget();

  virtual void setType( int t ) { type = t; }
  int getType() { return type; }

protected:
  int type;
};

#endif
