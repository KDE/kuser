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
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_SHADOW
#include <shadow.h>
#endif

#include <qstring.h>
#include <qdir.h>

#include "kglobal_.h"
#include "kuserfiles.h"
#include "misc.h"
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include "editDefaults.h"

KUserFiles::KUserFiles(KUserPrefsBase *cfg) : KUsers( cfg )
{
  pw_backuped = FALSE;
  pn_backuped = FALSE;
  s_backuped = FALSE;

  pwd_mode = 0644;
  pwd_uid = 0;
  pwd_gid = 0;

  sdw_mode = 0600;
  sdw_uid = 0;
  sdw_gid = 0;

  mUsers.setAutoDelete(TRUE);

  caps = Cap_Passwd;
#ifdef HAVE_SHADOW
  if ( !mCfg->shadowsrc().isEmpty() ) caps |= Cap_Shadow;
#endif
#if defined(__FreeBSD__) || defined(__bsdi__)
  caps |= Cap_BSD;
#endif

  reload();
}

KUserFiles::~KUserFiles()
{
}

bool KUserFiles::reload() {
  if (!loadpwd())
    return FALSE;

  if (!loadsdw())
    return FALSE;

  return TRUE;
}

// Load passwd file

bool KUserFiles::loadpwd()
{
  passwd *p;
  KU::KUser *tmpKU = 0;
  struct stat st;
  QString filename;
  QString passwd_filename;
  QString nispasswd_filename;
  int rc = 0;
  int passwd_errno = 0;
  int nispasswd_errno = 0;
  char processing_file = '\0';
  #define P_PASSWD    0x01
  #define P_NISPASSWD 0x02
  #define MAXFILES 2

  // Read KUser configuration

  passwd_filename = mCfg->passwdsrc();
  nispasswd_filename = mCfg->nispasswdsrc();

  // Handle unconfigured environments

  if(passwd_filename.isEmpty() && nispasswd_filename.isEmpty()) {
    mCfg->setPasswdsrc( PASSWORD_FILE );
    mCfg->setGroupsrc( GROUP_FILE );
    passwd_filename = mCfg->passwdsrc();
    KMessageBox::error( 0, i18n("KUser sources were not configured.\nLocal passwd source set to %1\nLocal group source set to %2.").arg(mCfg->passwdsrc().arg(mCfg->groupsrc())) );
  }

  if(!passwd_filename.isEmpty()) {
    processing_file = processing_file | P_PASSWD;
    filename.append(passwd_filename);
  }

  // Start reading passwd file(s)

  for(int i = 0; i < MAXFILES; i++) {
    rc = stat(QFile::encodeName(filename), &st);
    if(rc != 0) {
      KMessageBox::error( 0, i18n("Stat call on file %1 failed: %2\nCheck KUser settings.").arg(filename).arg(QString::fromLocal8Bit(strerror(errno))) );
      if( (processing_file & P_PASSWD) != 0 ) {
        passwd_errno = errno;
        if(!nispasswd_filename.isEmpty()) {
          processing_file = processing_file & ~P_PASSWD;
          processing_file = processing_file | P_NISPASSWD;
          filename.truncate(0);
          filename.append(nispasswd_filename);
        }
        continue;
      }
      else{
        nispasswd_errno = errno;
        break;
      }
    }

    pwd_mode = st.st_mode & 0666;
    pwd_uid = st.st_uid;
    pwd_gid = st.st_gid;

    // We are reading our configuration specified passwd file
    QString tmp;

#ifdef HAVE_FGETPWENT
    FILE *fpwd = fopen(QFile::encodeName(filename), "r");
    if(fpwd == NULL) {
      KMessageBox::error( 0, i18n("Error opening %1 for reading.").arg(filename) );
      return FALSE;
    }

    while ((p = fgetpwent(fpwd)) != NULL) {
#else
    setpwent(); //This should be enough for BSDs
    while ((p = getpwent()) != NULL) {
#endif
      tmpKU = new KU::KUser();
      tmpKU->setCaps( KU::KUser::Cap_POSIX );
      tmpKU->setUID(p->pw_uid);
      tmpKU->setGID(p->pw_gid);
      tmpKU->setName(QString::fromLocal8Bit(p->pw_name));
      tmp  = QString::fromLocal8Bit( p->pw_passwd );
      if ( tmp != "x" && tmp != "*" && !tmp.startsWith("!") )
        tmpKU->setDisabled( false );
      else
        tmpKU->setDisabled( true );
      if ( tmp.startsWith("!") ) tmp.remove(0, 1);
      tmpKU->setPwd( tmp );
      tmpKU->setHomeDir(QString::fromLocal8Bit(p->pw_dir));
      tmpKU->setShell(QString::fromLocal8Bit(p->pw_shell));
#if defined(__FreeBSD__) || defined(__bsdi__)
      tmpKU->setClass(QString::fromLatin1(p->pw_class));
      tmpKU->setLastChange(p->pw_change);
      tmpKU->setExpire(p->pw_expire);
#endif

      if ((p->pw_gecos != 0) && (p->pw_gecos[0] != 0))
        fillGecos(tmpKU, p->pw_gecos);
      mUsers.append(tmpKU);
    }

    // End reading passwd_filename

#ifdef HAVE_FGETPWENT
    fclose(fpwd);
#else
    endpwent();
#endif
    if((!nispasswd_filename.isEmpty()) && (nispasswd_filename != passwd_filename)) {
      processing_file = processing_file & ~P_PASSWD;
      processing_file = processing_file | P_NISPASSWD;
      filename.truncate(0);
      filename.append(nispasswd_filename);
    }
    else
      break;

  } // end of processing files, for loop

  if( (passwd_errno == 0) && (nispasswd_errno == 0) )
    return (TRUE);
  if( (passwd_errno != 0) && (nispasswd_errno != 0) )
    return (FALSE);
  else
    return(TRUE);
}

// Load shadow passwords

bool KUserFiles::loadsdw()
{
#ifdef HAVE_SHADOW
  QString shadow_file,tmp;
  struct spwd *spw;
  KU::KUser *up = NULL;
  struct stat st;

  shadow_file = mCfg->shadowsrc();
  if ( shadow_file.isEmpty() )
    return TRUE;

  stat( QFile::encodeName(shadow_file), &st);
  sdw_mode = st.st_mode & 0666;
  sdw_uid = st.st_uid;
  sdw_gid = st.st_gid;

#ifdef HAVE_FGETSPENT
  FILE *f;
  kdDebug() << "open shadow file: " << shadow_file << endl;
  if ((f = fopen( QFile::encodeName(shadow_file), "r")) == NULL) {
    KMessageBox::error( 0, i18n("Error opening %1 for reading.").arg(shadow_file) );
    caps &= ~Cap_Shadow;
    return TRUE;
  }
  while ((spw = fgetspent( f ))) {     // read a shadow password structure
#else
  setspent();
  while ((spw = getspent())) {     // read a shadow password structure
#endif

    kdDebug() << "shadow entry: " << spw->sp_namp << endl;
    if ((up = lookup(QString::fromLocal8Bit(spw->sp_namp))) == NULL) {
      KMessageBox::error( 0, i18n("No /etc/passwd entry for %1.\nEntry will be removed at the next `Save'-operation.").arg(QString::fromLocal8Bit(spw->sp_namp)) );
      continue;
    }

    tmp = QString::fromLocal8Bit( spw->sp_pwdp );
    if ( tmp.startsWith("!!") || tmp == "*" ) {
      up->setDisabled( true );
      tmp.remove( 0, 2 );
    } else
      up->setDisabled( false );

    up->setSPwd( tmp );        // cp the encrypted pwd
    up->setLastChange( daysToTime( spw->sp_lstchg ) );
    up->setMin(spw->sp_min);
    up->setMax(spw->sp_max);
#ifndef _SCO_DS
    up->setWarn(spw->sp_warn);
    up->setInactive(spw->sp_inact);
    up->setExpire( daysToTime( spw->sp_expire ) );
    up->setFlag(spw->sp_flag);
#endif
  }

#ifdef HAVE_FGETSPENT
  fclose(f);
#else
  endspent();
#endif

#endif // HAVE_SHADOW
  return TRUE;
}

// Save password file

#define escstr(a,b) tmp2 = user->a(); \
                    tmp2.replace(':',"_"); \
                    tmp2.replace(',',"_"); \
                    user->b( tmp2 );


bool KUserFiles::savepwd()
{
  FILE *passwd_fd = NULL;
  FILE *nispasswd_fd = NULL;
  uid_t minuid = 0;
  int nis_users_written = 0;
  uid_t tmp_uid = 0;
  QString s;
  QString s1;
  QString tmp, tmp2;
  QString passwd_filename;
  QString nispasswd_filename;


  char errors_found = '\0';
    #define NOMINUID    0x01
    #define NONISPASSWD 0x02

  // Read KUser configuration info

  passwd_filename = mCfg->passwdsrc();
  nispasswd_filename = mCfg->nispasswdsrc();
  QString new_passwd_filename =
    passwd_filename + QString::fromLatin1(KU_CREATE_EXT);
  QString new_nispasswd_filename =
    nispasswd_filename+QString::fromLatin1(KU_CREATE_EXT);

  if( nispasswd_filename != passwd_filename ) {
    minuid = mCfg->nisminuid();
  }

  // Backup file(s)

  if(!passwd_filename.isEmpty()) {
    if (!pw_backuped) {
      if (!backup(passwd_filename)) return FALSE;
      pw_backuped = TRUE;
    }
  }
  if(!nispasswd_filename.isEmpty() &&
    (nispasswd_filename != passwd_filename)) {
    if (!pn_backuped) {
      if (!backup(nispasswd_filename)) return FALSE;
      pn_backuped = TRUE;
    }
  }

  // Open file(s)

  if(!passwd_filename.isEmpty()) {
    if ((passwd_fd =
      fopen(QFile::encodeName(new_passwd_filename),"w")) == NULL)
        KMessageBox::error( 0, i18n("Error opening %1 for writing.").arg(passwd_filename) );
  }

  if(!nispasswd_filename.isEmpty() && (nispasswd_filename != passwd_filename)){
    if ((nispasswd_fd =
      fopen(QFile::encodeName(new_nispasswd_filename),"w")) == NULL)
        KMessageBox::error( 0, i18n("Error opening %1 for writing.").arg(nispasswd_filename) );
  }

  QPtrListIterator<KU::KUser> it( mUsers );
  KU::KUser *user;
  bool addok = false;
  user = (*it);
  while (true) {
    if ( user == 0 ) {
      if ( addok ) break;
      it = QPtrListIterator<KU::KUser> ( mAdd );
      user = (*it);
      addok = true;
      if ( user == 0 ) break;
    };
    if ( mDel.containsRef( user ) ) {
      ++it;
      user = (*it);
      continue;
    }
    if ( mMod.contains( user ) ) user = &( mMod[ user ] );

    tmp_uid = user->getUID();
    if ( caps & Cap_Shadow )
      tmp = "x";
    else {
      tmp = user->getPwd();
      if ( user->getDisabled() && tmp != "x" && tmp != "*" )
        tmp = "!" + tmp;
    }

    escstr( getName, setName );
    escstr( getHomeDir, setHomeDir );
    escstr( getShell, setShell );
    escstr( getName, setName );
    escstr( getFullName, setFullName );
#if defined(__FreeBSD__) || defined(__bsdi__)
    escstr( getClass, setClass );
    escstr( getOffice, setOffice );
    escstr( getWorkPhone, setWorkPhone );
    escstr( getHomePhone, setHomePhone );
    s =
      user->getName() + ":" +
      tmp + ":" +
      QString::number( user->getUID() ) + ":" +
      QString::number( user->getGID() ) + ":" +
      user->getClass() + ":" +
      QString::number( user->getLastChange() ) + ":" +
      QString::number( user->getExpire() ) + ":";

    s1 =
      user->getFullName() + "," +
      user->getOffice() + "," +
      user->getWorkPhone() + "," +
      user->getHomePhone();
#else
    escstr( getOffice1, setOffice1 );
    escstr( getOffice2, setOffice2 );
    escstr( getAddress, setAddress );
    s =
      user->getName() + ":" +
      tmp + ":" +
      QString::number( user->getUID() ) + ":" +
      QString::number( user->getGID() ) + ":";

    s1 =
      user->getFullName() + "," +
      user->getOffice1() + "," +
      user->getOffice2() + "," +
      user->getAddress();

#endif
    for (int j=(s1.length()-1); j>=0; j--) {
      if (s1[j] != ',')
        break;
      s1.truncate(j);
    }

    s += s1 + ":" +
      user->getHomeDir() + ":" +
      user->getShell() + "\n";

    if( (nispasswd_fd != 0) && (minuid != 0) ) {
      if (minuid <= tmp_uid) {
        fputs(s.local8Bit().data(), nispasswd_fd);
        nis_users_written++;
        ++it;
        user = (*it);
        continue;
      }
    }

    if( (nispasswd_fd != 0) && (minuid == 0) ) {
      errors_found = errors_found | NOMINUID;
    }

    if( (nispasswd_fd == 0) && (minuid != 0) ) {
      errors_found = errors_found | NONISPASSWD;
    }
    kdDebug() << s << endl;
    fputs(s.local8Bit().data(), passwd_fd);

    ++it;
    user = (*it);
  }

  if(passwd_fd) {
    fclose(passwd_fd);
    chmod(QFile::encodeName(new_passwd_filename), pwd_mode);
    chown(QFile::encodeName(new_passwd_filename), pwd_uid, pwd_gid);
    rename(QFile::encodeName(new_passwd_filename),
      QFile::encodeName(passwd_filename));
  }

  if(nispasswd_fd) {
    fclose(nispasswd_fd);
    chmod(QFile::encodeName(new_nispasswd_filename), pwd_mode);
    chown(QFile::encodeName(new_nispasswd_filename), pwd_uid, pwd_gid);
    rename(QFile::encodeName(new_nispasswd_filename),
      QFile::encodeName(nispasswd_filename));
  }

  if( (errors_found & NOMINUID) != 0 ) {
    KMessageBox::error( 0, i18n("Unable to process NIS passwd file without a minimum UID specified.\nPlease update KUser settings (Files).") );
  }

  if( (errors_found & NONISPASSWD) != 0 ) {
    KMessageBox::error( 0, i18n("Specifying NIS minimum UID requires NIS file(s).\nPlease update KUser settings (Files).") );
  }

  // need to run a utility program to build /etc/passwd, /etc/pwd.db
  // and /etc/spwd.db from /etc/master.passwd
#if defined(__FreeBSD__) || defined(__bsdi__)
  if (system(PWMKDB) != 0) {
    KMessageBox::error( 0, i18n("Unable to build password database.") );
    return FALSE;
  }
#else
  if( (nis_users_written > 0) || (nispasswd_filename == passwd_filename) ) {
    if (system(PWMKDB) != 0) {
      KMessageBox::error( 0, i18n("Unable to build password databases.") );
      return FALSE;
    }
  }
#endif

  return TRUE;
}

#undef escstr

// Save shadow passwords file

bool KUserFiles::savesdw()
{
#ifdef HAVE_SHADOW
  bool addok = false;
  QString tmp;
  FILE *f;
  struct spwd *spwp;
  struct spwd s;
  KU::KUser *up;
  QString shadow_file = mCfg->shadowsrc();
  QString new_shadow_file = shadow_file+QString::fromLatin1(KU_CREATE_EXT);

  if ( shadow_file.isEmpty() )
    return TRUE;

  if (!s_backuped) {
    if (!backup(shadow_file)) return FALSE;
    s_backuped = TRUE;
  }

  if ((f = fopen(QFile::encodeName(new_shadow_file), "w")) == NULL) {
    KMessageBox::error( 0, i18n("Error opening %1 for writing.").arg(new_shadow_file) );
    return FALSE;
  }

  s.sp_namp = (char *)malloc(200);
  s.sp_pwdp = (char *)malloc(200);

  QPtrListIterator<KU::KUser> it( mUsers );
  up = (*it);
  while (true) {

    if ( up == 0 ) {
      if ( addok ) break;
      it = QPtrListIterator<KU::KUser> ( mAdd );
      up = (*it);
      addok = true;
      if ( up == 0 ) break;
    };

    if ( mDel.containsRef( up ) ) {
      ++it;
      up = (*it);
      continue;
    }
    if ( mMod.contains( up ) ) up = &( mMod[ up ] );

    strncpy( s.sp_namp, up->getName().local8Bit(), 200 );
    if ( up->getDisabled() )
      strncpy( s.sp_pwdp, QString("!!" + up->getSPwd()).local8Bit(), 200 );
    else
      strncpy( s.sp_pwdp, up->getSPwd().local8Bit(), 200 );

    s.sp_lstchg = timeToDays( up->getLastChange() );
    s.sp_min    = up->getMin();
    s.sp_max    = up->getMax();
#ifndef _SCO_DS
    s.sp_warn   = up->getWarn();
    s.sp_inact  = up->getInactive();
    s.sp_expire = timeToDays( up->getExpire() );
    s.sp_flag   = up->getFlag();
#endif
    spwp = &s;
    putspent(spwp, f);

    ++it;
    up = (*it);
  }
  fclose(f);

  chmod(QFile::encodeName(new_shadow_file), sdw_mode);
  chown(QFile::encodeName(new_shadow_file), sdw_uid, sdw_gid);
  rename(QFile::encodeName(new_shadow_file),
    QFile::encodeName(shadow_file));

  free(s.sp_namp);
  free(s.sp_pwdp);
#endif // HAVE_SHADOW
  return TRUE;
}


void KUserFiles::createPassword( KU::KUser *user, const QString &password )
{
  if ( caps & Cap_Shadow ) {
    user->setSPwd( encryptPass( password, mCfg->md5shadow() ) );
    user->setPwd( QString::fromLatin1("x") );
  } else
    user->setPwd( encryptPass( password, false ) );
}

bool KUserFiles::dbcommit()
{
  bool ret;
  mode_t mode;

  mAddSucc.clear();
  mDelSucc.clear();
  mModSucc.clear();
  if ( mDel.isEmpty() && mAdd.isEmpty() && mMod.isEmpty() )
    return true;

  mode = umask(0077);
  ret = savepwd();
  if ( ret && ( caps & Cap_Shadow ) ) ret = savesdw();
  umask( mode );
  if ( !ret ) return false;

  mDelSucc = mDel;
  mAddSucc = mAdd;
  mModSucc = mMod;
  mDel.clear();
  mAdd.clear();
  mMod.clear();
  return TRUE;
}
