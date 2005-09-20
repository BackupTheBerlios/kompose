//
// C++ Implementation: komposesystray
//
// Description:
//
//
// Author: Hans Oischinger <hans.oischinger@kde-mail.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "komposesystray.h"

#include "komposeviewmanager.h"
#include "komposefullscreenwidget.h"
#include "komposesettings.h"
#include "komposeglobal.h"
#include "komposetaskmanager.h"

#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>

#include <kapplication.h>
#include <kaction.h>
#include <kpopupmenu.h>
#include <kiconeffect.h>
#include <kglobalsettings.h>

KomposeSysTray::KomposeSysTray(QWidget *parent, const char *name)
    : KSystemTray(parent, name)
{
  // Create Menu
  menu = contextMenu();
  move( -1000, -1000 );
  // Fill Menu
  KomposeGlobal::self()->getActShowWorldView()->plug(menu);
  KomposeGlobal::self()->getActShowVirtualDesktopView()->plug(menu);
  KomposeGlobal::self()->getActShowCurrentDesktopView()->plug(menu);
  menu->insertSeparator();
  KomposeGlobal::self()->getActPreferencesDialog()->plug(menu);
  KomposeGlobal::self()->getActConfigGlobalShortcuts()->plug(menu);
  KomposeGlobal::self()->getActAboutDlg()->plug(menu);

  slotConfigChanged();
  connect( KomposeGlobal::self(), SIGNAL(settingsChanged()), this, SLOT(slotConfigChanged()) );
}


KomposeSysTray::~KomposeSysTray()
{}

void KomposeSysTray::slotConfigChanged( )
{
  // set the icon here
  QPixmap iconPixmap = loadIcon("kompose");
  setPixmap(iconPixmap);
  icon = iconPixmap.convertToImage();
  currentDesktopChanged(KomposeTaskManager::self()->getCurrentDesktopNum());
}

void KomposeSysTray::mouseReleaseEvent (QMouseEvent * )
{}

void KomposeSysTray::mousePressEvent ( QMouseEvent * e )
{
  if ( !rect().contains( e->pos() ) )
    return;

  switch ( e->button() )
  {
  case LeftButton:
    KomposeViewManager::self()->createView( KomposeSettings::self()->view( KomposeSettings::EnumViewMode::Default ) );
    break;
  case MidButton:
    // fall through
  case RightButton:
    contextMenuAboutToShow( menu );
    menu->popup( e->globalPos() );
    break;
  default:
    // nothing
    break;
  }
}

void KomposeSysTray::currentDesktopChanged(int desktop)
{
  if (!KomposeSettings::self()->showDesktopNum())
    return;
  // update the icon to display the current desktop number
  // qDebug("Displaying current desktop number on the tray icon....\n");

  // copying from aKregator/src/trayicon.cpp
  // from KMSystemTray
  int oldW = pixmap()->size().width();
  int oldH = pixmap()->size().height();

  QString uStr=QString::number( desktop );
  QFont f=KGlobalSettings::generalFont();
  f.setBold(true);
  f.setItalic(true);
  float pointSize=f.pointSizeFloat();
  QFontMetrics fm(f);
  int w=fm.width(uStr);
  if( w > (oldW) )
  {
    pointSize *= float(oldW) / float(w);
    f.setPointSizeFloat(pointSize);
  }
  QPixmap pix(oldW, oldH);
  pix.fill(Qt::white);
  QPainter p(&pix);
  p.setFont(f);
  p.setPen(Qt::black);
  p.drawText(pix.rect(), Qt::AlignCenter, uStr);
  pix.setMask(pix.createHeuristicMask());
  QImage img=pix.convertToImage();
  // overlay
  QImage overlayImg=icon.copy();
  KIconEffect::overlay(overlayImg, img);
  QPixmap newIcon;
  newIcon.convertFromImage(overlayImg);
  setPixmap(newIcon);
}

#include "komposesystray.moc"
