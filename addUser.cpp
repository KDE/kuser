#include <sys/file.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <qdir.h>

#include <kmsgbox.h>
#include <kstring.h>

#include "globals.h"
#include "maindlg.h"
#include "misc.h"
#include "addUser.moc"

#ifdef _KU_QUOTA
addUser::addUser(KUser *auser, Quota *aquota, QWidget *parent = 0, const char *name = 0, int isprep = false) :
  propdlg(auser, aquota, parent, name, isprep) {

  createhome = new QCheckBox(w1, "createHome");
  createhome->setText(i18n("Create home directory"));
  createhome->setGeometry(200, 70, 200, 30);

  copyskel = new QCheckBox(w1, "copySkel");
  copyskel->setText(i18n("Copy skeleton"));
  copyskel->setGeometry(200, 110, 200, 30);
}
#else
addUser::addUser(KUser *auser, QWidget *parent = 0, const char *name = 0, int isprep = false) :
  propdlg(auser, parent, name, isprep) {

  createhome = new QCheckBox(w1, "createHome");
  createhome->setText(i18n("Create home directory"));
  createhome->setGeometry(200, 70, 200, 30);

  copyskel = new QCheckBox(w1, "copySkel");
  copyskel->setText(i18n("Copy skeleton"));
  copyskel->setGeometry(200, 110, 200, 30);
}
#endif

void addUser::ok() {
  QString tmp;
  uint newuid;
  tmp.setStr(leid->text());
  newuid = tmp.toInt();
  
  if (users->user_lookup(newuid) != NULL) {
    ksprintf(&tmp, i18n("User with UID %u already exists"), newuid);
    KMsgBox::message(0, i18n("Message"), tmp, KMsgBox::STOP, i18n("OK"));
    return;
  }

  check();
  
  if (createhome->isChecked())
    if ((checkHome()) && (checkMailBox())) {
      user->setr_createhome(1);
      user->setr_createmailbox(1);
    }

  if (copyskel->isChecked())
    user->setr_copyskel(1);

  accept();
}

bool addUser::checkHome() {
  struct stat s;
  int r;
  QString tmp;

  r = stat(user->getp_dir(), &s);

  if ((r == -1) && (errno = ENOENT))
    return true;

  if (r == 0)
    if (S_ISDIR(s.st_mode))
      ksprintf(&tmp, i18n("Directory %s already exists (uid = %d, gid = %d)"), 
           (const char *)user->getp_dir(), s.st_uid, s.st_gid);
    else
      ksprintf(&tmp, i18n("%s is not a directory") ,(const char *)user->getp_dir());
  else
    ksprintf(&tmp, "checkHome: stat: %s ", strerror(errno));
  
  err->addMsg(tmp, STOP);
  err->display();

  return false;
}

bool addUser::checkMailBox() {
  QString mailboxpath;
  QString tmp;

  struct stat s;
  int r;

  ksprintf(&mailboxpath, "%s/%s", MAIL_SPOOL_DIR,
           (const char *)user->getp_name());
  r = stat(mailboxpath, &s);
  
  if ((r == -1) && (r == ENOENT))
    return true;

  if (r == 0)
    if (S_ISREG(s.st_mode))
      ksprintf(&tmp, i18n("Mailbox %s already exist (uid=%d)"),
               (const char *)mailboxpath, s.st_uid);
    else
      ksprintf(&tmp, i18n("%s exists but is not a regular file"),
               (const char *)mailboxpath);
  else
    ksprintf(&tmp, "checkMail: stat: %s ", strerror(errno));
  
  err->addMsg(tmp, STOP);
  err->display();

  return false;
}
