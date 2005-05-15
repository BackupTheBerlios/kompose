//
// C++ Interface: komposesettings
//
// Description:
//
//
// Author: Hans Oischinger <oisch@users.berlios.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSESETTINGS_H
#define KOMPOSESETTINGS_H

#include <qobject.h>
#include <qcolor.h>
#include <qfont.h>

#define MAX_DESKTOPS 16

enum KomposeDisplay { KOMPOSEDISPLAY_WORLD, KOMPOSEDISPLAY_VIRTUALDESKS, KOMPOSEDISPLAY_CURRENTDESK };

class KGlobalAccel;
class QFontMetrics;


/**
@author Hans Oischinger
*/
class KomposeSettings : public QObject
{
  Q_OBJECT
protected:
  KomposeSettings(QObject *parent = 0, const char *name = 0);

  ~KomposeSettings();

public:
  static KomposeSettings *instance();
  void readConfig();
  void writeConfig();

  bool hasDialogOpen() { return dialogOpen; }
  KGlobalAccel *getGlobalAccel() const { return globalAccel; }

  bool getUseComposite() const { return useComposite; }
  void setUseComposite( bool b ) { useComposite = b; }

  bool getPassiveScreenshots() const { return passiveScreenshots; }
  void setPassiveScreenshots( bool b ) { passiveScreenshots = b; }
  uint getScreenshotGrabDelay() { return screenshotGrabDelay; }
  void setScreenshotGrabDelay( uint val ) { screenshotGrabDelay=val; }
  bool getDynamicVirtDeskLayout() const { return dynamicVirtDeskLayout; }
  void setDynamicVirtDeskLayout( bool b ) { dynamicVirtDeskLayout = b; }
  bool getImageEffects() const { return imageEffects; }
  void setImageEffects( bool b ) { imageEffects = b; }

  int  getDefaultView() const { return defaultView; }
  void setDefaultView( int d ) { defaultView = d; }
  int getIconSize() { return iconSize; }
  void setIconSize( int i ) { iconSize = i; }
  int getShowIcons() { return showIcons; }
  void setShowIcons( bool b ) { showIcons = b; }
  int getIconSizePixels();

  void setWindowTitleFont( QFont f ) { windowTitleFont = f; }
  const QFont& getWindowTitleFont() const { return windowTitleFont; }
  int getShowWindowTitles() { return showWindowTitles; }
  void setShowWindowTitles( bool b ) { showWindowTitles = b; }
  int getShowWindowTitleShadow() { return showWindowTitleShadow; }
  void setShowWindowTitleShadow( bool b ) { showWindowTitleShadow = b; }
  const QColor& getWindowTitleFontColor() const { return windowTitleFontColor; }
  void setWindowTitleFontColor( QColor c ) { windowTitleFontColor = c; }
  const QColor& getWindowTitleFontShadowColor() const { return windowTitleFontShadowColor; }
  void setWindowTitleFontShadowColor( QColor c ) { windowTitleFontShadowColor = c; }

  const QFontMetrics* getWindowTitleFontMetrics() { return windowTitleFontMetrics; }
  int getWindowTitleFontAscent() { return windowTitleFontAscent; }
  int getWindowTitleFontHeight() { return windowTitleFontHeight; }

  void setDesktopTitleFont( QFont f ) { desktopTitleFont = f; }
  const QFont& getDesktopTitleFont() const { return desktopTitleFont; }
  const QColor& getDesktopTitleFontColor() const { return desktopTitleFontColor; }
  void setDesktopTitleFontColor( QColor c ) { desktopTitleFontColor = c; }
  const QColor& getDesktopTitleFontHighlightColor() const { return desktopTitleFontHighlightColor; }
  void setDesktopTitleFontHighlightColor( QColor c ) { desktopTitleFontHighlightColor = c; }

  bool getCacheScaledPixmaps() const { return cacheScaledPixmaps; }
  void setCacheScaledPixmaps( bool b ) { cacheScaledPixmaps = b; }

  bool getActivateOnTopLeftCorner() const { return activateOnTopLeftCorner; }
  void setActivateOnTopLeftCorner( bool b ) { activateOnTopLeftCorner = b; }
  bool getActivateOnTopRightCorner() const { return activateOnTopRightCorner; }
  void setActivateOnTopRighCorner( bool b ) { activateOnTopRightCorner = b; }
  bool getActivateOnBottomLeftCorner() const { return activateOnBottomLeftCorner; }
  void setActivateOnBottomLeftCorner( bool b ) { activateOnBottomLeftCorner = b; }
  bool getActivateOnBottomRightCorner() const { return activateOnBottomRightCorner; }
  void setActivateOnBottomRightCorner( bool b ) { activateOnBottomRightCorner = b; }

  bool getActivateOnTopEdge() const { return activateOnTopEdge; }
  void setActivateOnTopEdge( bool b ) { activateOnTopEdge = b; }
  bool getActivateOnLeftEdge() const { return activateOnLeftEdge; }
  void setActivateOnLeftEdge( bool b ) { activateOnLeftEdge= b; }
  bool getActivateOnBottomEdge() const { return activateOnBottomEdge; }
  void setActivateOnBottomEdge( bool b ) { activateOnBottomEdge = b; }
  bool getActivateOnRightEdge() const { return activateOnRightEdge; }
  void setActivateOnRightEdge( bool b ) { activateOnRightEdge = b; }

  uint getAutoLockDelay() { return autoLockDelay; }
  void setAutoLockDelay( uint val ) { autoLockDelay=val; }

  void setShowDesktopNum( bool b ) { showDesktopNum = b; }
  bool getShowDesktopNum() const { return showDesktopNum; }

public slots:
  void showPreferencesDlg();

protected slots:
  void calcFontMetrics();

signals:
  void settingsChanged();

private:
  KGlobalAccel *globalAccel;
  bool dialogOpen;

  bool useComposite;
  bool passiveScreenshots;
  uint screenshotGrabDelay;

  bool dynamicVirtDeskLayout;

  bool imageEffects;
  int defaultView;

  int iconSize;
  bool showIcons;

  bool showWindowTitles;
  QFont windowTitleFont;
  QFontMetrics *windowTitleFontMetrics;
  int windowTitleFontAscent;
  int windowTitleFontHeight;
  QColor windowTitleFontColor;
  bool showWindowTitleShadow;
  QColor windowTitleFontShadowColor;

  QFont desktopTitleFont;
  QColor desktopTitleFontColor;
  QColor desktopTitleFontHighlightColor;

  bool cacheScaledPixmaps;

  bool activateOnTopLeftCorner;
  bool activateOnTopRightCorner;
  bool activateOnBottomLeftCorner;
  bool activateOnBottomRightCorner;

  bool activateOnTopEdge;
  bool activateOnLeftEdge;
  bool activateOnBottomEdge;
  bool activateOnRightEdge;

  uint autoLockDelay;

  bool showDesktopNum;
};

#endif
