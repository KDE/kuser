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

#include "globals.h"
#include "kglobal_.h"
#include "misc.h"

#include "addUser.h"
#include <kmessagebox.h>

#ifdef _KU_QUOTA
addUser::addUser(KUser &AUser, Quota &AQuota, QWidget *parent, const char *name, int isprep) :
  propdlg(AUser, AQuota, parent, name, isprep) {
#else
addUser::addUser(KUser &AUser, QWidget *parent, const char *name, int isprep) :
  propdlg(AUser, parent, name, isprep) {
#endif

  createhome = new QCheckBox(w1, "createHome");
  createhome->setText(i18n("Create home directory"));
  createhome->setGeometry(200, 70, 200, 30);
  createhome->setChecked(true);
  connect(createhome, SIGNAL(toggled(bool)), this, SLOT(createHomeChecked(bool)));

  copyskel = new QCheckBox(w1, "copySkel");
  copyskel->setText(i18n("Copy skeleton"));
  copyskel->setGeometry(200, 110, 200, 30);
  copyskel->setEnabled(FALSE);

  userPrivateGroup = new QCheckBox(w1, "usePrivateGroup");
  userPrivateGroup->setText(i18n("Use Private Group"));
  userPrivateGroup->setGeometry(200, 150, 200, 30);
  connect(userPrivateGroup, SIGNAL(toggled(bool)), this, SLOT(userPrivateGroupChecked(bool)));
}

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
  
  if (kug->getUsers().lookup(newuid) != NULL) {
    err->addMsg(i18n("User with UID %1 already exists").arg(newuid));
    err->display();
    return;
  }

  check();
  
  if (createhome->isChecked())
    if ((checkHome()) && (checkMailBox())) {
      user.setCreateHome(1);
      user.setCreateMailBox(1);
    }
		else																													
			return;	

  if (copyskel->isChecked())
    user.setCopySkel(1);

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

  QString h_dir = user.getHomeDir();
  r = stat(QFile::encodeName(h_dir), &s);

  if ((r == -1) && (errno == ENOENT))
    return true;

  if (r == 0) {
    if (S_ISDIR(s.st_mode)) {
       if (KMessageBox::
         warningContinueCancel (0, i18n("Directory %1 already exists!\n%2 may become owner and permissions may change.\nDo you really want to use %3?").arg(h_dir).arg(user.getName()).arg(h_dir), QString::null, i18n("&Continue")) == KMessageBox::Cancel)
                  return false;
            else
                  return true;
       } else
	  err->addMsg(i18n("%1 is not a directory").arg(h_dir));
  } else
    err->addMsg(QString("checkHome: stat: %1 ").arg(QString::fromLocal8Bit(strerror(errno))));
  
  err->display();

  return false;
}

bool addUser::checkMailBox() {
  QString mailboxpath;

  struct stat s;
  int r;

  mailboxpath = QString("%1/%1").arg(MAIL_SPOOL_DIR).arg(user.getFullName());
  r = stat(QFile::encodeName(mailboxpath), &s);
  
  if ((r == -1) && (errno == ENOENT))
    return true;

  if (r == 0)
    if (S_ISREG(s.st_mode))
      err->addMsg(i18n("Mailbox %1 already exist (uid=%2)")
                 .arg(mailboxpath)
                 .arg(s.st_uid));
    else
      err->addMsg(i18n("%1 exists but is not a regular file")
                 .arg(mailboxpath));
  else
    err->addMsg(QString("checkMail: stat: %1 ").arg(QString::fromLocal8Bit(strerror(errno))));
  
  err->display();

  return false;
}
#include "addUser.moc"
