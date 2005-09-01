//
// C++ Implementation: abstractviewwidget
//
// Description: 
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "abstractviewwidget.h"

AbstractViewWidget::AbstractViewWidget( int displayType, KomposeLayout *l)
 : KomposeTaskContainerWidget( -1,  0, l ),
    type(displayType)
{
}


AbstractViewWidget::~AbstractViewWidget()
{
}


