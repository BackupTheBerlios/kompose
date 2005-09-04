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

AbstractViewWidget::AbstractViewWidget( int displayType, KomposeLayout *l, WFlags f)
 : KomposeTaskContainerWidget( -1,  0, l, "abstrviewvidget", f ),
    type(displayType)
{
}


AbstractViewWidget::~AbstractViewWidget()
{
}


