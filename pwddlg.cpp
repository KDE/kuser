
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <config.h>

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <qfile.h>
#include <qgrid.h>

#include "pwddlg.h"
#include "misc.h"

pwddlg::pwddlg(KUser *AUser, QWidget* parent, const char* name)
  : KDialogBase(parent, name, true, i18n("Enter Password"), Ok | Cancel, Ok, true), 
  user(AUser) 
{
  QGrid *page = makeGridMainWidget(2, QGrid::Horizontal);  

  QLabel* lb1 = new QLabel(page, "lb1");
  lb1->setText(i18n("Password:"));
  lb1->setMinimumSize(lb1->sizeHint());
  lb1->setAlignment(AlignRight|AlignVCenter);

  leusername1 = new QLineEdit(page, "LineEdit_1");

  // ensure it fits at least 12 characters
  leusername1->setText(QString::fromLatin1("XXXXXXXXXXXX"));
  leusername1->setMinimumSize(leusername1->sizeHint());

  // clear text
  leusername1->clear();
  leusername1->setFocus();
  leusername1->setEchoMode(QLineEdit::Password);

  QLabel* lb2 = new QLabel(page, "lb2");
  lb2->setText(i18n("Verify:"));
  lb2->setMinimumSize(lb2->sizeHint());
  lb2->setAlignment(AlignRight|AlignVCenter);

  leusername2 = new QLineEdit(page, "LineEdit_2");

  // ensure it fits at least 12 characters
  leusername2->setText(QString::fromLatin1("XXXXXXXXXXXX"));
  leusername2->setMinimumSize(leusername2->sizeHint());

  // clear text
  leusername2->clear();
  leusername2->setEchoMode(QLineEdit::Password);
}

pwddlg::~pwddlg() {
  delete leusername1;
  delete leusername2;
}

void pwddlg::slotOk() {
  char salt[3];
  char tmp[128];

  const char * set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

  if (leusername1->text() != leusername2->text()) {
    err->addMsg(i18n("Passwords are not identical.\nTry again"));
    err->display();
    leusername1->clear();
    leusername2->clear();
    leusername1->setFocus();
  }
  else {
    salt[0] = set[getpid() % strlen(set)];
    salt[1] = set[kapp->random() % strlen(set)];
    salt[2] = 0;

    strcpy(tmp, crypt(QFile::encodeName(leusername1->text()), salt));

#ifdef HAVE_SHADOW
    if (is_shadow != 0) {
      user->setSPwd(QString::fromLocal8Bit(tmp));
      user->setPwd(QString::fromLatin1("x"));
    }
    else
#endif
      user->setPwd(QString::fromLocal8Bit(tmp));
    accept();
  }
}

#include "pwddlg.moc"
