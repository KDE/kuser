#include "includes.h"
#include "misc.h"
#include "maindlg.h"

QList<KUser> users;
QList<MntEnt> mounts;
QFont rufont("-*-times-medium-r-*-*-*-120-*-*-*-*-*-*");

char tmp[1024];
QPixmap *pic_user;
KConfig *config;
bool changed = false;

#ifdef _XU_QUOTA
int is_quota = 1;
#else
int is_quota = 0;
#endif

#ifdef _XU_SHADOW
int is_shadow = 1;
#else
int is_shadow = 0;
#endif

char picspath[200];

void initmain() {
  if (!getenv("KDEDIR")) {
    puts(_("Enviroment variable KDEDIR doesn't defined.\nPlease define it and run kuser again"));
    exit(1);
  }
  setlocale (LC_MESSAGES, "");
  strncpy(picspath, getenv("KDEDIR"), 200);
  strncat(picspath, "/lib/locale", 200);
  bindtextdomain("kuser", picspath);
  textdomain("kuser");

  strncpy(picspath, getenv("KDEDIR"), 200);
  strncat(picspath, "/lib/pic/kuser/", 200);
  users.setAutoDelete(TRUE);
  strncpy(tmp, picspath, 200);
  strncat(tmp, "user.bmp", 200);
  pic_user = new QPixmap(tmp);

  config = kapp->getConfig();

//  QFont *tmpf = new QFont();
//  tpmf->

  init();

  rufont.setRawMode(TRUE);
  if (!rufont.exactMatch())
    puts("Doesn't found a font!\n");
}

void donemain() {
puts("Doing");
//  delete pic_user;
puts("Done");
}

int main( int argc, char **argv )
{
  KApplication a(argc, argv, "kuser");

  initmain();

  if (getuid()) {
    QMessageBox::message(_("Error"), _("Only root allowed manage users."),"Ok");
    exit(1);
  }

  maindlg w(NULL, "kuser");

  a.setMainWidget( &w );
  w.show();

  int res=a.exec();
  donemain();
}
