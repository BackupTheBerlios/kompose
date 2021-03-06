//
// C++ Interface: komposepreferences
//
// Description:
//
//
// Author: Hans Oischinger <hans.oischinger@kde-mail.net>, (C) 2004
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
class QRadioButton;

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
  void showDesktopTitleFontDialog();
  void showWindowTitleFontDialog();
  void setUseCompositeToggled( bool );

private:
  bool m_hasXinerama;
  QFrame *page1;
  QFrame *page2;

  QButtonGroup *defaultViewBtnGroup;
  QRadioButton *defaultViewWorld;
  QRadioButton *defaultViewVirtualDesks;
  QRadioButton *defaultViewCurrentDesk;

  QCheckBox *useComposite;

  QCheckBox *passiveScreenshots;
  QCheckBox *dynamicVirtDeskLayout;
  QCheckBox *cacheScaledPixmaps;

  QCheckBox *imageEffects;

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

  QPushButton *desktopTitleFontBtn;
  QLabel *desktopTitleFontColorLabel;
  QLabel *desktopTitleFontHighlightColorLabel;
  KColorButton *desktopTitleFontColor;
  KColorButton *desktopTitleFontHighlightColor;
  QFont *desktopTitleFont;

  QCheckBox *m_topLeftCorner;
  QCheckBox *m_bottomLeftCorner;
  QCheckBox *m_topRightCorner;
  QCheckBox *m_bottomRightCorner;

  QCheckBox *m_topEdge;
  QCheckBox *m_bottomEdge;
  QCheckBox *m_rightEdge;
  QCheckBox *m_leftEdge;
  QSpinBox *autoLockDelay;

  QCheckBox *showDesktopNum;

  QSpinBox *m_viewScreen;
  QCheckBox *m_viewScreenAll;
};

#endif
