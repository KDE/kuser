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

#include "globals.h"

#include <config.h>

#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>

#include <QVBoxLayout>
#include <QGroupBox>

#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>

#include "ku_global.h"

#include "ku_adduser.h"

KU_AddUser::KU_AddUser( KU_User &user, bool useprivategroup,
  QWidget *parent ) :
  KU_EditUser( user, useprivategroup, parent )
{
  QGroupBox *group = new QGroupBox(frontpage);
  group->setTitle(i18n("New Account Options"));
  QVBoxLayout *grouplayout = new QVBoxLayout( group );
  grouplayout->setMargin( marginHint() );
  grouplayout->setSpacing( spacingHint() );
//  grouplayout->addSpacing( group->fontMetrics().lineSpacing() );

  createhome = new QCheckBox(i18n("Create home folder"), 0);
  createhome->setChecked(true);
  grouplayout->addWidget( createhome );
  copyskel = new QCheckBox(i18n("Copy skeleton"), 0);
  grouplayout->addWidget( copyskel );

  connect(createhome, SIGNAL(toggled(bool)), copyskel, SLOT(setEnabled(bool)));
  frontlayout->addWidget(group, frontrow, 0, 1, 3);

  if ( useprivategroup ) pbprigr->setEnabled( false );
}

void KU_AddUser::slotOk()
{
  if ( !check() ) return;

  mergeUser( mNewUser, mNewUser );
  if ( ( mNewUser.getCaps() & KU_User::Cap_POSIX ) &&
    kug->getUsers()->lookup( mNewUser.getUID() ) != -1 ) {
    KMessageBox::sorry( 0, i18n("User with UID %1 already exists.").arg( mNewUser.getUID() ) );
    return;
  }

  if ( ( kug->getUsers()->getCaps() & KU_Users::Cap_Samba ) &&
     ( mNewUser.getCaps() & KU_User::Cap_Samba ) ) {
    if ( kug->getUsers()->lookup_sam( mNewUser.getSID().getRID() ) != -1 ) {
      KMessageBox::sorry( 0, i18n("User with RID %1 already exists.").arg( mNewUser.getSID().getRID() ) );
      return;
    }
  }

  if (createhome->isChecked())
  {
    mNewUser.setCreateHome(true);
    mNewUser.setCreateMailBox(true);
  }
  if (copyskel->isChecked())
  {
    mNewUser.setCopySkel(true);
  }

  if (mNewUser.getCreateHome() && !checkHome())
     return;

  if (mNewUser.getCreateMailBox() && !checkMailBox())
     mNewUser.setCreateMailBox(false);

  saveg();
  accept();
  kDebug() << "slotOk name: " << mNewUser.getName() << endl;
}

bool KU_AddUser::checkHome()
{
  struct stat s;
  int r;

  QString h_dir = mNewUser.getHomeDir();
  r = stat( QFile::encodeName(h_dir), &s );

  if ( (r == -1) && (errno == ENOENT) )
    return true;

  if (r == 0) {
    if (S_ISDIR(s.st_mode)) {
       if ( KMessageBox::
         warningContinueCancel ( 0,
           i18n("Folder %1 already exists!\n%2 may become owner and permissions may change.\nDo you really want to use %3?").
           arg(h_dir).arg(mNewUser.getName()).arg(h_dir), QString::null, KStdGuiItem::cont() ) == KMessageBox::Cancel )

         return false;
       else
         return true;
    } else
      KMessageBox::error( 0, i18n("%1 is not a folder.").arg(h_dir) );
  } else
    KMessageBox::error( 0, i18n("stat() failed on %1.").arg(h_dir) );

  return false;
}

bool KU_AddUser::checkMailBox()
{
  QString mailboxpath;

  struct stat s;
  int r;

  mailboxpath = QFile::decodeName(MAIL_SPOOL_DIR) + "/" + mNewUser.getName();
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
#include "ku_adduser.moc"