/***************************************************************************
 *   Copyright (C) 2004 by Hans Oischinger                                 *
 *   oisch@sourceforge.net                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "kompose.h"
#include "komposeglobal.h"

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kuniqueapplication.h>

static const char description[] =
  I18N_NOOP("A KDE Fullscreen TaskManager");

static const char version[] = "0.43";

static KCmdLineOptions options[] =
  {
    { "nosystray", I18N_NOOP( "Don't show the systray icon" ), 0 },
    { "singleshot", I18N_NOOP( "Display the default view and exit (non daemon mode)" ), 0 },
    KCmdLineLastOption
  };

int main(int argc, char **argv)
{
  KAboutData about("kompose", I18N_NOOP("Kompose"), version, description,
                   KAboutData::License_GPL, "(C) 2004 Hans Oischinger", "This software is still in a very early stage of development, so be patient :)", "http://developer.berlios.de/projects/kompose", "oisch@sourceforge.net");
  about.addAuthor( "Hans Oischinger", 0, "oisch@sourceforge.net" );
  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions( options );
  
  // KApplication app;
  KUniqueApplication app;
  Kompose *mainWin = 0;

  // no session.. just start up normally
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  
  if ( !args->isSet("systray") )
    KomposeGlobal::instance()->setHideSystray(true);
  if ( args->isSet("singleshot") )
  {
    KomposeGlobal::instance()->setHideSystray(true);
    KomposeGlobal::instance()->setSingleShot(true);
  }
  
  mainWin = new Kompose();
  //app.setMainWidget( mainWin );
  //mainWin->show();
  
  args->clear();

  // mainWin has WDestructiveClose flag by default, so it will delete itself.
  return app.exec();
}

