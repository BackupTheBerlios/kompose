//
// C++ Interface: komposepreferences
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KOMPOSEPREFERENCES_H
#define KOMPOSEPREFERENCES_H

#include <kdialogbase.h>

#include <klocale.h>

class QCheckBox;
class QFrame;
class QSpinBox;
class KColorButton;
class QButtonGroup;
class QSlider;
class QPushButton;
class QFont;

/**
@author Hans Oischinger
*/
class KomposePreferences : public KDialogBase
{
Q_OBJECT
public:
  KomposePreferences();

  ~KomposePreferences();


protected:
  void fillPages();
  
protected slots:
  void slotApply();
  void slotOk();
  void updateIconSliderDesc( int );
  void showFontDialog();

private:
  QFrame *page1;
  QFrame *page2;
  
  QButtonGroup *defaultViewBtnGroup;
  QCheckBox *defaultViewWorld;
  QCheckBox *defaultViewVirtualDesks;
  
  QCheckBox *passiveScreenshots;
//   QCheckBox *onlyOneScreenshot;
  QCheckBox *imageEffects;
  QCheckBox *tintVirtDesks;
  KColorButton *tintVirtDesksColor;
  
  QSpinBox *screenshotGrabDelay;
  
  QCheckBox *showIcons;
  QSlider *iconSize;
  QLabel *iconSizeDescription;
  
  QCheckBox *showWindowTitles;
  QPushButton *windowTitleFontBtn;
  QFont *windowTitleFont;
  
  KColorButton *windowTitleFontColor;
  QCheckBox *showWindowTitleShadow;
  KColorButton *windowTitleFontShadowColor;
  QLabel *windowTitleFontColorLabel;
};

#endif
