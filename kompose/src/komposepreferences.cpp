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
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qhbox.h>

#include <kiconloader.h>
#include <kcolorbutton.h>

KomposePreferences::KomposePreferences()
    : KDialogBase(IconList, i18n(QString("Komposé Preferences").utf8()), Ok|Apply|Cancel, Ok)
{
  QFrame *page1 = addPage( i18n("Behaviour"), QString::null, DesktopIcon("winprops", KIcon::SizeMedium) );
  QFrame *page2 = addPage( i18n("Appearance"), QString::null, DesktopIcon("appearance", KIcon::SizeMedium) );

  QVBoxLayout *page1Layout = new QVBoxLayout( page1, 0, KDialog::spacingHint() );

  defaultViewBtnGroup = new QButtonGroup( 2, Horizontal, i18n("Default View"), page1 );
  defaultViewBtnGroup->setExclusive( true );
  QString defaultViewBtnGroupHelp = i18n("Determines which View should be started by default (e.g. when you click on the systray icon).");
  QWhatsThis::add( defaultViewBtnGroup, defaultViewBtnGroupHelp );
  QToolTip::add( defaultViewBtnGroup, defaultViewBtnGroupHelp );
  defaultViewWorld = new QCheckBox(i18n("Unordered View"), defaultViewBtnGroup );
  QString defaultViewWorldHelp = i18n("Fullscreen View that shows all tasks in no specific order");
  QWhatsThis::add( defaultViewWorld, defaultViewWorldHelp );
  QToolTip::add( defaultViewWorld, defaultViewWorldHelp );
  defaultViewVirtualDesks = new QCheckBox(i18n("Arranged by Virtual Desktops"), defaultViewBtnGroup );
  QString defaultViewVirtualDesksHelp = i18n("Fullscreen View that shows a representation of your virtual desktops\n and places the tasks inside.");
  QWhatsThis::add( defaultViewVirtualDesks, defaultViewVirtualDesksHelp );
  QToolTip::add( defaultViewVirtualDesks, defaultViewVirtualDesksHelp );
  page1Layout->addWidget(defaultViewBtnGroup);
  
  
  QGroupBox *screenshotsGroupBox = new QGroupBox( 3, Vertical, i18n("Screenshots"), page1 );
  
  passiveScreenshots = new QCheckBox(i18n("Passive Screenshots"), screenshotsGroupBox);
  QString passiveScreenshotsHelp = i18n(QString("Create a screenshot whenever you raise or active a window.\nWhen selected the amount the annoying popup-effect before every Komposé activation will be minimized to nearly zero.\nThe drawback is that the screenshots are not so recent and may not display the actual content.").utf8());
  QWhatsThis::add( passiveScreenshots, passiveScreenshotsHelp );
  QToolTip::add( passiveScreenshots, passiveScreenshotsHelp );

  onlyOneScreenshot = new QCheckBox(i18n("Only grab a screenshot when none exists"), screenshotsGroupBox);
  QString onlyOneScreenshotHelp = i18n("When disabled new screenshots will be taken whenever possible.\n Enabling will only create a screenshot once the application is first activated and will never update it.");
  QWhatsThis::add( onlyOneScreenshot, onlyOneScreenshotHelp );
  QToolTip::add( onlyOneScreenshot, onlyOneScreenshotHelp );

  QHBox *hLayScreenshotGrabDelay = new QHBox(screenshotsGroupBox);
  screenshotGrabDelay = new QSpinBox(0, 2000, 1, hLayScreenshotGrabDelay);
  QLabel *screenshotGrabDelayLabel = new QLabel(screenshotGrabDelay, i18n("Delay between Screenshots (ms)"), hLayScreenshotGrabDelay);
  QString screenshotGrabDelayHelp = i18n("Specifies the time to wait between the Activation of a window and the screenshot Taking.\nIncrease it when your windows need more time to draw themselves after activation.\nValues below 300ms are not recommended, but may work in some cases" );
  QWhatsThis::add( screenshotGrabDelay, screenshotGrabDelayHelp );
  QToolTip::add( screenshotGrabDelay, screenshotGrabDelayHelp );
  QWhatsThis::add( screenshotGrabDelayLabel, screenshotGrabDelayHelp );
  QToolTip::add( screenshotGrabDelayLabel, screenshotGrabDelayHelp );
  
  page1Layout->addWidget(screenshotsGroupBox);

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
  switch ( KomposeSettings::instance()->getDefaultView() )
  {
    case KOMPOSEDISPLAY_VIRTUALDESKS:
      defaultViewVirtualDesks->setChecked( true );
      break;
    case KOMPOSEDISPLAY_WORLD:
      defaultViewWorld->setChecked( true );
      break;
  }
  
  passiveScreenshots->setChecked( KomposeSettings::instance()->getPassiveScreenshots() );
  onlyOneScreenshot->setChecked( KomposeSettings::instance()->getOnlyOneScreenshot() );
  screenshotGrabDelay->setValue( KomposeSettings::instance()->getScreenshotGrabDelay() / 1000000 );

  highlightWindows->setChecked( KomposeSettings::instance()->getHighlightWindows() );
  tintVirtDesks->setChecked( KomposeSettings::instance()->getTintVirtDesks() );
  tintVirtDesksColor->setColor( KomposeSettings::instance()->getTintVirtDesksColor() );
  tintVirtDesksColor->setEnabled( KomposeSettings::instance()->getTintVirtDesks() );
}




void KomposePreferences::slotApply()
{
  KomposeSettings::instance()->setDefaultView( defaultViewBtnGroup->selectedId() );
  
  KomposeSettings::instance()->setPassiveScreenshots( passiveScreenshots->isChecked() );
  KomposeSettings::instance()->setOnlyOneScreenshot( onlyOneScreenshot->isChecked() );
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

