//
// C++ Interface: komposesystray
//
// Description:
//
//
// Author: Hans Oischinger <oisch@users.berlios.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSESYSTRAY_H
#define KOMPOSESYSTRAY_H

#include <ksystemtray.h>

#include <qimage.h>

class KAction;
class KGlobalAccel;
class KPopupMenu;

/**
@author Hans Oischinger
*/
class KomposeSysTray : public KSystemTray
{
  Q_OBJECT
public:
  KomposeSysTray(QWidget *parent = 0, const char *name = 0);
  ~KomposeSysTray();

  void currentDesktopChanged(int desktop);

protected:
  void mouseReleaseEvent (QMouseEvent *);
  void mousePressEvent (QMouseEvent *);

protected slots:
  void slotConfigChanged();

private:
  KPopupMenu *menu;
  QImage icon;
};

#endif
