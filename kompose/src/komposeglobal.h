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
class KSharedPixmap;

// Check for Composite extension
// FIXME: Am I doing this right? I don't know anything about automake & co
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(Q_WS_X11) && defined(HAVE_XCOMPOSITE) && defined(HAVE_XDAMAGE) && QT_VERSION >= 0x030300
# define COMPOSITE
#endif

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
  
  void setHideSystray( bool b ) { hideSystray = b; }
  void setSingleShot( bool b ) { singleShot = b; }
  bool getSingleShot() { return singleShot; }
  bool hasXcomposite() { return xcomposite; }
  int getDamageEvent() { return damageEvent; }
  
  // Action getters
  KAction *getActConfigGlobalShortcuts() { return actConfigGlobalShortcuts; }
  KAction *getActPreferencesDialog() { return actPreferencesDialog; }
  KAction *getActShowVirtualDesktopView() { return actShowVirtualDesktopView; }
  KAction *getActShowWorldView() { return actShowWorldView; }
  KAction *getActAboutDlg() { return actAboutDlg; }
  KAction *getActQuit() { return actQuit; }

  const KSharedPixmap *getDesktopBgPixmap() const { return desktopBgPixmap; }
  
protected:
  void initActions();
  void initSharedPixmaps();
  void initImlib();
  void initCompositeExt();

protected slots:
  void showGlobalShortcutsSettingsDialog();
  void showAboutDlg();

private:
  bool hideSystray;
  bool singleShot;
  bool xcomposite;
  int damageEvent, damageError;
  
  KSharedPixmap *desktopBgPixmap;
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
