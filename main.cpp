#include <kiconloader.h>
#include <kapp.h>
#include "kerror.h"
#include <kconfig.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <ktmainwindow.h>
#include <qfont.h>
#include <unistd.h>
#include "globals.h"
#include "misc.h"
#include "maindlg.h"
#include "mainWidget.h"

#include <kaboutdata.h>

static const char *description = 
	I18N_NOOP("KDE User editor");

char tmp[1024];
KConfig *config;
KError *err;
bool changed = false;

#ifdef _KU_QUOTA
int is_quota = 1;
#else
int is_quota = 0;
#endif

#ifdef _KU_SHADOW
int is_shadow = 1;
#else
int is_shadow = 0;
#endif

void initmain() {
  config = kapp->config();
  err = new KError();
}

void donemain() {
  delete err;
}

mainDlg *md = NULL;

int main(int argc, char **argv) {
  
  KAboutData aboutData( "kuser", I18N_NOOP("KUser"), 
    _KU_VERSION, description, KAboutData::GPL, 
    "(c) 1999-2000, Denis Pershin");
  aboutData.addAuthor("Denis Pershin",0, "dyp@inetlab.com");
  KCmdLineArgs::init( argc, argv, &aboutData );
//  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
  KApplication app;
  mainWidget *mw = NULL;

  KApplication a;

  initmain();

  if (getuid()) {
    err->addMsg(i18n("Only root is allowed to manage users."), STOP);
    err->display();
    exit(1);
  }

  mw = new mainWidget("kuser");
  
  a.setMainWidget(mw);
  mw->setCaption(i18n("KDE User Manager"));
  mw->show();

  a.exec();

  donemain();
}
