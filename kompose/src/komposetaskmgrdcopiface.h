//
// C++ Interface: komposetaskmgrdcopiface
//
// Description: 
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSETASKMGRDCOPIFACE_H
#define KOMPOSETASKMGRDCOPIFACE_H

#include <dcopobject.h>

/**
@author Hans Oischinger
*/

class KomposeTaskMgrDcopIface : virtual public DCOPObject
{
  K_DCOP

  k_dcop:

//     virtual ASYNC createDefaultView(QString someParameter) = 0;
    virtual void createDefaultView() = 0;
    virtual void createVirtualDesktopView() = 0;
    virtual void createWorldView() = 0;
    virtual void closeCurrentView() = 0;
};

#endif
