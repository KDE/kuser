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
  createhome->setGeometry(200, 70, 200, 30);

  copyskel = new QCheckBox(w1, "copySkel");
  copyskel->setText(_("Copy skeleton"));
  copyskel->setGeometry(200, 110, 200, 30);
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

  check();
  
  if (createhome->isChecked())
    createHome();

  if (copyskel->isChecked())
    if (copySkel() == -1)
      return;

  accept();
}

void addUser::createHome() {
  QDir d = QDir::root();

  if (d.cd(user->getp_dir())) {
    QString tmp;
    tmp.sprintf(_("Directory %s already exists"), (const char *)user->getp_dir());
    err->addMsg(tmp, STOP);
    err->display();
  }
  
  if (mkdir((const char *)user->getp_dir(), 0700) != 0) {
    QString tmp;
    tmp.sprintf(_("Cannot create home directory\nError: %s"), strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
  }

  if (chown((const char *)user->getp_dir(), user->getp_uid(), user->getp_gid()) != 0) {
    QString tmp;
    tmp.sprintf(_("Cannot change owner of home directory\nError: %s"), strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
  }

  if (chmod(user->getp_dir(), 0755) != 0) {
    QString tmp;
    tmp.sprintf(_("Cannot change permissions on home directory\nError: %s"), strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
  }

}

int addUser::copySkel() {
  QDir s("/etc/skel");
  QDir d(user->getp_dir());
  QString tmp;

  s.setFilter(QDir::Files | QDir::Hidden);

  if (!s.exists()) {
    QString tmp;
    tmp.sprintf(_("Directory %s does not exist"), (const char *)s.dirName());
    err->addMsg(tmp, STOP);
    err->display();
    return (-1);
  }

  if (!d.exists()) {
    QString tmp;
    tmp.sprintf(_("Directory %s does not exist"), (const char *)d.dirName());
    err->addMsg(tmp, STOP);
    err->display();
    return (-1);
  }

  for (uint i=0; i<s.count(); i++) {
    if (copyFile(s.filePath(s[i]), d.filePath(s[i])) == -1) {
      err->display();
      continue;
    }

    if (chown(d.filePath(s[i]), user->getp_uid(), user->getp_gid()) != 0) {
      QString tmp;
      tmp.sprintf(_("Cannot change owner of file %s\nError: %s"), (const char *)d.filePath(s[i]), strerror(errno));
      err->addMsg(tmp, STOP);
      err->display();
    }

    if (chmod(d.filePath(s[i]), 0644) != 0) {
      QString tmp;
      tmp.sprintf(_("Cannot change permissions on file %s\nError: %s"), (const char *)d.filePath(s[i]), strerror(errno));
      err->addMsg(tmp, STOP);
      err->display();
    }
  }
  
  return (0);
}
