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
#include <qvgroupbox.h>
#include <qradiobutton.h>

#include <kiconloader.h>
#include <kcolorbutton.h>
#include <kfontdialog.h>

KomposePreferences::KomposePreferences()
    : KDialogBase(IconList, i18n(QString::fromUtf8("Komposé Preferences").utf8()), Ok|Apply|Cancel, Ok)
{
  // FIXME: this is the biggest constructor I've EVER written!
  // How about Qt Designer?!
  
  QFrame *page1 = addPage( i18n("Behavior"), QString::null, DesktopIcon("winprops", KIcon::SizeMedium) );
  QFrame *page2 = addPage( i18n("Appearance"), QString::null, DesktopIcon("appearance", KIcon::SizeMedium) );
  QFrame *page3 = addPage( i18n("Layouts"), QString::null, DesktopIcon("window_list", KIcon::SizeMedium) );

  QVBoxLayout *page1Layout = new QVBoxLayout( page1, 0, KDialog::spacingHint() );

  defaultViewBtnGroup = new QButtonGroup( 2, Horizontal, i18n("Default Layout"), page1 );
  defaultViewBtnGroup->setExclusive( true );
  QString defaultViewBtnGroupHelp = i18n("Determines which view should be started by default (e.g. when you click on the systray icon).");
  QWhatsThis::add( defaultViewBtnGroup, defaultViewBtnGroupHelp );
  QToolTip::add( defaultViewBtnGroup, defaultViewBtnGroupHelp );
  defaultViewWorld = new QRadioButton(i18n("Ungrouped"), defaultViewBtnGroup );
  QString defaultViewWorldHelp = i18n("Fullscreen layout that shows all windows in no specific order");
  QWhatsThis::add( defaultViewWorld, defaultViewWorldHelp );
  QToolTip::add( defaultViewWorld, defaultViewWorldHelp );
  defaultViewVirtualDesks = new QRadioButton(i18n("Grouped by virtual desktops"), defaultViewBtnGroup );
  QString defaultViewVirtualDesksHelp = i18n("Fullscreen layout that shows a representation of your virtual desktops\n and places the windows inside.");
  QWhatsThis::add( defaultViewVirtualDesks, defaultViewVirtualDesksHelp );
  QToolTip::add( defaultViewVirtualDesks, defaultViewVirtualDesksHelp );
  page1Layout->addWidget(defaultViewBtnGroup);


  QGroupBox *screenshotsGroupBox = new QGroupBox( 3, Vertical, i18n("Screenshots"), page1 );

  passiveScreenshots = new QCheckBox(i18n("Passive screenshots"), screenshotsGroupBox);
  QString passiveScreenshotsHelp = i18n(QString::fromUtf8("Create a screenshot whenever you raise or active a window.\nWhen selected the amount the annoying popup-effect before every Komposé activation will be minimized to nearly zero.\nThe drawback is that the screenshots are not so recent and may not display the actual content.").utf8());
  QWhatsThis::add( passiveScreenshots, passiveScreenshotsHelp );
  QToolTip::add( passiveScreenshots, passiveScreenshotsHelp );

  QHBox *hLayScreenshotGrabDelay = new QHBox(screenshotsGroupBox);
  QLabel *screenshotGrabDelayLabel = new QLabel(i18n("Delay between screenshots (ms):"), hLayScreenshotGrabDelay);
  screenshotGrabDelay = new QSpinBox(0, 999, 1, hLayScreenshotGrabDelay);
  screenshotGrabDelayLabel->setBuddy(screenshotGrabDelay);
  QString screenshotGrabDelayHelp = i18n("Specifies the time to wait between the activation of a window and the screenshot taking.\nIncrease it when your windows need more time to draw themselves after activation.\nValues below 300ms are not recommended, but may work in some cases." );
  QWhatsThis::add( screenshotGrabDelay, screenshotGrabDelayHelp );
  QToolTip::add( screenshotGrabDelay, screenshotGrabDelayHelp );
  QWhatsThis::add( screenshotGrabDelayLabel, screenshotGrabDelayHelp );
  QToolTip::add( screenshotGrabDelayLabel, screenshotGrabDelayHelp );

  cacheScaledPixmaps = new QCheckBox(i18n("Cache scaled Screenshots"), screenshotsGroupBox);
  QString cacheScaledPixmapsHelp = i18n("This may avoid some scaling operations to be called repeatedly when Komposé has been displayed before and the screenshot's size didn't change.\nIt will however increase memory usage quite a bit.");
  QWhatsThis::add( cacheScaledPixmaps, cacheScaledPixmapsHelp );
  QToolTip::add( cacheScaledPixmaps, cacheScaledPixmapsHelp );

  page1Layout->addWidget(screenshotsGroupBox);


  // Autolock
  QGroupBox* autoLockGroup = new QGroupBox( i18n("Autoactivate when mouse moves into"), page1 );
  autoLockGroup->setColumnLayout( 0, Qt::Vertical );
  QBoxLayout* autoLockLayout = new QVBoxLayout( autoLockGroup->layout(), KDialog::spacingHint() );
  m_topLeftCorner = new QCheckBox( i18n("Top-left corner"), autoLockGroup);
  autoLockLayout->addWidget( m_topLeftCorner );
  m_topRightCorner = new QCheckBox( i18n("Top-right corner"), autoLockGroup );
  autoLockLayout->addWidget( m_topRightCorner );
  m_bottomLeftCorner = new QCheckBox( i18n("Bottom-left corner"), autoLockGroup );
  autoLockLayout->addWidget( m_bottomLeftCorner );
  m_bottomRightCorner = new QCheckBox( i18n("Bottom-right corner"), autoLockGroup );
  autoLockLayout->addWidget( m_bottomRightCorner );

  page1Layout->addWidget(autoLockGroup);

  page1Layout->insertStretch(-1);



  QVBoxLayout *page2Layout = new QVBoxLayout( page2, 0, KDialog::spacingHint() );

  imageEffects = new QCheckBox(i18n("Enable image effects"), page2);
  QString imageEffectsHelp = i18n("Lighten windows when the mouse moves over it or gray out minimized windows.\nDepending on your system specs this can be a bit slower." );
  QWhatsThis::add( imageEffects, imageEffectsHelp );
  QToolTip::add( imageEffects, imageEffectsHelp );
  page2Layout->addWidget(imageEffects);


  QGroupBox *windowTitleGroupBox = new QGroupBox( 3, Vertical, i18n("Window Titles"), page2 );

  QHBox *hBoxWindowTitles = new QHBox(windowTitleGroupBox);
  showWindowTitles = new QCheckBox(i18n("Show window titles"), hBoxWindowTitles);
  windowTitleFontBtn = new QPushButton(i18n("Select Font..."), hBoxWindowTitles);
  QString showWindowTitlesHelp = i18n( QString::fromUtf8("Display the name of every window in Komposé") );
  QWhatsThis::add( showWindowTitles, showWindowTitlesHelp );
  QToolTip::add( showWindowTitles, showWindowTitlesHelp );
  connect( showWindowTitles, SIGNAL(toggled(bool)), windowTitleFontBtn, SLOT(setEnabled(bool)) );
  connect( windowTitleFontBtn, SIGNAL(clicked()), this, SLOT(showWindowTitleFontDialog()) );

  QGrid *gridWindowTitlesColor = new QGrid(2, windowTitleGroupBox);
  // windowTitleFontColorLabel = new QLabel(windowTitleFontColor, i18n("Text color: "), gridWindowTitlesColor); // FIXME: How to link to a buddy that doesn't yet exist?
  windowTitleFontColorLabel = new QLabel(i18n("Text color:"), gridWindowTitlesColor);
  windowTitleFontColor = new KColorButton(Qt::black, gridWindowTitlesColor);
  connect( showWindowTitles, SIGNAL(toggled(bool)), windowTitleFontColorLabel, SLOT(setEnabled(bool)) );
  connect( showWindowTitles, SIGNAL(toggled(bool)), windowTitleFontColor, SLOT(setEnabled(bool)) );
  showWindowTitleShadow = new QCheckBox(i18n("Shadow color:"), gridWindowTitlesColor);
  windowTitleFontShadowColor = new KColorButton(Qt::lightGray, gridWindowTitlesColor);
  connect( showWindowTitles, SIGNAL(toggled(bool)), showWindowTitleShadow, SLOT(setEnabled(bool)) );
  connect( showWindowTitles, SIGNAL(toggled(bool)), windowTitleFontShadowColor, SLOT(setEnabled(bool)) );
  connect( showWindowTitleShadow, SIGNAL(toggled(bool)), windowTitleFontShadowColor, SLOT(setEnabled(bool)) );

  page2Layout->addWidget(windowTitleGroupBox);



  QGroupBox *iconGroupBox = new QGroupBox( 3, Vertical, i18n("Task Icons"), page2 );
  showIcons = new QCheckBox(i18n("Show icons"), iconGroupBox);
  iconSize = new QSlider(0, 3, 1, 0, Qt::Horizontal, iconGroupBox);
  iconSizeDescription = new QLabel( iconSize, "", iconGroupBox);
  connect( iconSize, SIGNAL( sliderMoved(int) ), this, SLOT( updateIconSliderDesc(int) ) );
  connect( iconSize, SIGNAL( valueChanged(int) ), this, SLOT( updateIconSliderDesc(int) ) );
  connect( showIcons, SIGNAL( toggled(bool) ), iconSize, SLOT( setEnabled(bool) ) );
  connect( showIcons, SIGNAL( toggled(bool) ), iconSizeDescription, SLOT( setEnabled(bool) ) );
  page2Layout->addWidget(iconGroupBox);

  page2Layout->insertStretch(-1);



  QVBoxLayout *page3Layout = new QVBoxLayout( page3, 0, KDialog::spacingHint() );

  QVGroupBox *virtDesksLayoutGroupBox = new QVGroupBox( i18n("Grouped by Virtual Desktops"), page3 );
  dynamicVirtDeskLayout = new QCheckBox(i18n("Layout empty virtual desktops minimized"), virtDesksLayoutGroupBox );
  QString dynamicVirtDeskLayoutHelp = i18n("Check this if you want empty virtual desktops to take less space on the screen.\nUncheck it if you want them to be arranged statically, each of the same size.");
  QWhatsThis::add( dynamicVirtDeskLayout, dynamicVirtDeskLayoutHelp );
  QToolTip::add( dynamicVirtDeskLayout, dynamicVirtDeskLayoutHelp );

  QGrid *desktopColorsGroupBox = new QGrid( 2, virtDesksLayoutGroupBox );
  desktopColorsGroupBox->setSpacing( 4 );
  tintVirtDesks = new QCheckBox(i18n("Tint virtual desktop widgets:"), desktopColorsGroupBox);
  tintVirtDesksColor = new KColorButton(Qt::blue, desktopColorsGroupBox);
  QString tintVirtDesksHelp = i18n("Colorize the transparent background of the virtual desktop widgets" );
  QWhatsThis::add( tintVirtDesks, tintVirtDesksHelp );
  QToolTip::add( tintVirtDesks, tintVirtDesksHelp );
  QWhatsThis::add( tintVirtDesksColor, tintVirtDesksHelp );
  QToolTip::add( tintVirtDesksColor, tintVirtDesksHelp );
  connect( tintVirtDesks, SIGNAL(toggled(bool)), tintVirtDesksColor, SLOT(setEnabled(bool)) );
  desktopTitleFontColorLabel = new QLabel(i18n("Desktop frame color:"), desktopColorsGroupBox);
  desktopTitleFontColor = new KColorButton(Qt::black, desktopColorsGroupBox);
  desktopTitleFontHighlightColorLabel = new QLabel(i18n("Desktop frame highlight color:"), desktopColorsGroupBox);
  desktopTitleFontHighlightColor = new KColorButton(Qt::black, desktopColorsGroupBox);

  page3Layout->addWidget(virtDesksLayoutGroupBox);

  desktopTitleFontBtn = new QPushButton(i18n("Select Desktop Names Font..."), virtDesksLayoutGroupBox);
  connect( desktopTitleFontBtn, SIGNAL(clicked()), this, SLOT(showDesktopTitleFontDialog()) );

  page3Layout->insertStretch(-1);


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

void KomposePreferences::showWindowTitleFontDialog()
{
  KFontDialog::getFont( *windowTitleFont );
  // if ( result == KFontDialog::Accepted )
}

void KomposePreferences::showDesktopTitleFontDialog()
{
  KFontDialog::getFont( *desktopTitleFont );
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
  cacheScaledPixmaps->setChecked( KomposeSettings::instance()->getCacheScaledPixmaps() );
  dynamicVirtDeskLayout->setChecked( KomposeSettings::instance()->getDynamicVirtDeskLayout() );

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

  desktopTitleFont = new QFont(KomposeSettings::instance()->getDesktopTitleFont());
  desktopTitleFontColor->setColor( KomposeSettings::instance()->getDesktopTitleFontColor() );
  desktopTitleFontHighlightColor->setColor( KomposeSettings::instance()->getDesktopTitleFontHighlightColor() );

  showIcons->setChecked( KomposeSettings::instance()->getShowIcons() );
  iconSize->setValue( KomposeSettings::instance()->getIconSize() );
  iconSize->setEnabled( showIcons->isChecked() );
  iconSizeDescription->setEnabled( showIcons->isChecked() );
  updateIconSliderDesc( iconSize->value() );
  
  m_topLeftCorner->setChecked( KomposeSettings::instance()->getActivateOnTopLeftCorner() );
  m_bottomLeftCorner->setChecked( KomposeSettings::instance()->getActivateOnBottomLeftCorner() );
  m_topRightCorner->setChecked( KomposeSettings::instance()->getActivateOnTopRightCorner() );
  m_bottomRightCorner->setChecked( KomposeSettings::instance()->getActivateOnBottomRightCorner() );
}




void KomposePreferences::slotApply()
{
  KomposeSettings::instance()->setDefaultView( defaultViewBtnGroup->selectedId() );

  KomposeSettings::instance()->setPassiveScreenshots( passiveScreenshots->isChecked() );
  KomposeSettings::instance()->setScreenshotGrabDelay( screenshotGrabDelay->value() * 1000000 );
  KomposeSettings::instance()->setCacheScaledPixmaps( cacheScaledPixmaps->isChecked() );
  KomposeSettings::instance()->setDynamicVirtDeskLayout( dynamicVirtDeskLayout->isChecked() );
  KomposeSettings::instance()->setImageEffects( imageEffects->isChecked() );
  KomposeSettings::instance()->setTintVirtDesks( tintVirtDesks->isChecked() );
  KomposeSettings::instance()->setTintVirtDesksColor( tintVirtDesksColor->color() );

  KomposeSettings::instance()->setShowWindowTitles( showWindowTitles->isChecked() );
  KomposeSettings::instance()->setWindowTitleFont( *windowTitleFont );
  KomposeSettings::instance()->setWindowTitleFontColor( windowTitleFontColor->color() );
  KomposeSettings::instance()->setWindowTitleFontShadowColor( windowTitleFontShadowColor->color() );
  KomposeSettings::instance()->setShowWindowTitleShadow( showWindowTitleShadow->isChecked() );

  KomposeSettings::instance()->setDesktopTitleFont( *desktopTitleFont );
  KomposeSettings::instance()->setDesktopTitleFontColor( desktopTitleFontColor->color() );
  KomposeSettings::instance()->setDesktopTitleFontHighlightColor( desktopTitleFontHighlightColor->color() );

  KomposeSettings::instance()->setShowIcons( showIcons->isChecked() );
  KomposeSettings::instance()->setIconSize( iconSize->value() );

  KomposeSettings::instance()->setActivateOnTopLeftCorner( m_topLeftCorner->isChecked() );
  KomposeSettings::instance()->setActivateOnTopRighCorner( m_topRightCorner->isChecked() );
  KomposeSettings::instance()->setActivateOnBottomLeftCorner( m_bottomLeftCorner->isChecked() );
  KomposeSettings::instance()->setActivateOnBottomRightCorner( m_bottomRightCorner->isChecked() );
  
  KomposeSettings::instance()->writeConfig();

  KDialogBase::slotApply();
}

void KomposePreferences::slotOk()
{
  slotApply();
  KDialogBase::slotOk();
}


#include "komposepreferences.moc"
