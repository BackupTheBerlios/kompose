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

#define MAX_DESKTOPS 16

enum KomposeDisplay { KOMPOSEDISPLAY_WORLD, KOMPOSEDISPLAY_VIRTUALDESKS };

class KGlobalAccel;
class KPixmapIO;;
class QColor;

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
  bool getOnlyOneScreenshot() const { return onlyOneScreenshot; }
  void setOnlyOneScreenshot( bool b ) { onlyOneScreenshot = b; }
  uint getScreenshotGrabDelay() { return screenshotGrabDelay; }
  void setScreenshotGrabDelay( uint val ) { screenshotGrabDelay=val; }
  bool getHighlightWindows() const { return highlightWindows; }
  void setHighlightWindows( bool b ) { highlightWindows = b; }
  bool getTintVirtDesks() const { return tintVirtDesks; }
  void setTintVirtDesks( bool b ) { tintVirtDesks = b; }
  const QColor& getTintVirtDesksColor() const { return tintVirtDesksColor; }
  void setTintVirtDesksColor( QColor c ) { tintVirtDesksColor = c; }
  int  getDefaultView() const { return defaultView; }
  void setDefaultView( int d ) { defaultView = d; }
    
public slots:
  void showPreferencesDlg();

signals:
  void settingsChanged();

private:
  KGlobalAccel *globalAccel;
  KPixmapIO *pixmapIO;
  
  bool passiveScreenshots;
  bool onlyOneScreenshot;
  uint screenshotGrabDelay;
  bool highlightWindows;
  bool tintVirtDesks;
  QColor tintVirtDesksColor;
  int defaultView;
};

#endif
