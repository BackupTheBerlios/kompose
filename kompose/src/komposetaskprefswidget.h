//
// C++ Interface: komposetaskprefswidget
//
// Description:
//
//
// Author: Hans Oischinger <hans.oischinger@kde-mail.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSETASKPREFSWIDGET_H
#define KOMPOSETASKPREFSWIDGET_H

#include <qdockarea.h> 

class KomposeTaskWidget;

/**
@author Hans Oischinger
*/
class KomposeTaskPrefsWidget : public QDockArea
{
  Q_OBJECT
public:
  KomposeTaskPrefsWidget(KomposeTaskWidget *parent = 0, const char *name = 0);

  ~KomposeTaskPrefsWidget();

};

#endif
