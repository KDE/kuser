/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Maintained by Adriaan de Groot <groot@kde.org>
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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include "globals.h"

#include <unistd.h>

#include <qfont.h>

#include <kiconloader.h>
#include <kapplication.h>
#include "kerror.h"
#include <kconfig.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "kglobal_.h"
#include "misc.h"
#include "mainWidget.h"

static const char description[] =
	I18N_NOOP("KDE User Editor");

char tmp[1024];

/** Global error config object, is this a good idea ? */
KConfig *config;
/** Global error queue object, is this a good idea ? */
KError *err;
bool changed = false;

KUserGlobals *kug = 0;

// Why not using bool here ?
#ifdef _KU_QUOTA
int is_quota = 1;
#else
int is_quota = 0;
#endif

// Why not using bool here ?
#ifdef HAVE_SHADOW
int is_shadow = 1;
#else
int is_shadow = 0;
#endif

/** Initialize the global config and error queue objects */
void initmain() {
  config = kapp->config();
  err = new KError();
}

/** Delete the globar error queue object. */
void donemain() {
  delete err;
}

/** Main function, where we start rolling. */
int main(int argc, char **argv) {
  
  KAboutData aboutData("kuser", I18N_NOOP("KUser"),
    _KU_VERSION, description, KAboutData::License_GPL, 
    "(c) 1997-2000, Denis Perchine");
  aboutData.addAuthor("Denis Perchine", I18N_NOOP("kuser author"),
    "dyp@perchine.com", "http://www.perchine.com/dyp/");
  KCmdLineArgs::init(argc, argv, &aboutData);
  
  // Pointer to our main windget, initialized to 0 by now.
  mainWidget *mw = 0;

  // Our KApplication.
  KApplication a;

  // Initializate the config and error queue objects.
  initmain();

  // Check if our User IDentification is different than 0 (root).
  if (getuid()) {
    // If we are not running as root, show a message and quit.
    err->addMsg(i18n("Only root is allowed to manage users."));
    err->display();
    exit(1);
  }

  KUserGlobals l_kug;
  kug = &l_kug;
  l_kug.init();

  // Create the main widget.
  mw = new mainWidget("kuser");
  
  a.setMainWidget(mw);
  mw->setCaption(i18n("KDE User Manager"));
  mw->show();

  a.exec();

  donemain();
}
