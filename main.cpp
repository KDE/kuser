#include <kmsgbox.h>
#include <kiconloader.h>
#include <kapp.h>
#include <kconfig.h>
#include <ktopwidget.h>
#include <qfont.h>
#include <unistd.h>
#include "misc.h"
#include "maindlg.h"

QFont rufont("-*-times-medium-r-*-*-*-120-*-*-*-*-*-*");

char tmp[1024];
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
  config = kapp->getConfig();

  rufont.setRawMode(TRUE);
  if (!rufont.exactMatch())
    puts("Doesn't found a font!\n");
}

void donemain() {
}
mainDlg *md = NULL;

int main( int argc, char **argv )
{
  KApplication a(argc, argv, "kuser");
  if (getuid()) {
    KMsgBox::message(0, _("Error"), _("Only root is allowed to manage users."), KMsgBox::STOP);
    exit(1);
  }

  initmain();

  md = new mainDlg("kuser");
  md->init();
  
  a.setMainWidget(md);
  md->show();

  a.exec();

  delete md;
  donemain();
}
