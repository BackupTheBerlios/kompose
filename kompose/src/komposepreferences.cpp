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
#include <qslider.h>
#include <qpushbutton.h>
#include <qgrid.h>

#include <kiconloader.h>
#include <kcolorbutton.h>
#include <kfontdialog.h> 

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
  defaultViewWorld = new QCheckBox(i18n("Ungrouped View"), defaultViewBtnGroup );
  QString defaultViewWorldHelp = i18n("Fullscreen View that shows all windows in no specific order");
  QWhatsThis::add( defaultViewWorld, defaultViewWorldHelp );
  QToolTip::add( defaultViewWorld, defaultViewWorldHelp );
  defaultViewVirtualDesks = new QCheckBox(i18n("Grouped  by Virtual Desktops"), defaultViewBtnGroup );
  QString defaultViewVirtualDesksHelp = i18n("Fullscreen View that shows a representation of your Virtual Desktops\n and places the windows inside.");
  QWhatsThis::add( defaultViewVirtualDesks, defaultViewVirtualDesksHelp );
  QToolTip::add( defaultViewVirtualDesks, defaultViewVirtualDesksHelp );
  page1Layout->addWidget(defaultViewBtnGroup);


  QGroupBox *screenshotsGroupBox = new QGroupBox( 3, Vertical, i18n("Screenshots"), page1 );

  passiveScreenshots = new QCheckBox(i18n("Passive Screenshots"), screenshotsGroupBox);
  QString passiveScreenshotsHelp = i18n(QString("Create a screenshot whenever you raise or active a window.\nWhen selected the amount the annoying popup-effect before every Komposé activation will be minimized to nearly zero.\nThe drawback is that the screenshots are not so recent and may not display the actual content.").utf8());
  QWhatsThis::add( passiveScreenshots, passiveScreenshotsHelp );
  QToolTip::add( passiveScreenshots, passiveScreenshotsHelp );

  //   onlyOneScreenshot = new QCheckBox(i18n("Only grab a screenshot when none exists"), screenshotsGroupBox);
  //   QString onlyOneScreenshotHelp = i18n("When disabled new screenshots will be taken whenever possible.\n Enabling will only create a screenshot once the application is first activated and will never update it.");
  //   QWhatsThis::add( onlyOneScreenshot, onlyOneScreenshotHelp );
  //   QToolTip::add( onlyOneScreenshot, onlyOneScreenshotHelp );

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
  tintVirtDesks = new QCheckBox(i18n("Tint Virtual Desktop widgets: "), page2);
  tintVirtDesksColor = new KColorButton(Qt::blue, page2);
  QString tintVirtDesksHelp = i18n("Colorize the transparent background of the Virtual Desktop widgets" );
  QWhatsThis::add( tintVirtDesks, tintVirtDesksHelp );
  QToolTip::add( tintVirtDesks, tintVirtDesksHelp );
  QWhatsThis::add( tintVirtDesksColor, tintVirtDesksHelp );
  QToolTip::add( tintVirtDesksColor, tintVirtDesksHelp );
  hLayTintVirtDesks->addWidget(tintVirtDesks);
  hLayTintVirtDesks->addWidget(tintVirtDesksColor);
  hLayTintVirtDesks->addStretch();
  page2Layout->addLayout(hLayTintVirtDesks);
  connect( tintVirtDesks, SIGNAL(toggled(bool)), tintVirtDesksColor, SLOT(setEnabled(bool)) );

  imageEffects = new QCheckBox(i18n("Enable image effects"), page2);
  QString imageEffectsHelp = i18n("Lighten windows when the mouse moves over it or gray out minimized windows.\nDepending on your system specs this can be a bit slower." );
  QWhatsThis::add( imageEffects, imageEffectsHelp );
  QToolTip::add( imageEffects, imageEffectsHelp );
  page2Layout->addWidget(imageEffects);

  
  QGroupBox *windowTitleGroupBox = new QGroupBox( 3, Vertical, i18n("Window titles"), page2 );
  
  QHBox *hBoxWindowTitles = new QHBox(windowTitleGroupBox);
  showWindowTitles = new QCheckBox(i18n("Show Window titles"), hBoxWindowTitles);
  windowTitleFontBtn = new QPushButton(i18n("Select Font"), hBoxWindowTitles);
  QString showWindowTitlesHelp = i18n( QString("Display the name of every window in Komposé") );
  QWhatsThis::add( showWindowTitles, showWindowTitlesHelp );
  QToolTip::add( showWindowTitles, showWindowTitlesHelp );
  connect( showWindowTitles, SIGNAL(toggled(bool)), windowTitleFontBtn, SLOT(setEnabled(bool)) );
  connect( windowTitleFontBtn, SIGNAL(clicked()), this, SLOT(showFontDialog()) );
  
  QGrid *gridWindowTitlesColor = new QGrid(2, windowTitleGroupBox);
  // windowTitleFontColorLabel = new QLabel(windowTitleFontColor, i18n("Text Color: "), gridWindowTitlesColor); // FIXME: How to link to a buddy that doesn't yet exist?
  windowTitleFontColorLabel = new QLabel(i18n("Text Color: "), gridWindowTitlesColor);
  windowTitleFontColor = new KColorButton(Qt::black, gridWindowTitlesColor);
  connect( showWindowTitles, SIGNAL(toggled(bool)), windowTitleFontColorLabel, SLOT(setEnabled(bool)) );
  connect( showWindowTitles, SIGNAL(toggled(bool)), windowTitleFontColor, SLOT(setEnabled(bool)) );
  showWindowTitleShadow = new QCheckBox(i18n("Shadow Color: "), gridWindowTitlesColor);
  windowTitleFontShadowColor = new KColorButton(Qt::lightGray, gridWindowTitlesColor);
  connect( showWindowTitles, SIGNAL(toggled(bool)), showWindowTitleShadow, SLOT(setEnabled(bool)) );
  connect( showWindowTitles, SIGNAL(toggled(bool)), windowTitleFontShadowColor, SLOT(setEnabled(bool)) );
  connect( showWindowTitleShadow, SIGNAL(toggled(bool)), windowTitleFontShadowColor, SLOT(setEnabled(bool)) );
  
  page2Layout->addWidget(windowTitleGroupBox);

  
  QGroupBox *iconGroupBox = new QGroupBox( 3, Vertical, i18n("Task Icons"), page2 );
  showIcons = new QCheckBox(i18n("Show Icons"), iconGroupBox);
  iconSize = new QSlider(0, 3, 1, 0, Qt::Horizontal, iconGroupBox);
  iconSizeDescription = new QLabel( iconSize, "", iconGroupBox);
  connect( iconSize, SIGNAL( sliderMoved(int) ), this, SLOT( updateIconSliderDesc(int) ) );
  connect( iconSize, SIGNAL( valueChanged(int) ), this, SLOT( updateIconSliderDesc(int) ) );
  connect( showIcons, SIGNAL( toggled(bool) ), iconSize, SLOT( setEnabled(bool) ) );
  connect( showIcons, SIGNAL( toggled(bool) ), iconSizeDescription, SLOT( setEnabled(bool) ) );
  page2Layout->addWidget(iconGroupBox);

  page2Layout->insertStretch(-1);

  fillPages();
}


KomposePreferences::~KomposePreferences()
{}

void KomposePreferences::updateIconSliderDesc( int val )
{
  switch( val )
  {
  case 0:
    iconSizeDescription->setText( i18n("16x16") );
    break;
  case 1:
    iconSizeDescription->setText( i18n("32x32") );
    break;
  case 2:
    iconSizeDescription->setText( i18n("64x64") );
    break;
  case 3:
    iconSizeDescription->setText( i18n("Don't scale, show as they are (preferably bigger)") );
    break;
  }
}

void KomposePreferences::showFontDialog()
{
  int result = KFontDialog::getFont( *windowTitleFont );
//   if ( result == KFontDialog::Accepted )
}

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
  screenshotGrabDelay->setValue( KomposeSettings::instance()->getScreenshotGrabDelay() / 1000000 );

  imageEffects->setChecked( KomposeSettings::instance()->getImageEffects() );
  tintVirtDesks->setChecked( KomposeSettings::instance()->getTintVirtDesks() );
  tintVirtDesksColor->setColor( KomposeSettings::instance()->getTintVirtDesksColor() );
  tintVirtDesksColor->setEnabled( KomposeSettings::instance()->getTintVirtDesks() );

  showWindowTitles->setChecked( KomposeSettings::instance()->getShowWindowTitles() );
  windowTitleFont = new QFont(KomposeSettings::instance()->getWindowTitleFont());
  windowTitleFontColor->setColor( KomposeSettings::instance()->getWindowTitleFontColor() );
  windowTitleFontShadowColor->setColor( KomposeSettings::instance()->getWindowTitleFontShadowColor() );
  showWindowTitleShadow->setChecked( KomposeSettings::instance()->getShowWindowTitleShadow() );
  // Disable dependant widgets
  windowTitleFontBtn->setEnabled( KomposeSettings::instance()->getShowWindowTitles() );
  windowTitleFontColorLabel->setEnabled( KomposeSettings::instance()->getShowWindowTitles() );
  windowTitleFontColor->setEnabled( KomposeSettings::instance()->getShowWindowTitles() );
  showWindowTitleShadow->setEnabled( KomposeSettings::instance()->getShowWindowTitles() );
  windowTitleFontShadowColor->setEnabled( KomposeSettings::instance()->getShowWindowTitles() || KomposeSettings::instance()->getShowWindowTitleShadow() );
    
  
  showIcons->setChecked( KomposeSettings::instance()->getShowIcons() );
  iconSize->setValue( KomposeSettings::instance()->getIconSize() );
  iconSize->setEnabled( showIcons->isChecked() );
  iconSizeDescription->setEnabled( showIcons->isChecked() );
  updateIconSliderDesc( iconSize->value() );
}




void KomposePreferences::slotApply()
{
  KomposeSettings::instance()->setDefaultView( defaultViewBtnGroup->selectedId() );

  KomposeSettings::instance()->setPassiveScreenshots( passiveScreenshots->isChecked() );
  KomposeSettings::instance()->setScreenshotGrabDelay( screenshotGrabDelay->value() * 1000000 );

  KomposeSettings::instance()->setImageEffects( imageEffects->isChecked() );
  KomposeSettings::instance()->setTintVirtDesks( tintVirtDesks->isChecked() );
  KomposeSettings::instance()->setTintVirtDesksColor( tintVirtDesksColor->color() );

  KomposeSettings::instance()->setShowWindowTitles( showWindowTitles->isChecked() );
  KomposeSettings::instance()->setWindowTitleFont( *windowTitleFont );
  KomposeSettings::instance()->setWindowTitleFontColor( windowTitleFontColor->color() );
  KomposeSettings::instance()->setWindowTitleFontShadowColor( windowTitleFontShadowColor->color() );
  KomposeSettings::instance()->setShowWindowTitleShadow( showWindowTitleShadow->isChecked() );

  KomposeSettings::instance()->setShowIcons( showIcons->isChecked() );
  KomposeSettings::instance()->setIconSize( iconSize->value() );

  KomposeSettings::instance()->writeConfig();

  KDialogBase::slotApply();
}

void KomposePreferences::slotOk()
{
  slotApply();
  KDialogBase::slotOk();
}


#include "komposepreferences.moc"
