//
// C++ Interface: komposeglobal
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSEGLOBAL_H
#define KOMPOSEGLOBAL_H

#include "komposesystray.h"

#include <qobject.h>


/**
@author Hans Oischinger
*/
class KomposeGlobal : public QObject
{
  Q_OBJECT
protected:
  KomposeGlobal(QObject *parent = 0, const char *name = 0);

  ~KomposeGlobal();

public:
  static KomposeGlobal *instance();
  
  KomposeSysTray* getSysTray() { return systray; }

private:
  KomposeSysTray* systray;
};

#endif
