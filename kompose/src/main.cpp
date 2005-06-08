/***************************************************************************
 *   Copyright (C) 2004 by Hans Oischinger                                 *
 *   hans.oischinger@kde-mail.net                                                 *
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

#include <stdlib.h>

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>


static const char description[] = 
  I18N_NOOP("A fullscreen task manager for KDE");

static const char version[] = "0.5.2 beta1";

static KCmdLineOptions options[] =
  {
    { "nosystray", I18N_NOOP( "Do not show the systray icon" ), 0 },
    { "singleshot", I18N_NOOP( "Display the default view and exit (non daemon mode)" ), 0 },
    KCmdLineLastOption
  };

void myMessageOutput( QtMsgType type, const char *msg )
{
  return;
  switch ( type )
  {
  case QtDebugMsg:
    fprintf( stderr, "Debug: %s\n", msg );
    break;
  case QtWarningMsg:
    fprintf( stderr, "Warning: %s\n", msg );
    break;
  case QtFatalMsg:
    fprintf( stderr, "Fatal: %s\n", msg );
    abort();                    // deliberately core dump
  }
}

int main(int argc, char **argv)
{
//   qInstallMsgHandler( myMessageOutput );
  
  KAboutData about("kompose", I18N_NOOP("Kompose"), version, description,
                   KAboutData::License_GPL, "(C) 2005 Hans Oischinger", "", "http://developer.berlios.de/projects/kompose", "hans.oischinger@kde-mail.net");
  about.addAuthor( "Hans Oischinger", 0, "hans.oischinger@kde-mail.net" );
  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions( options );

  Kompose *app = new Kompose();

  // no session.. just start up normally
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  
  if ( !args->isSet("systray") )
  {
    KomposeGlobal::instance()->setHideSystray(true);
  }
  if ( args->isSet("singleshot") )
  {
    KomposeGlobal::instance()->setHideSystray(true);
    KomposeGlobal::instance()->setSingleShot(true);
  }

  KomposeGlobal::instance()->initGui();
  
  //app.setMainWidget( mainWin );

  args->clear();

  // mainWin has WDestructiveClose flag by default, so it will delete itself.
  return app->exec();
}

