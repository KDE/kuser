#include "globals.h"

#include <unistd.h>

#include <qfont.h>

#include <kiconloader.h>
#include <kapp.h>
#include "kerror.h"
#include <kconfig.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "kglobal_.h"
#include "kerror.h"
#include "misc.h"
#include "mainWidget.h"

static const char *description = 
	I18N_NOOP("KDE User editor");

char tmp[1024];
KConfig *config;
KError *err;
bool changed = false;

KUserGlobals *kug = NULL;

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

int main(int argc, char **argv) {
  
  KAboutData aboutData("kuser", I18N_NOOP("KUser"),
    _KU_VERSION, description, KAboutData::License_GPL, 
    "(c) 1997-2000, Denis Perchine");
  aboutData.addAuthor("Denis Perchine", I18N_NOOP("kuser author"),
    "dyp@perchine.com", "http://www.perchine.com/dyp/");
  KCmdLineArgs::init(argc, argv, &aboutData);
  mainWidget *mw = NULL;

  KApplication a;

  initmain();

  if (getuid()) {
    err->addMsg(i18n("Only root is allowed to manage users."));
    err->display();
    exit(1);
  }

  KUserGlobals l_kug;
  kug = &l_kug;
  l_kug.init();

  mw = new mainWidget("kuser");
  
  a.setMainWidget(mw);
  mw->setCaption(i18n("KDE User Manager"));
  mw->show();

  a.exec();

  donemain();
}
