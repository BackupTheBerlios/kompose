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
#ifndef KOMPOSEDCOPIFACE_H
#define KOMPOSEDCOPIFACE_H

#include <dcopobject.h>

/**
@author Hans Oischinger
*/
class KomposeDcopIface : virtual public DCOPObject
{
  K_DCOP

  k_dcop:

    virtual void createDefaultView() = 0;
    virtual void createVirtualDesktopView() = 0;
    virtual void createWorldView() = 0;
    virtual void closeCurrentView() = 0;
};

#endif
