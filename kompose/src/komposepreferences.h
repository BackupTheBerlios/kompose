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

/**
@author Hans Oischinger
*/
class KomposePreferences : public KDialogBase
{
public:
  KomposePreferences();

  ~KomposePreferences();


protected:
  void fillPages();
  
protected slots:
  void slotApply();
  void slotOk();

private:
  QFrame *page1;
  QFrame *page2;
  
  QCheckBox *passiveScreenshots;
  QCheckBox *overwriteOldScreenshots;
  QCheckBox *highlightWindows;
  QCheckBox *tintVirtDesks;
  KColorButton *tintVirtDesksColor;
  
  QSpinBox *screenshotGrabDelay;
};

#endif
