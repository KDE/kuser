/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Maintained by Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

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
#include <qgroupbox.h>


#include "globals.h"
#include "kglobal_.h"
#include "misc.h"

#include "addUser.h"
#include <kmessagebox.h>

#ifdef _KU_QUOTA
addUser::addUser(KUser *AUser, Quota *AQuota, QWidget *parent, const char *name, int isprep) :
  propdlg(AUser, AQuota, parent, name, isprep) {
#else
addUser::addUser(KUser *AUser, QWidget *parent, const char *name, int isprep) :
  propdlg(AUser, parent, name, isprep) {
#endif

  QGroupBox *group = new QGroupBox(frontpage);
  group->setTitle(i18n("New Account Options"));
  QVBoxLayout *groupLayout = new QVBoxLayout(group, marginHint(), spacingHint());
  groupLayout->addSpacing(group->fontMetrics().lineSpacing());
  groupLayout->setAutoAdd(true);
  createhome = new QCheckBox(i18n("Create home folder"), group);
  createhome->setChecked(true);
  copyskel = new QCheckBox(i18n("Copy skeleton"), group);
  usePrivateGroup = new QCheckBox(i18n("Use private group"), group);
  connect(createhome, SIGNAL(toggled(bool)), copyskel, SLOT(setEnabled(bool)));
  frontlayout->addMultiCellWidget(group, frontrow, frontrow, 0, 2);
}

void addUser::slotOk() {
  QString tmp;
  uint newuid;
  tmp = leid->text();
  newuid = tmp.toInt();
  
  if (kug->getUsers().lookup(newuid) != 0) {
    err->addMsg(i18n("User with UID %1 already exists").arg(newuid));
    err->display();
    return;
  }

  if (createhome->isChecked())
  {
    user->setCreateHome(true);
    user->setCreateMailBox(true);
  }
  if (copyskel->isChecked())
  {
    user->setCopySkel(true);
  }

  check();
  
  if (user->getCreateHome() && !checkHome())
     return;

  if (user->getCreateMailBox() && !checkMailBox())
     user->setCreateMailBox(false);

  accept();
}

bool addUser::checkHome() {
  struct stat s;
  int r;

  QString h_dir = user->getHomeDir();
  r = stat(QFile::encodeName(h_dir), &s);

  if ((r == -1) && (errno == ENOENT))
    return true;

  if (r == 0) {
    if (S_ISDIR(s.st_mode)) {
       if (KMessageBox::
         warningContinueCancel (0, i18n("Folder %1 already exists!\n%2 may become owner and permissions may change.\nDo you really want to use %3?").arg(h_dir).arg(user->getName()).arg(h_dir), QString::null, KStdGuiItem::cont()) == KMessageBox::Cancel)
                  return false;
            else
                  return true;
       } else
	  err->addMsg(i18n("%1 is not a folder").arg(h_dir));
  } else
    err->addMsg(QString::fromLatin1("checkHome: stat: %1 ").arg(QString::fromLocal8Bit(strerror(errno))));
  
  err->display();

  return false;
}

bool addUser::checkMailBox() {
  QString mailboxpath;

  struct stat s;
  int r;

  mailboxpath = QString::fromLatin1("%1/%2").arg(QFile::decodeName(MAIL_SPOOL_DIR)).arg(user->getName());
  r = stat(QFile::encodeName(mailboxpath), &s);
  
  if ((r == -1) && (errno == ENOENT))
    return true;

  if (r == 0)
    if (S_ISREG(s.st_mode))
      err->addMsg(i18n("Mailbox %1 already exists (uid=%2)")
                 .arg(mailboxpath)
                 .arg(s.st_uid));
    else
      err->addMsg(i18n("%1 exists but is not a regular file")
                 .arg(mailboxpath));
  else
    err->addMsg(QString::fromLatin1("checkMail: stat: %1 ").arg(QString::fromLocal8Bit(strerror(errno))));
  
  err->display();

  return false;
}
#include "addUser.moc"
