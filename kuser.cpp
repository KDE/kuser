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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include "globals.h"

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
#include <qstring.h>
#include <qdir.h>

#include "kglobal_.h"
#include "kuser.h"
#include "misc.h"
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kdebug.h>

// class KUser

KUser::KUser()
{
  p_change = 0;
  p_expire = -1;
  p_uid     = 0;
  p_gid     = 100;

  s_min     = 0;
  s_max     = 99999;
  s_warn    = 7;
  s_inact   = -1;
//  s_flag    = 0;
  isCreateHome = false;
  isCreateMailBox = false;
  isCopySkel = false;
  isDeleteHome = false;
  isDeleteMailBox = false;

  isDisabled = true;
}

KUser::KUser(const KUser *user)
{
  copy(user);
}

void KUser::copy(const KUser *user)
{
  if ( user != this ) {
    p_name = user->p_name;
    p_surname = user->p_surname;
    p_email = user->p_email;
    p_pwd = user->p_pwd;
    p_dir = user->p_dir;
    p_shell = user->p_shell;
    p_fname = user->p_fname;
    p_office = user->p_office;
    p_ophone = user->p_ophone;
    p_hphone = user->p_hphone;
    p_class = user->p_class;
    p_change = user->p_change;
    p_expire = user->p_expire;
    p_office1 = user->p_office1;
    p_office2 = user->p_office2;
    p_address = user->p_address;

    p_uid     = user->p_uid;
    p_gid     = user->p_gid;

    s_pwd = user->s_pwd;
    s_min     = user->s_min;
    s_max     = user->s_max;
    s_warn    = user->s_warn;
    s_inact   = user->s_inact;
    s_flag    = user->s_flag;

    sam_lmpwd = user->sam_lmpwd;
    sam_ntpwd = user->sam_ntpwd;
    sam_loginscript = user->sam_loginscript;
    sam_profile = user->sam_profile;
    sam_homedrive = user->sam_homedrive;
    sam_homepath = user->sam_homepath;
    sid = user->sid;
    pgroup_sid = user->pgroup_sid;

    isCreateHome = user->isCreateHome;
    isCreateMailBox = user->isCreateMailBox;
    isDeleteHome = user->isDeleteHome;
    isDeleteMailBox = user->isDeleteMailBox;
    isCopySkel = user->isCopySkel;
    isDisabled = user->isDisabled;
  }
}

KUser::~KUser()
{
}

bool KUser::getDeleteHome()
{
  return isDeleteHome;
}

bool KUser::getDeleteMailBox()
{
  return isDeleteMailBox;
}

bool KUser::getCreateHome()
{
  return isCreateHome;
}

bool KUser::getCreateMailBox()
{
  return isCreateMailBox;
}

bool KUser::getCopySkel()
{
  return isCopySkel;
}

const QString &KUser::getName() const
{
  return p_name;
}

const QString &KUser::getSurname() const
{
  return p_surname;
}

const QString &KUser::getEmail() const
{
  return p_email;
}

const QString &KUser::getPwd() const
{
  return p_pwd;
}

const QString &KUser::getHomeDir() const
{
  return p_dir;
}

const QString &KUser::getShell() const
{
  return p_shell;
}

const QString &KUser::getFullName() const
{
  return p_fname;
}

bool KUser::getDisabled() const
{
  return isDisabled;
}

// FreeBSD apparently uses the GECOS fields differently than other Unices.
// Create some better named functions to make the FreeBSD code clear
const QString &KUser::getOffice() const
{
  return p_office;
}

const QString &KUser::getWorkPhone() const
{
  return p_ophone;
}

const QString &KUser::getHomePhone() const
{
  return p_hphone;
}

// New fields needed for the FreeBSD /etc/master.passwd file
const QString &KUser::getClass() const
{
  return p_class;
}

const QString &KUser::getOffice1() const
{
  return p_office1;
}

const QString &KUser::getOffice2() const
{
  return p_office2;
}

const QString &KUser::getAddress() const
{
  return p_address;
}

uid_t KUser::getUID() const
{
  return p_uid;
}

gid_t KUser::getGID() const
{
  return p_gid;
}

const QString &KUser::getSPwd() const
{
  return s_pwd;
}

time_t KUser::getLastChange() const
{
  return p_change;
}

int KUser::getMin() const
{
  return s_min;
}

int KUser::getMax() const
{
  return s_max;
}

int KUser::getWarn() const
{
  return s_warn;
}

int KUser::getInactive() const
{
  return s_inact;
}

int KUser::getFlag() const
{
  return s_flag;
}

time_t KUser::getExpire() const
{
  return p_expire;
}

const QString &KUser::getLMPwd() const //  sam_lmpwd,
{
  return sam_lmpwd;
}

const QString &KUser::getNTPwd() const //sam_ntpwd,
{
  return sam_ntpwd;
}

const QString &KUser::getLoginScript() const //sam_loginscript,
{
  return sam_loginscript;
}

const QString &KUser::getProfilePath() const //  sam_profile,
{
  return sam_profile;
}

const QString &KUser::getHomeDrive() const //sam_homedrive,
{
  return sam_homedrive;
}

const QString &KUser::getHomePath() const //sam_homepath;
{
  return sam_homepath;
}

const SID &KUser::getSID() const //sid,
{
  return sid;
}

const SID &KUser::getPGSID() const //pgroup_sid;
{
  return pgroup_sid;
}

void KUser::setName(const QString &data)
{
  p_name = data;
}

void KUser::setSurname(const QString &data)
{
  p_surname = data;
}

void KUser::setEmail(const QString &data)
{
  p_email = data;
}

void KUser::setPwd(const QString &data)
{
  p_pwd = data;
}

void KUser::setHomeDir(const QString &data)
{
  p_dir = data;
}

void KUser::setShell(const QString &data)
{
  p_shell = data;
}

void KUser::setFullName(const QString &data)
{
  p_fname = data;
}

void KUser::setDisabled(bool data)
{
  isDisabled = data;
}

// FreeBSD apparently uses the GECOS fields differently than other Unices.
// Create some better named functions to make the FreeBSD code clear
void KUser::setOffice(const QString &data)
{
  p_office = data;
}

void KUser::setWorkPhone(const QString &data)
{
  p_ophone = data;
}

void KUser::setHomePhone(const QString &data)
{
  p_hphone = data;
}

// New fields needed for the FreeBSD /etc/master.passwd file
void KUser::setClass(const QString &data)
{
  p_class = data;
}

void KUser::setLastChange(time_t data)
{
  p_change = data;
}

void KUser::setExpire(time_t data)
{
  p_expire = data;
}

void KUser::setOffice1(const QString &data)
{
  p_office1 = data;
}

void KUser::setOffice2(const QString &data)
{
  p_office2 = data;
}

void KUser::setAddress(const QString &data)
{
  p_address = data;
}

void KUser::setUID(uid_t data)
{
  p_uid = data;
}

void KUser::setGID(gid_t data)
{
  p_gid = data;
}

void KUser::setSPwd(const QString &data)
{
  s_pwd = data;
}

void KUser::setMin(int data)
{
  s_min = data;
}

void KUser::setMax(int data)
{
  s_max = data;
}

void KUser::setWarn(int data)
{
  s_warn = data;
}

void KUser::setInactive(int data)
{
  s_inact = data;
}

void KUser::setLMPwd( const QString &data ) //  sam_lmpwd,
{
  sam_lmpwd = data;
}

void KUser::setNTPwd( const QString &data ) //sam_ntpwd,
{
  sam_ntpwd = data;
}

void KUser::setLoginScript( const QString &data ) //sam_loginscript,
{
  sam_loginscript = data;
}

void KUser::setProfilePath( const QString &data) //  sam_profile,
{
  sam_profile = data;
}

void KUser::setHomeDrive( const QString &data ) //sam_homedrive,
{
  sam_homedrive = data;
}

void KUser::setHomePath( const QString &data ) //sam_homepath;
{
  sam_homepath = data;
}

void KUser::setSID( const SID &data ) //sid,
{
  sid = data;
}

void KUser::setPGSID( const SID &data ) //pgroup_sid;
{
  pgroup_sid = data;
}

void KUser::setFlag(int data)
{
  s_flag = data;
}

void KUser::setCreateHome(bool data)
{
  isCreateHome = data;
}

void KUser::setCreateMailBox(bool data)
{
  isCreateMailBox = data;
}

void KUser::setCopySkel(bool data)
{
  isCopySkel = data;
}

void KUser::setDeleteHome(bool data)
{
  isDeleteHome = data;
}

void KUser::setDeleteMailBox(bool data)
{
  isDeleteMailBox = data;
}

int KUser::createHome()
{

  if(p_dir.isNull() || p_dir.isEmpty()) {
    KMessageBox::sorry( 0, i18n("Cannot create home folder for %1: it is null or empty.").arg(p_name) );
    return(0);
  }
  if (mkdir(QFile::encodeName(p_dir), 0700) != 0) {
    if (errno != EEXIST)
    {
      KMessageBox::error( 0, i18n("Cannot create home folder %1.\nError: %2").arg(p_dir).arg(QString::fromLocal8Bit(strerror(errno))) );
      return(0);
    }
  }

  if (chown(QFile::encodeName(p_dir), p_uid, p_gid) != 0) {
    KMessageBox::error( 0, i18n("Cannot change owner of home folder %1.\nError: %2").arg(p_dir).arg(QString::fromLocal8Bit(strerror(errno))) );
    return(1);
  }

  if (chmod(QFile::encodeName(p_dir), KU_HOMEDIR_PERM) != 0) {
    KMessageBox::error( 0, i18n("Cannot change permissions on home folder %1.\nError: %2").arg(p_dir).arg(QString::fromLocal8Bit(strerror(errno))) );
    return(1);
  }
  return(1);
}

int KUser::tryCreate(const QString &dir)
{
  struct stat sb;
  int rc = 0;

  rc = stat(QFile::encodeName(dir), &sb);
  if (rc == 0) {
    if (S_ISDIR(sb.st_mode)) {
      if (KMessageBox::warningContinueCancel( 0,
        i18n("Folder %1 already exists!\nWill make %2 owner and change permissions.\nDo you want to continue?").arg(dir).arg(p_name),
        QString::null, KStdGuiItem::cont() ) == KMessageBox::Continue) {

        if (chown(QFile::encodeName(dir), p_uid, p_gid) != 0) {
          KMessageBox::error( 0, i18n("Cannot change owner of %1 folder.\nError: %2") .arg(dir).arg(QString::fromLocal8Bit(strerror(errno))) );
        }
        return(0);
      } else {
        KMessageBox::information( 0, i18n("Folder %1 left 'as is'.\nVerify ownership and permissions for user %2 who may not be able to log in!").arg(dir).arg(p_name) );
        return(-1);
      }
    } else {
      KMessageBox::information( 0, i18n("%1 exists and is not a folder. User %2 will not be able to log in!").arg(dir).arg(p_name) );
      return(-1);
    }
  } else {
    if (errno == ENOENT) {
      if (mkdir(QFile::encodeName(dir), 0700) != 0) {
        KMessageBox::error( 0, i18n("Cannot create %1 folder.\nError: %2").arg(dir).arg(QString::fromLocal8Bit(strerror(errno))));
        return(-1);
      }
      if (chown(QFile::encodeName(dir), p_uid, p_gid) != 0) {
        KMessageBox::error( 0, i18n("Cannot change owner of %1 folder.\nError: %2").arg(dir).arg(QString::fromLocal8Bit(strerror(errno))) );
      }
      return(0);
    } else {
      KMessageBox::error( 0, i18n("stat call on %1 failed.\nError: %2").arg(dir).arg(QString::fromLocal8Bit(strerror(errno))) );
      return(-1);
    }
  }
}

int KUser::createMailBox()
{
  QString mailboxpath;
  int fd;
  mailboxpath = QString::fromLatin1("%1/%2").arg(QFile::decodeName(MAIL_SPOOL_DIR)).arg(p_name);
  if((fd = open(QFile::encodeName(mailboxpath), O_CREAT|O_EXCL|O_WRONLY,
                S_IRUSR|S_IWUSR)) < 0) {
    if (errno != EEXIST)
    {
      KMessageBox::error( 0, i18n("Cannot create %1: %2")
                .arg(mailboxpath)
                .arg(QString::fromLocal8Bit(strerror(errno))) );
      return -1;
    }
  }

  close(fd);

  if (chown(QFile::encodeName(mailboxpath), p_uid, KU_MAILBOX_GID) != 0) {
    KMessageBox::error( 0, i18n("Cannot change owner on mailbox: %1\nError: %2")
                .arg(mailboxpath).arg(QString::fromLocal8Bit(strerror(errno))) );
    return -1;
  }

  if (chmod(QFile::encodeName(mailboxpath), KU_MAILBOX_PERM) != 0) {
    KMessageBox::error( 0, i18n("Cannot change permissions on mailbox: %1\nError: %2")
                .arg(mailboxpath).arg(QString::fromLocal8Bit(strerror(errno))) );
    return -1;
  }

  return 0;
}

void KUser::copyDir(const QString &srcPath, const QString &dstPath)
{
  mode_t mode;
  QDir s(srcPath);
  QDir d(dstPath);

  QString dot = QString::fromLatin1(".");
  QString dotdot = QString::fromLatin1("..");
  
  s.setFilter( QDir::All | QDir::Hidden | QDir::System );

  for (uint i=0; i<s.count(); i++) {
    QString name(s[i]);

    if (name == dot)
      continue;
    if (name == dotdot)
      continue;

    QString filename(s.filePath(name));
    
    QFileInfo info(filename);
    mode = 0;
    if ( info.permission(QFileInfo::ReadOwner) ) mode |=  S_IRUSR;
    if ( info.permission(QFileInfo::WriteOwner) ) mode |=  S_IWUSR;
    if ( info.permission(QFileInfo::ExeOwner) ) mode |=  S_IXUSR;
    if ( info.permission(QFileInfo::ReadGroup) ) mode |=  S_IRGRP;
    if ( info.permission(QFileInfo::WriteGroup) ) mode |=  S_IWGRP;
    if ( info.permission(QFileInfo::ExeGroup) ) mode |=  S_IXGRP;
    if ( info.permission(QFileInfo::ReadOther) ) mode |=  S_IROTH;
    if ( info.permission(QFileInfo::WriteOther) ) mode |=  S_IWOTH;
    if ( info.permission(QFileInfo::ExeOther) ) mode |=  S_IXOTH;
    
    if ( info.isSymLink() ) {
      QString link = info.readLink();
      
      if (symlink(QFile::encodeName(link),QFile::encodeName(d.filePath(name))) != 0) {
        KMessageBox::error( 0, i18n("Error creating symlink %1.\nError: %2")
                  .arg(d.filePath(s[i])).arg(QString::fromLocal8Bit(strerror(errno))) );
      }
    } else if ( info.isDir() ) {
      QDir dir(filename);

      d.mkdir(name, FALSE);
      copyDir(s.filePath(name), d.filePath(name));

      if (chown(QFile::encodeName(d.filePath(name)), p_uid, p_gid) != 0) {
        KMessageBox::error( 0, i18n("Cannot change owner of folder %1.\nError: %2")
                  .arg(d.filePath(s[i])).arg(QString::fromLocal8Bit(strerror(errno))) );
      }

      if (chmod(QFile::encodeName(d.filePath(name)), mode) != 0) {
        KMessageBox::error( 0, i18n("Cannot change permissions on folder %1.\nError: %2")
                  .arg(d.filePath(s[i])).arg(QString::fromLocal8Bit(strerror(errno))) );
      }

    } else {
      if (copyFile(filename, d.filePath(name)) == -1) {
        continue;
      }

      if (chown(QFile::encodeName(d.filePath(name)), p_uid, p_gid) != 0) {
        KMessageBox::error( 0, i18n("Cannot change owner of file %1.\nError: %2")
                  .arg(d.filePath(s[i])).arg(QString::fromLocal8Bit(strerror(errno))) );
      }

      if (chmod(QFile::encodeName(d.filePath(name)), mode) != 0) {
        KMessageBox::error( 0, i18n("Cannot change permissions on file %1.\nError: %2")
                  .arg(d.filePath(s[i])).arg(QString::fromLocal8Bit(strerror(errno))) );
      }
    }
  }
}

int KUser::copySkel()
{
  QDir s(QFile::decodeName(SKELDIR));
  QDir d(p_dir);
  mode_t mode;
  
  if (!s.exists()) {
    KMessageBox::error( 0, i18n("Folder %1 does not exist, cannot copy skeleton for %2.").arg(s.absPath()).arg(p_name) );
    return (-1);
  }

  if (!d.exists()) {
    KMessageBox::error( 0, i18n("Folder %1 does not exist, cannot copy skeleton.").arg(d.absPath()) );
    return (-1);
  }
  
  mode = umask(0007);
  copyDir(s.absPath(), d.absPath());
  umask( mode );
  
  return 0;
}

// Temporarily use rm
//TODO: replace by our own procedure cause calling other programs
//      for things we are know how to do is not a good idea

int KUser::removeHome()
{
  struct stat sb;
  QString command;

  if (!stat(QFile::encodeName(p_dir), &sb))
    if (S_ISDIR(sb.st_mode) && sb.st_uid == p_uid) {
#ifdef MINIX
      command = QString::fromLatin1("/usr/bin/rm -rf -- %1").arg(KProcess::quote(p_dir));
#else
      command = QString::fromLatin1("/bin/rm -rf -- %1").arg(KProcess::quote(p_dir));
#endif
      if (system(QFile::encodeName(command)) != 0) {
             KMessageBox::error( 0, i18n("Cannot remove home folder %1.\nError: %2")
                       .arg(command).arg(QString::fromLocal8Bit(strerror(errno))) );
      }
    } else {
      KMessageBox::error( 0, i18n("Removal of home folder %1 failed (uid = %2, gid = %3).").arg(p_dir).arg(sb.st_uid).arg(sb.st_gid) );
    }
  else {
    KMessageBox::error( 0, i18n("stat call on file %1 failed.\nError: %2")
                 .arg(p_dir).arg(QString::fromLocal8Bit(strerror(errno))) );
  }

  return 0;
}

//TODO: remove at jobs too.

int KUser::removeCrontabs()
{
  QString file;
  QString command;

  file = QString::fromLatin1("/var/cron/tabs/%1").arg(p_name);
  if ( access(QFile::encodeName(file), F_OK) == 0 ) {
    command = QString::fromLatin1("crontab -u %1 -r").arg(KProcess::quote(p_name));
    if ( system(QFile::encodeName(command)) != 0 ) {
      KMessageBox::error( 0, i18n("Cannot remove crontab %1.\nError: %2")
                  .arg(command).arg(QString::fromLocal8Bit(strerror(errno))) );
     }
  }

  return 0;
}

int KUser::removeMailBox()
{
  QString file;

  file = QString::fromLatin1("%1/%2").arg(QFile::decodeName(MAIL_SPOOL_DIR)).arg(p_name);
  if (remove(QFile::encodeName(file)) != 0) {
    KMessageBox::error( 0, i18n("Cannot remove mailbox %1.\nError: %2")
                .arg(file).arg(QString::fromLocal8Bit(strerror(errno))) );
  }

  return 0;
}

int KUser::removeProcesses()
{
  // be paranoid -- kill all processes owned by that user, if not root.

  if (p_uid != 0)
    switch (fork()) {
      case 0:
        setuid(p_uid);
        kill(-1, 9);
        _exit(0);
        break;
      case -1:
        KMessageBox::error( 0,
          i18n("Cannot fork while trying to kill processes for uid %1.").arg(p_uid) );
        break;
    }

  return 0;
}

KUsers::KUsers(KUserPrefsBase *cfg)
{
  mUsers.setAutoDelete(TRUE);
  mCfg = cfg;
}

KUsers::~KUsers()
{
  mUsers.clear();
}

const QString &KUsers::getDOMSID() const
{
  return domsid;
}

void KUsers::fillGecos(KUser *user, const char *gecos)
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
      case 0: user->setFullName(val); break;
      case 1: caps & Cap_BSD ? user->setOffice(val) : user->setOffice1(val); break;
      case 2: caps & Cap_BSD ? user->setWorkPhone(val) : user->setOffice2(val); break;
      case 3: caps & Cap_BSD ? user->setHomePhone(val) : user->setAddress(val); break;
    }
    if(pos == NULL) break;
    s = pos+1;
    no++;
  }
}

bool KUsers::doCreate(KUser *user)
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
  return TRUE;
}

bool KUsers::doDelete( KUser *user )
{
  kdDebug() << "delete user: " << user->getName() << " uid: " << user->getUID() << endl;
  if ( user->isDeleteHome ) {
    user->removeHome();
    user->removeCrontabs();
  }
  if ( user->isDeleteMailBox )
    user->removeMailBox();
/*
  user->removeProcesses();
*/
  return TRUE;
}

KUser *KUsers::lookup(const QString & name)
{
  KUser *user;
  QPtrListIterator<KUser> it( mUsers );

  while ( (user = it.current()) != 0 && user->getName() != name ) ++it;
  return user;
}

KUser *KUsers::lookup(uid_t uid)
{
  KUser *user;
  QPtrListIterator<KUser> it( mUsers );

  while ( (user = it.current()) != 0 && user->getUID() != uid ) ++it;
  return user;
}

KUser *KUsers::lookup_sam( const SID &sid )
{
  KUser *user;
  QPtrListIterator<KUser> it( mUsers );

  while ( (user = it.current()) != 0 && user->getSID() != sid ) ++it;
  return user;
}

KUser *KUsers::lookup_sam( const QString &sid )
{
  KUser *user;
  QPtrListIterator<KUser> it( mUsers );

  while ( (user = it.current()) != 0 && user->getSID().getSID() != sid ) ++it;
  return user;
}

KUser *KUsers::lookup_sam( uint rid )
{
  KUser *user;
  QPtrListIterator<KUser> it( mUsers );

  while ( (user = it.current()) != 0 && user->getSID().getRID() != rid ) ++it;
  return user;
}

uid_t KUsers::first_free()
{
  uid_t t;

  for (t = mCfg->firstUID() ; t<65534; t++)
    if (lookup(t) == NULL)
      return t;

  return NO_FREE;
}

uint KUsers::first_free_sam()
{
  uint t;

  for (t = 1000; t<65534; t++)
    if (lookup_sam(t) == NULL)
      return t;

  return 0;
}

uint KUsers::count() const
{
  return mUsers.count();
}

KUser *KUsers::operator[](uint num)
{
  return mUsers.at(num);
}

KUser *KUsers::first()
{
  return mUsers.first();
}

KUser *KUsers::next()
{
  return mUsers.next();
}

void KUsers::add(KUser *user)
{
  mAdd.append( user );
}

void KUsers::del(KUser *user)
{
  mDel.append( user );
}

void KUsers::mod(KUser *uold, const KUser &unew)
{
  mMod.insert( uold, unew );
}

void KUsers::commit()
{
  kdDebug() << "KUsers::commit()" << endl;
  KUser *user;
  DelIt dit( mDelSucc );
  AddIt ait( mAddSucc );
  ModIt mit = mModSucc.begin();

//commit deletes
  while ( (user = dit.current()) != 0 ) {
    ++dit;
    doDelete( user );
    mUsers.remove( user );
  }
//commit additions
  while ( (user = ait.current()) != 0 ) {
    ++ait;
    doCreate( user );
    mUsers.append( user );
  }
//commit modifications
  while ( mit != mModSucc.end() ) {
    *(mit.key()) = mit.data();
    mit++;
  }

//clear the unsuccessful modifications
  cancelMods();
}

void KUsers::cancelMods()
{
  KUser *user;
  while ( (user = mAdd.first()) ) {
    delete user;
    mAdd.remove();
  }
  mDel.clear();
  mMod.clear();
}
