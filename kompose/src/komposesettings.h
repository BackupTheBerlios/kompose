//
// C++ Interface: komposesettings
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
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

enum KomposeDisplay { KOMPOSEDISPLAY_WORLD, KOMPOSEDISPLAY_VIRTUALDESKS };

class KGlobalAccel;
class KPixmapIO;
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
  
  KGlobalAccel *getGlobalAccel() const { return globalAccel; }
  KPixmapIO *getPixmapIO() { return pixmapIO; }
  
  bool getPassiveScreenshots() const { return passiveScreenshots; }
  void setPassiveScreenshots( bool b ) { passiveScreenshots = b; }
//   bool getOnlyOneScreenshot() const { return onlyOneScreenshot; }
//   void setOnlyOneScreenshot( bool b ) { onlyOneScreenshot = b; }
  uint getScreenshotGrabDelay() { return screenshotGrabDelay; }
  void setScreenshotGrabDelay( uint val ) { screenshotGrabDelay=val; }
  bool getDynamicVirtDeskLayout() const { return dynamicVirtDeskLayout; }
  void setDynamicVirtDeskLayout( bool b ) { dynamicVirtDeskLayout = b; }
  bool getImageEffects() const { return imageEffects; }
  void setImageEffects( bool b ) { imageEffects = b; }
  bool getTintVirtDesks() const { return tintVirtDesks; }
  void setTintVirtDesks( bool b ) { tintVirtDesks = b; }
  const QColor& getTintVirtDesksColor() const { return tintVirtDesksColor; }
  void setTintVirtDesksColor( QColor c ) { tintVirtDesksColor = c; }
  
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
  
  
public slots:
  void showPreferencesDlg();

signals:
  void settingsChanged();

private:
  KGlobalAccel *globalAccel;
  KPixmapIO *pixmapIO;
  
  bool passiveScreenshots;
  uint screenshotGrabDelay;
  
  bool dynamicVirtDeskLayout;
  
  bool imageEffects;
  bool tintVirtDesks;
  QColor tintVirtDesksColor;
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
};

#endif
