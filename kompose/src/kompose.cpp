/***************************************************************************
 *   Copyright (C) 2004 by Hans Oischinger                                 *
 *   oisch@sourceforge.net                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "kompose.h"

#include "komposeglobal.h"
#include "komposetaskmanager.h"

#include <klocale.h>
#include <kuniqueapplication.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifdef COMPOSITE
#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xrender.h>
#endif

Kompose::Kompose()
{
  KomposeGlobal::instance()->initGui();
}

Kompose::~Kompose()
{}

bool Kompose::x11EventFilter (XEvent *event)
{
#ifdef COMPOSITE
  if ( KomposeGlobal::instance()->hasXcomposite() )
  {
    if ( event->type == KomposeGlobal::instance()->getDamageEvent() + XDamageNotify )
    {
      KomposeTaskManager::instance()->processX11Event( event );
      //     XDamageNotifyEvent *e = reinterpret_cast<XDamageNotifyEvent*>( event );
      //     // e->drawable is the window ID of the damaged window
      //     // e->geometry is the geometry of the damaged window
      //     // e->area     is the bounding rect for the damaged area
      //     // e->damage   is the damage handle returned by XDamageCreate()
      //
      //     // Subtract all the damage, repairing the window.
      //     XDamageSubtract( dpy, e->damage, None, None );
    }
    //
    //   else if ( event->type == shape_event + ShapeNotify )
    //   {
    //     XShapeEvent *e = reinterpret_cast<XShapeEvent*>( event );
    //     // It's probably safe to assume that the window shape region
    //     // is invalid at this point...
    //   }

    else if ( event->type == ConfigureNotify )
    {
      // XConfigureEvent *e = &event->xconfigure;
      // The windows size, position or Z index in the stacking
      // order has changed
      KomposeTaskManager::instance()->processX11Event( event );
    }

  }
#endif
  
  // This costed me nerves:
  // Call this or kwinmodule won't work
  return KApplication::x11EventFilter(event);
}


#include "kompose.moc"
