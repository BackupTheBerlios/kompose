//
// C++ Implementation: komposepreferences
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposepreferences.h"

#include "komposesettings.h"

#include <qlayout.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <kapplication.h>
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qspinbox.h>

#include <kiconloader.h>
#include <kcolorbutton.h>

KomposePreferences::KomposePreferences()
    : KDialogBase(IconList, i18n("Komposé Preferences"), Ok|Apply|Cancel, Ok)
{
  QFrame *page1 = addPage( i18n("Screenshots"), QString::null, DesktopIcon("winprops", KIcon::SizeMedium) );
  QFrame *page2 = addPage( i18n("Appearance"), QString::null, DesktopIcon("appearance", KIcon::SizeMedium) );

  QVBoxLayout *page1Layout = new QVBoxLayout( page1, 0, KDialog::spacingHint() );

  passiveScreenshots = new QCheckBox(i18n("Passive Screenshots"), page1);
  QString passiveScreenshotsHelp = i18n("Create a screenshot whenever you raise or active a window.\nWhen selected the amount the annoying popup-effect before every Komposé activation will be minimized to nearly zero.\nThe drawback is that the screenshots are not so recent and may not display the actual content.");
  QWhatsThis::add( passiveScreenshots, passiveScreenshotsHelp );
  QToolTip::add( passiveScreenshots, passiveScreenshotsHelp );
  page1Layout->addWidget(passiveScreenshots);

  overwriteOldScreenshots = new QCheckBox(i18n("Create screenshots whenever possible"), page1);
  QString overwriteOldScreenshotsHelp = i18n("When enabled old screenshots will be overwritten whenever possible.\n Disabling will only create a screenshot once the application is first activated and will never update it.");
  QWhatsThis::add( overwriteOldScreenshots, overwriteOldScreenshotsHelp );
  QToolTip::add( overwriteOldScreenshots, overwriteOldScreenshotsHelp );
  page1Layout->addWidget(overwriteOldScreenshots);

  QHBoxLayout *hLayScreenshotGrabDelay = new QHBoxLayout(0, 0, 6);
  screenshotGrabDelay = new QSpinBox(0, 2000, 1, page1);
  QLabel *screenshotGrabDelayLabel = new QLabel(screenshotGrabDelay, i18n("Delay between Screenshots (ms)"), page1);
  QString screenshotGrabDelayHelp = i18n("Specifies the time to wait between the Activation of a window and the screenshot Taking.\nIncrease it when your windows need more time to draw themselves after activation.\nValues below 300ms are not recommended, but may work in some cases" );
  QWhatsThis::add( screenshotGrabDelay, screenshotGrabDelayHelp );
  QToolTip::add( screenshotGrabDelay, screenshotGrabDelayHelp );
  QWhatsThis::add( screenshotGrabDelayLabel, screenshotGrabDelayHelp );
  QToolTip::add( screenshotGrabDelayLabel, screenshotGrabDelayHelp );
  hLayScreenshotGrabDelay->addWidget(screenshotGrabDelayLabel);
  hLayScreenshotGrabDelay->addWidget(screenshotGrabDelay);
  hLayScreenshotGrabDelay->insertStretch(-1);
  page1Layout->addLayout(hLayScreenshotGrabDelay);

  page1Layout->insertStretch(-1);



  QVBoxLayout *page2Layout = new QVBoxLayout( page2, 0, KDialog::spacingHint() );
  
  QHBoxLayout *hLayTintVirtDesks = new QHBoxLayout(0, 0, 6);
  tintVirtDesks = new QCheckBox(i18n("Tint virtual desktop widgets: "), page2);
  tintVirtDesksColor = new KColorButton(Qt::blue, page2);
  QString tintVirtDesksHelp = i18n("Colorize the transparent background of the virtual Desktop widgets" );
  QWhatsThis::add( tintVirtDesks, tintVirtDesksHelp );
  QToolTip::add( tintVirtDesks, tintVirtDesksHelp );
  QWhatsThis::add( tintVirtDesksColor, tintVirtDesksHelp );
  QToolTip::add( tintVirtDesksColor, tintVirtDesksHelp );
  hLayTintVirtDesks->addWidget(tintVirtDesks);
  hLayTintVirtDesks->addWidget(tintVirtDesksColor);
  hLayTintVirtDesks->addStretch();
  page2Layout->addLayout(hLayTintVirtDesks);
  connect( tintVirtDesks, SIGNAL(toggled(bool)), tintVirtDesksColor, SLOT(setEnabled(bool)) );

  highlightWindows = new QCheckBox(i18n("Highlight Windows on MouseOver"), page2);
  QString highlightWindowsHelp = i18n("Lighten windows when the mouse moves over them.\nThis may cause Kompose to take some ms longer to show up." );
  QWhatsThis::add( highlightWindows, highlightWindowsHelp );
  QToolTip::add( highlightWindows, highlightWindowsHelp );
  page2Layout->addWidget(highlightWindows);

  page2Layout->insertStretch(-1);

  fillPages();
}


KomposePreferences::~KomposePreferences()
{}


void KomposePreferences::fillPages()
{
  // Sync Settings to Preferences dialog
  passiveScreenshots->setChecked( KomposeSettings::instance()->getPassiveScreenshots() );
  overwriteOldScreenshots->setChecked( KomposeSettings::instance()->getOverwriteOldScreenshots() );
  screenshotGrabDelay->setValue( KomposeSettings::instance()->getScreenshotGrabDelay() / 1000000 );

  highlightWindows->setChecked( KomposeSettings::instance()->getHighlightWindows() );
  tintVirtDesks->setChecked( KomposeSettings::instance()->getTintVirtDesks() );
  tintVirtDesksColor->setColor( KomposeSettings::instance()->getTintVirtDesksColor() );
  tintVirtDesksColor->setEnabled( KomposeSettings::instance()->getTintVirtDesks() );
}




void KomposePreferences::slotApply()
{
  KomposeSettings::instance()->setPassiveScreenshots( passiveScreenshots->isChecked() );
  KomposeSettings::instance()->setOverwriteOldScreenshots( overwriteOldScreenshots->isChecked() );
  KomposeSettings::instance()->setScreenshotGrabDelay( screenshotGrabDelay->value() * 1000000 );

  KomposeSettings::instance()->setHighlightWindows( highlightWindows->isChecked() );
  KomposeSettings::instance()->setTintVirtDesks( tintVirtDesks->isChecked() );
  KomposeSettings::instance()->setTintVirtDesksColor( tintVirtDesksColor->color() );
  KomposeSettings::instance()->writeConfig();

  KDialogBase::slotApply();
}

void KomposePreferences::slotOk()
{
  slotApply();
  KDialogBase::slotOk();
}

