#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
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

#include "globals.h"
#include "maindlg.h"
#include "misc.h"

#include "addUser.h"

#ifdef _KU_QUOTA
addUser::addUser(KUser *auser, Quota *aquota, QWidget *parent, const char *name, int isprep) :
  propdlg(auser, aquota, parent, name, isprep) {

  createhome = new QCheckBox(w1, "createHome");
  createhome->setText(i18n("Create home directory"));
  createhome->setGeometry(200, 70, 200, 30);
  createhome->setChecked(true);
  connect(createhome, SIGNAL(toggled(bool)), this, SLOT(createHomeChecked(bool)));

  copyskel = new QCheckBox(w1, "copySkel");
  copyskel->setText(i18n("Copy skeleton"));
  copyskel->setGeometry(200, 110, 200, 30);
  copyskel->setEnabled(FALSE);

  userPrivateGroup = new QCheckBox(w1, "userPrivateGroup");
  userPrivateGroup->setText(i18n("User Private Group"));
  userPrivateGroup->setGeometry(200, 150, 200, 30);
  connect(userPrivateGroup, SIGNAL(toggled(bool)), this, SLOT(userPrivateGroupChecked(bool)));
}
#else
addUser::addUser(KUser *auser, QWidget *parent = 0, const char *name = 0, int isprep = false) :
  propdlg(auser, parent, name, isprep) {

  createhome = new QCheckBox(w1, "createHome");
  createhome->setText(i18n("Create home directory"));
  createhome->setGeometry(200, 70, 200, 30);
  createhome->setChecked(true);
  connect(createhome, SIGNAL(toggled(bool)), this, SLOT(createHomeChecked(bool)));

  copyskel = new QCheckBox(w1, "copySkel");
  copyskel->setText(i18n("Copy skeleton"));
  copyskel->setGeometry(200, 110, 200, 30);
  copyskel->setEnabled(FALSE);

  usePrivateGroup = new QCheckBox(w1, "usePrivateGroup");
  usePrivateGroup->setText(i18n("Use Private Group"));
  usePrivateGroup->setGeometry(200, 150, 200, 30);
  connect(usePrivateGroup, SIGNAL(toggled(bool)), this, SLOT(usePrivateGroupChecked(bool)));
}
#endif

void addUser::setUserPrivateGroup(bool data) {
  userPrivateGroup->setChecked(data);
}

void addUser::setCreateHomeDir(bool data) {
  createhome->setChecked(data);
  copyskel->setEnabled(data);
}

void addUser::setCopySkel(bool data) {
  copyskel->setChecked(data);
}

bool addUser::getUserPrivateGroup() {
  return userPrivateGroup->isChecked();
}

bool addUser::getCreateHomeDir() {
  return createhome->isChecked();
}

bool addUser::getCopySkel() {
  return copyskel->isChecked();
}

void addUser::ok() {
  QString tmp;
  uint newuid;
  tmp = leid->text();
  newuid = tmp.toInt();
  
  if (users->lookup(newuid) != NULL) {
    tmp = i18n("User with UID %1 already exists").arg(newuid);
    KMsgBox::message(0, i18n("Message"), tmp, KMsgBox::STOP, i18n("OK"));
    return;
  }

  check();
  
  if (createhome->isChecked())
    if ((checkHome()) && (checkMailBox())) {
      user->setCreateHome(1);
      user->setCreateMailBox(1);
    }

  if (copyskel->isChecked())
    user->setCopySkel(1);

  accept();
}

void addUser::userPrivateGroupChecked(bool data) {
  cbpgrp->setEnabled(!data);
}

void addUser::createHomeChecked(bool data) {
  copyskel->setEnabled(data);
}

bool addUser::checkHome() {
  struct stat s;
  int r;
  QString tmp;

  r = stat(user->getHomeDir(), &s);

  if ((r == -1) && (errno = ENOENT))
    return true;

  if (r == 0)
    if (S_ISDIR(s.st_mode))
      tmp = i18n("Directory %1 already exists (uid = %2, gid = %3)") 
                 .arg(user->getHomeDir())
                 .arg(s.st_uid)
                 .arg(s.st_gid);
    else
      tmp = i18n("%1 is not a directory").arg(user->getHomeDir());
  else
    tmp = QString("checkHome: stat: %1 ").arg(strerror(errno));
  
  err->addMsg(tmp, STOP);
  err->display();

  return false;
}

bool addUser::checkMailBox() {
  QString mailboxpath;
  QString tmp;

  struct stat s;
  int r;

  mailboxpath = QString("%s/%s")
	.arg(MAIL_SPOOL_DIR)
	.arg(user->getFullName());
  r = stat(mailboxpath, &s);
  
  if ((r == -1) && (errno == ENOENT))
    return true;

  if (r == 0)
    if (S_ISREG(s.st_mode))
      tmp = i18n("Mailbox %1 already exist (uid=%2)")
                 .arg(mailboxpath)
                 .arg(s.st_uid);
    else
      tmp = i18n("%1 exists but is not a regular file")
                 .arg(mailboxpath);
  else
    tmp = QString("checkMail: stat: %s ").arg(strerror(errno));
  
  err->addMsg(tmp, STOP);
  err->display();

  return false;
}
