#include <kmsgbox.h>
#include <kiconloader.h>
#include "includes.h"
#include "misc.h"
#include "maindlg.h"

QList<KUser> users;
QList<MntEnt> mounts;
QFont rufont("-*-times-medium-r-*-*-*-120-*-*-*-*-*-*");

char tmp[1024];
QPixmap pic_user;
KConfig *config;
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
  if (!getenv("KDEDIR")) {
    puts(_("Enviroment variable KDEDIR doesn't defined.\nPlease define it and run kuser again"));
    exit(1);
  }
/*
  setlocale (LC_MESSAGES, "");
  strncpy(picspath, getenv("KDEDIR"), 200);
  strncat(picspath, "/lib/locale", 200);
  bindtextdomain("kuser", picspath);
  textdomain("kuser");
  strncpy(picspath, getenv("KDEDIR"), 200);
  strncat(picspath, "/lib/pic/kuser/", 200);
  strncpy(tmp, picspath, 200);
  strncat(tmp, "user.xpm", 200);
*/
  users.setAutoDelete(TRUE);
  QString pixdir = kapp->kdedir() + QString("/share/apps/kfax/pics/");
  pic_user.load((pixdir + "user.xpm").data());

  config = kapp->getConfig();

  init();

  rufont.setRawMode(TRUE);
  if (!rufont.exactMatch())
    puts("Doesn't found a font!\n");
}

void donemain() {
}

int main( int argc, char **argv )
{
  KApplication a(argc, argv, "kuser");

  if (getuid()) {
    KMsgBox::message(0, _("Error"), _("Only root allowed manage users."), KMsgBox::STOP);
    exit(1);
  }

  initmain();

  maindlg w("kuser");
  
  a.setMainWidget( &w );
  w.show();

  a.exec();
  donemain();
}
