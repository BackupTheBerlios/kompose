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

class KomposeSysTray;

#include <qobject.h>
#include <qstring.h>

class KPopupMenu;
class KActionCollection;
class KAction;

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
  void initGui();
  void initImlib();
  
  void setHideSystray( bool b ) { hideSystray = b; }
  void setSingleShot( bool b ) { singleShot = b; }
  bool getSingleShot() { return singleShot; }

  // Action getters
  KAction *getActConfigGlobalShortcuts() { return actConfigGlobalShortcuts; }
  KAction *getActPreferencesDialog() { return actPreferencesDialog; }
  KAction *getActShowVirtualDesktopView() { return actShowVirtualDesktopView; }
  KAction *getActShowWorldView() { return actShowWorldView; }
  KAction *getActAboutDlg() { return actAboutDlg; }
  KAction *getActQuit() { return actQuit; }

protected:
  void initActions();

protected slots:
  void showGlobalShortcutsSettingsDialog();
  void showAbutDlg();

private:
  bool hideSystray;
  bool singleShot;
  KomposeSysTray* systray;
  KActionCollection* actionCollection;

  KAction *actConfigGlobalShortcuts;
  KAction *actPreferencesDialog;
  KAction *actShowVirtualDesktopView;
  KAction *actShowWorldView;
  KAction *actAboutDlg;
  KAction *actQuit;
};

#endif
