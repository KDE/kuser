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

#include <config.h>

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
#ifdef HAVE_SHADOW
#include <shadow.h>
#endif

#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>

#include "ku_userfiles.h"
#include "ku_misc.h"


KU_UserFiles::KU_UserFiles(KU_PrefsBase *cfg) : KU_Users( cfg )
{
  pw_backuped = false;
  pn_backuped = false;
  s_backuped = false;

  pwd_mode = 0644;
  pwd_uid = 0;
  pwd_gid = 0;

  sdw_mode = 0600;
  sdw_uid = 0;
  sdw_gid = 0;

  caps = Cap_Passwd;
#ifdef HAVE_SHADOW
  if ( !mCfg->shadowsrc().isEmpty() ) caps |= Cap_Shadow;
#endif
#if defined(__FreeBSD__) || defined(__bsdi__)
  caps |= Cap_BSD;
#endif

  reload();
}

KU_UserFiles::~KU_UserFiles()
{
}

bool KU_UserFiles::reload() {
  if (!loadpwd())
    return false;

  if (!loadsdw())
    return false;

  return true;
}

// Load passwd file

bool KU_UserFiles::loadpwd()
{
  passwd *p;
  struct stat st;
  QString filename;
  QString passwd_filename;
  QString nispasswd_filename;
  KU_User user;
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
      return false;
    }

    while ((p = fgetpwent(fpwd)) != NULL) {
#else
    setpwent(); //This should be enough for BSDs
    while ((p = getpwent()) != NULL) {
#endif
      user = KU_User();
      user.setCaps( KU_User::Cap_POSIX );
      user.setUID(p->pw_uid);
      user.setGID(p->pw_gid);
      user.setName(QString::fromLocal8Bit(p->pw_name));
      tmp  = QString::fromLocal8Bit( p->pw_passwd );
      if ( tmp != "x" && tmp != "*" && !tmp.startsWith("!") )
        user.setDisabled( false );
      else
        user.setDisabled( true );
      if ( tmp.startsWith("!") ) tmp.remove(0, 1);
      user.setPwd( tmp );
      user.setHomeDir(QString::fromLocal8Bit(p->pw_dir));
      user.setShell(QString::fromLocal8Bit(p->pw_shell));
#if defined(__FreeBSD__) || defined(__bsdi__)
      user.setClass(QString::fromLatin1(p->pw_class));
      user.setLastChange(p->pw_change);
      user.setExpire(p->pw_expire);
#endif

      if ((p->pw_gecos != 0) && (p->pw_gecos[0] != 0))
        fillGecos(user, p->pw_gecos);
      append(user);
      kDebug() << "added: " << user.getName() << endl;
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
    return (true);
  if( (passwd_errno != 0) && (nispasswd_errno != 0) )
    return (false);
  else
    return(true);
}

// Load shadow passwords

bool KU_UserFiles::loadsdw()
{
#ifdef HAVE_SHADOW
  QString shadow_file,tmp;
  KU_User user;
  int index;
  struct spwd *spw;
  struct stat st;

  shadow_file = mCfg->shadowsrc();
  if ( shadow_file.isEmpty() )
    return true;

  stat( QFile::encodeName(shadow_file), &st);
  sdw_mode = st.st_mode & 0666;
  sdw_uid = st.st_uid;
  sdw_gid = st.st_gid;

#ifdef HAVE_FGETSPENT
  FILE *f;
  kDebug() << "open shadow file: " << shadow_file << endl;
  if ((f = fopen( QFile::encodeName(shadow_file), "r")) == NULL) {
    KMessageBox::error( 0, i18n("Error opening %1 for reading.").arg(shadow_file) );
    caps &= ~Cap_Shadow;
    return true;
  }
  while ((spw = fgetspent( f ))) {     // read a shadow password structure
#else
  setspent();
  while ((spw = getspent())) {     // read a shadow password structure
#endif

    kDebug() << "shadow entry: " << spw->sp_namp << endl;
    if ((index = lookup(QString::fromLocal8Bit(spw->sp_namp))) == -1) {
      KMessageBox::error( 0, i18n("No /etc/passwd entry for %1.\nEntry will be removed at the next `Save'-operation.").arg(QString::fromLocal8Bit(spw->sp_namp)) );
      continue;
    }
    user = at(index);
    
    tmp = QString::fromLocal8Bit( spw->sp_pwdp );
    if ( tmp.startsWith("!!") || tmp == "*" ) {
      user.setDisabled( true );
      tmp.remove( 0, 2 );
    } else
      user.setDisabled( false );

    user.setSPwd( tmp );        // cp the encrypted pwd
    user.setLastChange( daysToTime( spw->sp_lstchg ) );
    user.setMin(spw->sp_min);
    user.setMax(spw->sp_max);
#ifndef _SCO_DS
    user.setWarn(spw->sp_warn);
    user.setInactive(spw->sp_inact);
    user.setExpire( daysToTime( spw->sp_expire ) );
    user.setFlag(spw->sp_flag);
#endif
    replace( index, user );
  }

#ifdef HAVE_FGETSPENT
  fclose(f);
#else
  endspent();
#endif

#endif // HAVE_SHADOW

return true;
}

// Save password file

#define escstr(a) tmp2 = user.get##a(); \
                    tmp2.replace(':',"_"); \
                    tmp2.replace(',',"_"); \
                    user.set##a( tmp2 );


bool KU_UserFiles::savepwd()
{
  FILE *passwd_fd = NULL;
  FILE *nispasswd_fd = NULL;
  FILE *shadow_fd = NULL;
  uid_t minuid = 0;
  int nis_users_written = 0;
  uid_t tmp_uid = 0;
  QString s;
  QString s1;
  QString tmp, tmp2;
  QString passwd_filename;
  QString nispasswd_filename;
  QString shadow_filename;
  bool write_shadow = false;
#ifdef HAVE_SHADOW
  struct spwd spwd;
  shadow_filename = mCfg->shadowsrc();
  if ( !shadow_filename.isEmpty() && (caps & Cap_Shadow) ) write_shadow = true;
#endif

  char errors_found = '\0';
    #define NOMINUID    0x01
    #define NONISPASSWD 0x02

  // Read KU_User configuration info

  passwd_filename = mCfg->passwdsrc();
  nispasswd_filename = mCfg->nispasswdsrc();
  QString new_passwd_filename =
    passwd_filename + QString::fromLatin1(KU_CREATE_EXT);
  QString new_nispasswd_filename =
    nispasswd_filename + QString::fromLatin1(KU_CREATE_EXT);
  QString new_shadow_filename = 
    shadow_filename + QString::fromLatin1(KU_CREATE_EXT);

  if( nispasswd_filename != passwd_filename ) {
    minuid = mCfg->nisminuid();
  }

  // Backup file(s)

  if ( write_shadow ) {
    if (!s_backuped) {
      if (!backup(shadow_filename)) return false;
      s_backuped = true;
    }
  }

  if(!passwd_filename.isEmpty()) {
    if (!pw_backuped) {
      if (!backup(passwd_filename)) return false;
      pw_backuped = true;
    }
  }
  if(!nispasswd_filename.isEmpty() &&
    (nispasswd_filename != passwd_filename)) {
    if (!pn_backuped) {
      if (!backup(nispasswd_filename)) return false;
      pn_backuped = true;
    }
  }

  // Open file(s)
  if ((shadow_fd = fopen(QFile::encodeName(new_shadow_filename), "w")) == NULL) {
    KMessageBox::error( 0, i18n("Error opening %1 for writing.").arg(new_shadow_filename) );
    return false;
  }

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

  KU_User user;
  int usersindex = 0, addindex = 0;
#ifdef HAVE_SHADOW
  spwd.sp_namp = (char *)malloc(200);
  spwd.sp_pwdp = (char *)malloc(200);
#endif

  while (true) {
    if ( usersindex == count() ) {
      if ( addindex == mAdd.count() ) break;
      user = mAdd.at( addindex );
      addindex++;
    } else {
      if ( mDel.contains( usersindex ) ) {
      	usersindex++;
        continue;
      }
      if ( mMod.contains( usersindex ) ) {
        user = mMod.value( usersindex );
      } else {
        user = at( usersindex );
      }
      usersindex++;
    }

    tmp_uid = user.getUID();
    if ( caps & Cap_Shadow )
      tmp = "x";
    else {
      tmp = user.getPwd();
      if ( user.getDisabled() && tmp != "x" && tmp != "*" )
        tmp = "!" + tmp;
    }

    escstr( Name );
    escstr( HomeDir );
    escstr( Shell );
    escstr( Name );
    escstr( FullName );
#if defined(__FreeBSD__) || defined(__bsdi__)
    escstr( Class );
    escstr( Office );
    escstr( WorkPhone );
    escstr( HomePhone );
    s =
      user.getName() + ":" +
      tmp + ":" +
      QString::number( user.getUID() ) + ":" +
      QString::number( user.getGID() ) + ":" +
      user.getClass() + ":" +
      QString::number( user.getLastChange() ) + ":" +
      QString::number( user.getExpire() ) + ":";

    s1 =
      user.getFullName() + "," +
      user.getOffice() + "," +
      user.getWorkPhone() + "," +
      user.getHomePhone();
#else
    escstr( Office1 );
    escstr( Office2 );
    escstr( Address );
    s =
      user.getName() + ":" +
      tmp + ":" +
      QString::number( user.getUID() ) + ":" +
      QString::number( user.getGID() ) + ":";

    s1 =
      user.getFullName() + "," +
      user.getOffice1() + "," +
      user.getOffice2() + "," +
      user.getAddress();

#endif
    for (int j=(s1.length()-1); j>=0; j--) {
      if (s1[j] != ',')
        break;
      s1.truncate(j);
    }

    s += s1 + ":" +
      user.getHomeDir() + ":" +
      user.getShell() + "\n";

#ifdef HAVE_SHADOW
    if ( write_shadow ) {
      strncpy( spwd.sp_namp, user.getName().local8Bit(), 200 );
      if ( user.getDisabled() )
        strncpy( spwd.sp_pwdp, QString("!!" + user.getSPwd()).local8Bit(), 200 );
      else
        strncpy( spwd.sp_pwdp, user.getSPwd().local8Bit(), 200 );

      spwd.sp_lstchg = timeToDays( user.getLastChange() );
      spwd.sp_min    = user.getMin();
      spwd.sp_max    = user.getMax();
#ifndef _SCO_DS
      spwd.sp_warn   = user.getWarn();
      spwd.sp_inact  = user.getInactive();
      spwd.sp_expire = timeToDays( user.getExpire() );
      spwd.sp_flag   = user.getFlag();
#endif
      putspent(&spwd, shadow_fd);
    }
#endif
    
    if( (nispasswd_fd != 0) && (minuid != 0) ) {
      if (minuid <= tmp_uid) {
        fputs(s.local8Bit().data(), nispasswd_fd);
        nis_users_written++;
        continue;
      }
    }

    if( (nispasswd_fd != 0) && (minuid == 0) ) {
      errors_found = errors_found | NOMINUID;
    }

    if( (nispasswd_fd == 0) && (minuid != 0) ) {
      errors_found = errors_found | NONISPASSWD;
    }
    kDebug() << s << endl;
    fputs(s.local8Bit().data(), passwd_fd);
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

  if(shadow_fd) {
    fclose(shadow_fd);
    chmod(QFile::encodeName(new_shadow_filename), sdw_mode);
    chown(QFile::encodeName(new_shadow_filename), sdw_uid, sdw_gid);
    rename(QFile::encodeName(new_shadow_filename),
      QFile::encodeName(shadow_filename));
  }

#ifdef HAVE_SHADOW
  ::free(spwd.sp_namp);
  ::free(spwd.sp_pwdp);
#endif

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
    return false;
  }
#else
  if( (nis_users_written > 0) || (nispasswd_filename == passwd_filename) ) {
    if (system(PWMKDB) != 0) {
      KMessageBox::error( 0, i18n("Unable to build password databases.") );
      return false;
    }
  }
#endif

  return true;
}

#undef escstr

// Save shadow passwords file

void KU_UserFiles::createPassword( KU_User &user, const QString &password )
{
  if ( caps & Cap_Shadow ) {
    user.setSPwd( encryptPass( password, mCfg->md5shadow() ) );
    user.setPwd( QString::fromLatin1("x") );
  } else
    user.setPwd( encryptPass( password, false ) );
}

bool KU_UserFiles::dbcommit()
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
  umask( mode );
  if ( !ret ) return false;

  mDelSucc = mDel;
  mAddSucc = mAdd;
  mModSucc = mMod;
  mDel.clear();
  mAdd.clear();
  mMod.clear();
  return true;
}
