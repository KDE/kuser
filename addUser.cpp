#include <kmsgbox.h>

#include "globals.h"
#include "maindlg.h"
#include "misc.h"
#include "addUser.moc"

#ifdef _KU_QUOTA
addUser::addUser(KUser *auser, Quota *aquota, QWidget *parent = 0, const char *name = 0, int isprep = false) :
  propdlg(auser, aquota, parent, name, isprep) {

  createhome = new QCheckBox(w1, "createHome");
  createhome->setText(_("Create home directory"));
  createhome->setGeometry(200, 70, 200, 30);

  copyskel = new QCheckBox(w1, "copySkel");
  copyskel->setText(_("Copy skeleton"));
  copyskel->setGeometry(200, 110, 200, 30);
}
#else
addUser::addUser(KUser *auser, QWidget *parent = 0, const char *name = 0, int isprep = false) :
  propdlg(auser, parent, name, isprep) {

  createhome = new QCheckBox(w1, "createHome");
  createhome->setText(_("Create home directory"));
  createhome->setGeometry(260, 200, 120, 30);

  copyskel = new QCheckBox(w1, "copySkel");
  copyskel->setText(_("Copy skeleton"));
  copyskel->setGeometry(260, 250, 120, 30);
}
#endif

void addUser::ok() {
  QString tmp;
  uint newuid;
  tmp.setStr(leid->text());
  newuid = tmp.toInt();
  
  if (users->user_lookup(newuid) != NULL) {
    tmp.sprintf(_("User with UID %u already exists"), newuid);
    KMsgBox::message(0, _("Message"), tmp, KMsgBox::STOP, _("Ok"));
    return;
  }
  
  if (createhome->isChecked())
    createHome();

  if (copyskel->isChecked())
    copySkel();

  accept();
}

void addUser::createHome() {
  printf("createHome\n");
}

void addUser::copySkel() {
  printf("copySkel\n");
}
