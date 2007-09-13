/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi GyĂśrgy <gyurco@freemail.hu>
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

#include <ku_config.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/file.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <fcntl.h>

#include <QDir>
#include <QSharedData>

#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kshell.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <kurl.h>
#include <klocale.h>

#include "ku_user.h"
#include "ku_misc.h"

KU_User_Private::KU_User_Private()
{
  LastChange = 0;
  Expire = -1;
  UID     = 0;
  GID     = 100;

  Min     = 0;
  Max     = 99999;
  Warn    = 7;
  Inactive= -1;
//  s_flag    = 0;
  Caps = 0;
  CreateHome = false;
  CreateMailBox = false;
  CopySkel = false;
  DeleteHome = false;
  DeleteMailBox = false;

  Disabled = true;
}

// class KU_User

KU_User::KU_User()
{
  d = new KU_User_Private;
}

KU_User::KU_User(const KU_User *user)
{
  d = new KU_User_Private;
  copy(user);
}

void KU_User::copy(const KU_User *user)
{
  if ( user != this ) {
    *this = *user;
  }
}

bool KU_User::operator ==(const KU_User &other) const
{
  if ( getUID() == other.getUID() &&
       getName() == other.getName() )
    return true;
  else
    return false;
}

KU_User::~KU_User()
{
}

KU_PROPERTY_IMPL(KU_User,int,Caps)
KU_PROPERTY_IMPL(KU_User,QString,Name)
KU_PROPERTY_IMPL(KU_User,QString,Surname)
KU_PROPERTY_IMPL(KU_User,QString,Email)
KU_PROPERTY_IMPL(KU_User,QString,Pwd)
KU_PROPERTY_IMPL(KU_User,QString,HomeDir)
KU_PROPERTY_IMPL(KU_User,QString,Shell)
KU_PROPERTY_IMPL(KU_User,QString,FullName)
KU_PROPERTY_IMPL(KU_User,uid_t,UID)
KU_PROPERTY_IMPL(KU_User,uid_t,GID)
KU_PROPERTY_IMPL(KU_User,bool,Disabled)

//gecos
//--BSD gecos
KU_PROPERTY_IMPL(KU_User,QString,Office)
KU_PROPERTY_IMPL(KU_User,QString,WorkPhone)
KU_PROPERTY_IMPL(KU_User,QString,HomePhone)
KU_PROPERTY_IMPL(KU_User,QString,Class)
//--BSD end
KU_PROPERTY_IMPL(KU_User,QString,Office1)
KU_PROPERTY_IMPL(KU_User,QString,Office2)
KU_PROPERTY_IMPL(KU_User,QString,Address)

//shadow
KU_PROPERTY_IMPL(KU_User,QString,SPwd)
KU_PROPERTY_IMPL(KU_User,time_t,Expire)
KU_PROPERTY_IMPL(KU_User,time_t,LastChange)
KU_PROPERTY_IMPL(KU_User,int,Min)
KU_PROPERTY_IMPL(KU_User,int,Max)
KU_PROPERTY_IMPL(KU_User,int,Warn)
KU_PROPERTY_IMPL(KU_User,int,Inactive)
KU_PROPERTY_IMPL(KU_User,int,Flag)

//samba
KU_PROPERTY_IMPL(KU_User,QString, LMPwd)
KU_PROPERTY_IMPL(KU_User,QString, NTPwd)
KU_PROPERTY_IMPL(KU_User,QString, LoginScript)
KU_PROPERTY_IMPL(KU_User,QString, ProfilePath)
KU_PROPERTY_IMPL(KU_User,QString, HomeDrive)
KU_PROPERTY_IMPL(KU_User,QString, HomePath)
KU_PROPERTY_IMPL(KU_User,QString, Workstations)
KU_PROPERTY_IMPL(KU_User,QString, Domain)
KU_PROPERTY_IMPL(KU_User,SID, SID)
KU_PROPERTY_IMPL(KU_User,SID, PGSID)

//Administrative
KU_PROPERTY_IMPL(KU_User,bool, CreateHome)
KU_PROPERTY_IMPL(KU_User,bool, CreateMailBox)
KU_PROPERTY_IMPL(KU_User,bool, CopySkel)
KU_PROPERTY_IMPL(KU_User,bool, DeleteHome)
KU_PROPERTY_IMPL(KU_User,bool, DeleteMailBox)

int KU_User::createHome()
{

  if(d->HomeDir.isNull() || d->HomeDir.isEmpty()) {
    KMessageBox::sorry( 0, i18n("Cannot create home folder for %1: it is null or empty.", d->Name) );
    return(0);
  }
  if (mkdir(QFile::encodeName(d->HomeDir), 0700) != 0) {
    if (errno != EEXIST)
    {
      KMessageBox::error( 0, i18n("Cannot create home folder %1.\nError: %2", d->HomeDir, QString::fromLocal8Bit(strerror(errno))) );
      return(0);
    }
  }

  if (chown(QFile::encodeName(d->HomeDir), d->UID, d->GID) != 0) {
    KMessageBox::error( 0, i18n("Cannot change owner of home folder %1.\nError: %2", d->HomeDir, QString::fromLocal8Bit(strerror(errno))) );
    return(1);
  }

  if (chmod(QFile::encodeName(d->HomeDir), KU_HOMEDIR_PERM) != 0) {
    KMessageBox::error( 0, i18n("Cannot change permissions on home folder %1.\nError: %2", d->HomeDir, QString::fromLocal8Bit(strerror(errno))) );
    return(1);
  }
  return(1);
}

int KU_User::tryCreate(const QString &dir)
{
  struct stat sb;
  int rc = 0;

  rc = stat(QFile::encodeName(dir), &sb);
  if (rc == 0) {
    if (S_ISDIR(sb.st_mode)) {
      if (KMessageBox::warningContinueCancel( 0,
        i18n("Folder %1 already exists.\nWill make %2 owner and change permissions.\nDo you want to continue?", dir, d->Name),
        QString(), KStandardGuiItem::cont() ) == KMessageBox::Continue) {

        if (chown(QFile::encodeName(dir), d->UID, d->GID) != 0) {
          KMessageBox::error( 0, i18n("Cannot change owner of %1 folder.\nError: %2" , dir, QString::fromLocal8Bit(strerror(errno))) );
        }
        return(0);
      } else {
        KMessageBox::information( 0, i18n("Folder %1 left 'as is'.\nVerify ownership and permissions for user %2 who may not be able to log in.", dir, d->Name) );
        return(-1);
      }
    } else {
      KMessageBox::information( 0, i18n("%1 exists and is not a folder. User %2 will not be able to log in.", dir, d->Name) );
      return(-1);
    }
  } else {
    if (errno == ENOENT) {
      if (mkdir(QFile::encodeName(dir), 0700) != 0) {
        KMessageBox::error( 0, i18n("Cannot create %1 folder.\nError: %2", dir, QString::fromLocal8Bit(strerror(errno))));
        return(-1);
      }
      if (chown(QFile::encodeName(dir), d->UID, d->GID) != 0) {
        KMessageBox::error( 0, i18n("Cannot change owner of %1 folder.\nError: %2", dir, QString::fromLocal8Bit(strerror(errno))) );
      }
      return(0);
    } else {
      KMessageBox::error( 0, i18n("stat call on %1 failed.\nError: %2", dir, QString::fromLocal8Bit(strerror(errno))) );
      return(-1);
    }
  }
}

int KU_User::createMailBox()
{
  QString mailboxpath;
  int fd;
  mailboxpath = QFile::decodeName(MAIL_SPOOL_DIR) + '/' + d->Name;
  if((fd = open(QFile::encodeName(mailboxpath), O_CREAT|O_EXCL|O_WRONLY,
                S_IRUSR|S_IWUSR)) < 0) {
    if (errno != EEXIST)
    {
      KMessageBox::error( 0, i18n("Cannot create %1: %2",
                 mailboxpath,
                 QString::fromLocal8Bit(strerror(errno))) );
      return -1;
    }
  }

  close(fd);

  if (chown(QFile::encodeName(mailboxpath), d->UID, KU_MAILBOX_GID) != 0) {
    KMessageBox::error( 0, i18n("Cannot change owner on mailbox: %1\nError: %2",
                 mailboxpath, QString::fromLocal8Bit(strerror(errno))) );
    return -1;
  }

  if (chmod(QFile::encodeName(mailboxpath), KU_MAILBOX_PERM) != 0) {
    KMessageBox::error( 0, i18n("Cannot change permissions on mailbox: %1\nError: %2",
                 mailboxpath, QString::fromLocal8Bit(strerror(errno))) );
    return -1;
  }

  return 0;
}

int KU_User::copySkel()
{
  QDir source(QFile::decodeName(SKELDIR));
  QDir dest(d->HomeDir);
  mode_t mode;

  if (!source.exists()) {
    KMessageBox::error( 0, i18n("Folder %1 does not exist, cannot copy skeleton for %2.", source.absolutePath(), d->Name) );
    return (-1);
  }

  if (!dest.exists()) {
    KMessageBox::error( 0, i18n("Folder %1 does not exist, cannot copy skeleton.", dest.absolutePath()) );
    return (-1);
  }

  mode = umask(0007);
  copyDir(source.absolutePath(), dest.absolutePath(), d->UID, d->GID);
  umask( mode );

  return 0;
}

int KU_User::removeHome()
{
  struct stat sb;

  if (!stat(QFile::encodeName(d->HomeDir), &sb))
    if (S_ISDIR(sb.st_mode) && sb.st_uid == d->UID) {
      if (!KIO::NetAccess::del(KUrl(d->HomeDir),0L)) {
             KMessageBox::error( 0, i18n("Cannot remove home folder %1.\nError: %2",
                        d->HomeDir, KIO::NetAccess::lastErrorString()) );
      }
    } else {
      KMessageBox::error( 0, i18n("Removal of home folder %1 failed (uid = %2, gid = %3).", d->HomeDir, sb.st_uid, sb.st_gid) );
    }
  else {
    KMessageBox::error( 0, i18n("stat call on file %1 failed.\nError: %2",
                  d->HomeDir, QString::fromLocal8Bit(strerror(errno))) );
  }

  return 0;
}

//TODO: remove at jobs too.

int KU_User::removeCrontabs()
{
  QString file;
  QString command;

  file = QFile::decodeName(CRONTAB_DIR) + '/' + d->Name;
  if ( access(QFile::encodeName(file), F_OK) == 0 ) {
    command = QString::fromLatin1("crontab -u %1 -r").arg(KShell::quoteArg(d->Name));
    if ( system(QFile::encodeName(command)) != 0 ) {
      KMessageBox::error( 0, i18n("Cannot remove crontab %1.\nError: %2",
                   command, QString::fromLocal8Bit(strerror(errno))) );
     }
  }

  return 0;
}

int KU_User::removeMailBox()
{
  QString file;

  file = QFile::decodeName(MAIL_SPOOL_DIR) + '/' + d->Name;
  if (remove(QFile::encodeName(file)) != 0) {
    KMessageBox::error( 0, i18n("Cannot remove mailbox %1.\nError: %2",
                 file, QString::fromLocal8Bit(strerror(errno))) );
  }

  return 0;
}

KU_Users::KU_Users(KU_PrefsBase *cfg)
{
  mCfg = cfg;
}

KU_Users::~KU_Users()
{
}

const QString &KU_Users::getDOMSID() const
{
  return domsid;
}

void KU_Users::parseGecos( const char *gecos, QString &name,
  QString &field1, QString &field2, QString &field3 )
{
  int no = 0;
  const char *s = gecos;
  const char *pos = NULL;
  // At least one part of the string exists
  for(;;) {
    pos = strchr(s, ',');
    QString val;
    if(pos == NULL)
      val = QString::fromLocal8Bit(s);
    else
      val = QString::fromLocal8Bit(s, (int)(pos-s));

    switch(no) {
      case 0: name = val; break;
      case 1: field1 = val; break;
      case 2: field2 = val; break;
      case 3: field3 = val; break;
    }
    if(pos == NULL) break;
    s = pos+1;
    no++;
  }
}

void KU_Users::fillGecos(KU_User &user, const char *gecos)
{
  QString name,field1,field2,field3;
  parseGecos( gecos, name, field1, field2, field3 );
  user.setFullName( name );
  caps & Cap_BSD ? user.setOffice( field1 ) : user.setOffice1( field1 );
  caps & Cap_BSD ? user.setWorkPhone( field2 ) : user.setOffice2( field2 );
  caps & Cap_BSD ? user.setHomePhone( field3 ) : user.setAddress( field3 );
}

bool KU_Users::doCreate(KU_User *user)
{
  QString h_dir;

  if(user->getCreateMailBox()) {
    user->createMailBox();
    user->setCreateMailBox(false);
  }

  if(user->getCreateHome()) {
    if(user->createHome()) {
      user->setCreateHome(false);
     } else {
       return false; // if createHome fails, copySkel is irrelevant!
     }

     if(user->getCopySkel()) {
       if((user->copySkel()) == 0) {
         user->setCopySkel(false);
       }
     }

  }
  return true;
}

bool KU_Users::doDelete( KU_User *user )
{
  kDebug() << "delete user: " << user->getName() << " uid: " << user->getUID();
  if ( user->getDeleteHome() ) {
    user->removeHome();
    user->removeCrontabs();
  }
  if ( user->getDeleteMailBox() )
    user->removeMailBox();

  return true;
}

int KU_Users::lookup(const QString & name) const
{
  for ( int i = 0; i<count(); i++ ) {
    if ( at(i).getName() == name ) return i;
  }
  return -1;
}

int KU_Users::lookup(uid_t uid) const
{
  for ( int i = 0; i<count(); i++ ) {
    if ( at(i).getUID() == uid ) return i;
  }
  return -1;
}

int KU_Users::lookup_sam( const SID &sid ) const
{
  for ( int i = 0; i<count(); i++ ) {
    if ( at(i).getSID() == sid ) return i;
  }
  return -1;
}

int KU_Users::lookup_sam( const QString &sid ) const
{
  for ( int i = 0; i<count(); i++ ) {
    if ( at(i).getSID().getSID() == sid ) return i;
  }
  return -1;
}

int KU_Users::lookup_sam( uint rid ) const
{
  for ( int i = 0; i<count(); i++ ) {
    if ( at(i).getSID().getRID() == rid ) return i;
  }
  return -1;
}

uid_t KU_Users::first_free() const
{
  uid_t t;

  for (t = mCfg->firstUID() ; t<65534; t++)
    if (lookup(t) == -1)
      return t;

  return NO_FREE;
}

uint KU_Users::first_free_sam() const
{
  uint t;

  for (t = 1000; t<65534; t++)
    if (lookup_sam(t) == -1)
      return t;

  return 0;
}

void KU_Users::add(const KU_User &user)
{
  mAdd.append( user );
}

void KU_Users::del( int index)
{
  mDel.append( index );
}

void KU_Users::mod(int index, const KU_User &newuser)
{
  mMod.insert( index, newuser );
}

void KU_Users::commit()
{
  kDebug() << "KU_Users::commit()";
  
  for ( ModList::Iterator it = mModSucc.begin(); it != mModSucc.end(); ++it ) {
      replace(it.key(),*it);
  }
  for ( AddList::Iterator it = mAddSucc.begin(); it != mAddSucc.end(); ++it ) {
      append(*it);
  }
  for ( DelList::Iterator it = mDelSucc.begin(); it != mDelSucc.end(); ++it ) {
      removeAt(*it);
  }
  cancelMods();
}

void KU_Users::cancelMods()
{
  mAdd.clear();
  mDel.clear();
  mMod.clear();
  mAddSucc.clear();
  mDelSucc.clear();
  mModSucc.clear();
}
