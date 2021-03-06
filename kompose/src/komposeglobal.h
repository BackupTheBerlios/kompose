//
// C++ Interface: komposeglobal
//
// Description:
//
//
// Author: Hans Oischinger <hans.oischinger@kde-mail.net>, (C) 2004
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
class KWinModule;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(Q_WS_X11) && defined(HAVE_XCOMPOSITE) && defined(HAVE_XDAMAGE) && QT_VERSION >= 0x030300
# define COMPOSITE
#endif

class KGlobalAccel;
class KConfigDialog;
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
  static KomposeGlobal *self();
  KomposeSysTray* getSysTray() { return systray; }
  void initGui();

  bool hasDialogOpen() { return m_dialogOpen; }
  void setHideSystray( bool b ) { hideSystray = b; }
  void setSingleShot( bool b ) { singleShot = b; }
  bool getSingleShot() { return singleShot; }
  bool hasXcomposite() { return xcomposite; }
  int getDamageEvent() { return damageEvent; }

  // Action getters
  KAction *getActConfigGlobalShortcuts() { return actConfigGlobalShortcuts; }
  KAction *getActPreferencesDialog() { return actPreferencesDialog; }
  KAction *getActShowVirtualDesktopView() { return actShowVirtualDesktopView; }
  KAction *getActShowCurrentDesktopView() { return actShowCurrentDesktopView; }
  KAction *getActShowWorldView() { return actShowWorldView; }
  KAction *getActAboutDlg() { return actAboutDlg; }
  KAction *getActQuit() { return actQuit; }

  KGlobalAccel *globalAccel() const { return m_globalAccel; }
  KPopupMenu *getViewMenu() { return m_viewMenu; }
  const KSharedPixmap *getDesktopBgPixmap() const { return desktopBgPixmap; }

public slots:
  void showPreferencesDlg();

signals:
  void settingsChanged();

protected:
  void initActions();
  void initImlib();
  void initMenus();

protected slots:
  void initSharedPixmaps();
  QString pixmapName(int desk);
  void slotDone(bool success);
  void enablePixmapExports();
  void slotDesktopChanged(int desktop);
  void slotBackgroundChanged(int desktop);
  void refreshSharedPixmaps();

  void slotConfigChanged();
  void initCompositeExt();
  void showGlobalShortcutsSettingsDialog();
  void showAboutDlg();
  void cfgDlgFinished();

private:
  KGlobalAccel *m_globalAccel;
  bool m_dialogOpen;
  bool hideSystray;
  bool singleShot;
  bool xcomposite;
  int damageEvent, damageError;

  KSharedPixmap *desktopBgPixmap;
  int currentDesktop;
  KomposeSysTray* systray;
  KActionCollection* actionCollection;

  KAction *actConfigGlobalShortcuts;
  KAction *actPreferencesDialog;
  KAction *actShowVirtualDesktopView;
  KAction *actShowCurrentDesktopView;
  KAction *actShowWorldView;
  KAction *actAboutDlg;
  KAction *actQuit;

  KPopupMenu *m_viewMenu;
  KConfigDialog *m_cfgDialog;

  KWinModule* kwin_module;
};

#endif
