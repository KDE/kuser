/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
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

#include "globals.h"

#include <kiconloader.h>
#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kdebug.h>

#include "ku_global.h"
#include "ku_misc.h"
#include "ku_mainwidget.h"

static const char *description =
	I18N_NOOP("KDE User Editor");

int main(int argc, char **argv) 
{
  
  KAboutData aboutData("kuser", 0, ki18n("KUser"),
    _KU_VERSION, ki18n(description), KAboutData::License_GPL, 
    ki18n("(c) 1997-2000, Denis Perchine\n(c) 2004, Szombathelyi György"));
  aboutData.addAuthor(ki18n("Denis Perchine"), ki18n("kuser author"),
    "dyp@perchine.com", "http://www.perchine.com/dyp/");
  aboutData.addAuthor(ki18n("Szombathelyi György"), ki18n("kuser author"),
    "gyurco@freemail.hu");
  KCmdLineArgs::init(argc, argv, &aboutData);
  KU_MainWidget *mw = 0;

  KApplication a;
  KGlobal::locale()->insertCatalog( QLatin1String( "libkldap" ));
  KConfigGroup group( KGlobal::config(), "general" );
  KU_Global::initCfg( group.readEntry( "connection", "default" ) );

  mw = new KU_MainWidget();
  mw->setCaption(i18n("KDE User Manager"));
  mw->show();

  a.exec();

  KU_Global::kcfg()->writeConfig();
}
