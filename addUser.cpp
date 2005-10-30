/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
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
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>

#include <qdir.h>
#include <qgroupbox.h>

#include <kdebug.h>

#include "kglobal_.h"
#include "misc.h"

#include "addUser.h"
#include <kmessagebox.h>

addUser::addUser( KU::KUser *AUser, bool useprivategroup,
  QWidget *parent, const char *name ) :
  propdlg( AUser, useprivategroup, parent, name )
{
  QGroupBox *group = new QGroupBox(frontpage);
  group->setTitle(i18n("New Account Options"));
  QVBoxLayout *groupLayout = new QVBoxLayout(group, marginHint(), spacingHint());
  groupLayout->addSpacing(group->fontMetrics().lineSpacing());
  groupLayout->setAutoAdd(true);
  createhome = new QCheckBox(i18n("Create home folder"), group);
  createhome->setChecked(true);
  copyskel = new QCheckBox(i18n("Copy skeleton"), group);
  connect(createhome, SIGNAL(toggled(bool)), copyskel, SLOT(setEnabled(bool)));
  frontlayout->addMultiCellWidget(group, frontrow, frontrow, 0, 2);

  if ( useprivategroup ) pbprigr->setEnabled( false );
}

void addUser::slotOk()
{
  KU::KUser *user = mUsers.first();

  if ( !check() ) return;

  mergeUser( user, user );

  if ( ( user->getCaps() & KU::KUser::Cap_POSIX ) && 
    kug->getUsers().lookup( user->getUID() ) ) {
    KMessageBox::sorry( 0, i18n("User with UID %1 already exists.").arg( user->getUID() ) );
    return;
  }

  if ( ( kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ) && 
     ( user->getCaps() & KU::KUser::Cap_Samba ) ) {
    if ( kug->getUsers().lookup_sam( user->getSID().getRID() ) ) {
      KMessageBox::sorry( 0, i18n("User with RID %1 already exists.").arg( user->getSID().getRID() ) );
      return;
    }
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

  if (user->getCreateHome() && !checkHome())
     return;

  if (user->getCreateMailBox() && !checkMailBox())
     user->setCreateMailBox(false);

  saveg();
  accept();
}

bool addUser::checkHome()
{
  KU::KUser *user = mUsers.first();

  struct stat s;
  int r;

  QString h_dir = user->getHomeDir();
  r = stat( QFile::encodeName(h_dir), &s );

  if ( (r == -1) && (errno == ENOENT) )
    return true;

  if (r == 0) {
    if (S_ISDIR(s.st_mode)) {
       if ( KMessageBox::
         warningContinueCancel ( 0,
           i18n("Folder %1 already exists!\n%2 may become owner and permissions may change.\nDo you really want to use %3?").
           arg(h_dir).arg(user->getName()).arg(h_dir), QString::null, KStdGuiItem::cont() ) == KMessageBox::Cancel )

         return false;
       else
         return true;
    } else
      KMessageBox::error( 0, i18n("%1 is not a folder.").arg(h_dir) );
  } else
    KMessageBox::error( 0, i18n("stat() failed on %1.").arg(h_dir) );

  return false;
}

bool addUser::checkMailBox()
{
  QString mailboxpath;
  KU::KUser *user = mUsers.first();

  struct stat s;
  int r;

  mailboxpath = QFile::decodeName(MAIL_SPOOL_DIR) + "/" + user->getName();
  r = stat(QFile::encodeName(mailboxpath), &s);

  if ((r == -1) && (errno == ENOENT))
    return true;

  if (r == 0)
    if (S_ISREG(s.st_mode))
      KMessageBox::error( 0, i18n("Mailbox %1 already exists (uid=%2).")
                 .arg(mailboxpath)
                 .arg(s.st_uid) );
    else
      KMessageBox::error( 0, i18n("%1 exists but is not a regular file.")
                 .arg(mailboxpath) );
  else
    KMessageBox::error( 0, i18n("stat() failed on %1.").arg(mailboxpath) );

  return false;
}
#include "addUser.moc"
