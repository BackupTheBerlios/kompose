//
// C++ Interface: komposesystray
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSESYSTRAY_H
#define KOMPOSESYSTRAY_H

#include <ksystemtray.h>


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

protected:
  void mouseReleaseEvent (QMouseEvent *);
  void mousePressEvent (QMouseEvent *);

public slots:
  void showGlobalShortcutsSettingsDialog();
  
private:
  KPopupMenu *menu;
  KAction *actConfigGlobalShortcuts;
  KAction *actPreferencesDialog;
  KAction *actShowVirtualDesktopView;
  KAction *actShowWorldView;
};

#endif
