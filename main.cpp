/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#include <qfont.h>

#include <kiconloader.h>
#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kdebug.h>

#include "kglobal_.h"
#include "misc.h"
#include "mainWidget.h"

static const char *description =
	I18N_NOOP("KDE User Editor");

KUserGlobals *kug = 0;

int main(int argc, char **argv) 
{
  
  KAboutData aboutData("kuser", I18N_NOOP("KUser"),
    _KU_VERSION, description, KAboutData::License_GPL, 
    "(c) 1997-2000, Denis Perchine\n(c) 2004, Szombathelyi György");
  aboutData.addAuthor("Denis Perchine", I18N_NOOP("kuser author"),
    "dyp@perchine.com", "http://www.perchine.com/dyp/");
  aboutData.addAuthor("Szombathelyi György", I18N_NOOP("kuser author"),
    "gyurco@freemail.hu");
  KCmdLineArgs::init(argc, argv, &aboutData);
  mainWidget *mw = 0;

  KApplication a;

  kapp->sharedConfig()->setGroup( "general" );
  kug = new KUserGlobals();
  kug->initCfg( kapp->sharedConfig()->readEntry( "connection", "default" ) );
  
  mw = new mainWidget("kuser");
  a.setMainWidget(mw);
  mw->setCaption(i18n("KDE User Manager"));
  mw->show();

  a.exec();

  kug->kcfg()->writeConfig();
  delete kug;

}
