#include "globals.h"

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_FCNTL_H
#include <sys/fcntl.h>
#endif
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_SHADOW
#include <shadow.h>
#endif

#include <qstring.h>
#include <qdir.h>

#include "kglobal_.h"
#include "kuser.h"
#include "misc.h"
#include <kstddirs.h>
#include <kmessagebox.h>
#include "editDefaults.h"	

#ifdef _KU_QUOTA
#include "mnt.h"
#include "quota.h"
#endif

// This is to simplify compilation for Red Hat Linux systems, where
// uid's for regular users start at 500 <duncan@kde.org>
#ifdef KU_FIRST_USER
#define _KU_FIRST_UID KU_FIRST_USER
#else 
#define _KU_FIRST_UID 1001 
#endif

// class KUser

KUser::KUser() : p_pwd("*") {
#if defined(__FreeBSD__) || defined(__bsdi__)
  p_change = 0;
  p_expire = 0;
#endif
  p_uid     = 0;
  p_gid     = 100;

#ifdef HAVE_SHADOW   
  s_lstchg  = 0;
  s_min     = 0;
  s_max     = -1;
  s_warn    = 7;
  s_inact   = -1;
  s_expire  = 99999;
  s_flag    = 0;
#endif

  isCreateHome = false;
  isCreateMailBox = false;
  isCopySkel = false;
}
  
KUser::KUser(const KUser *user) {
  copy(user);
}

void KUser::copy(const KUser *user) {
  p_name = user->p_name;
  p_pwd = user->p_pwd;
  p_dir = user->p_dir;
  p_shell = user->p_shell;
  p_fname = user->p_fname;
#if defined(__FreeBSD__) || defined(__bsdi__)
  p_office = user->p_office;
  p_ophone = user->p_ophone;
  p_hphone = user->p_hphone;
  p_class = user->p_class;
  p_change = user->p_change;
  p_expire = user->p_expire;
#else
  p_office1 = user->p_office1;
  p_office2 = user->p_office2;
  p_address = user->p_address;
#endif
  p_uid     = user->p_uid;
  p_gid     = user->p_gid;

#ifdef HAVE_SHADOW   
  s_pwd = user->s_pwd;
  s_lstchg  = user->s_lstchg;
  s_min     = user->s_min;
  s_max     = user->s_max;
  s_warn    = user->s_warn;
  s_inact   = user->s_inact;
  s_expire  = user->s_expire;
  s_flag    = user->s_flag;
#endif

  isCreateHome = user->isCreateHome;
  isCreateMailBox = user->isCreateMailBox;
  isCopySkel = user->isCopySkel;
}
  
KUser::~KUser() {
}

bool KUser::getCreateHome() {
  return isCreateHome;
}

bool KUser::getCreateMailBox() {
  return isCreateMailBox;
}

bool KUser::getCopySkel() {
  return isCopySkel;
}

const QString &KUser::getName() const {
  return p_name;
}

const QString &KUser::getPwd() const {
  return p_pwd;
}

const QString &KUser::getHomeDir() const {
  return p_dir;
}

const QString &KUser::getShell() const {
  return p_shell;
}

const QString &KUser::getFullName() const {
  return p_fname;
}

#if defined(__FreeBSD__) || defined(__bsdi__)
// FreeBSD apparently uses the GECOS fields differently than other Unices.
// Create some better named functions to make the FreeBSD code clear
const QString &KUser::getOffice() const {
  return p_office;
}

const QString &KUser::getWorkPhone() const {
  return p_ophone;
}

const QString &KUser::getHomePhone() const {
  return p_hphone;
}

// New fields needed for the FreeBSD /etc/master.passwd file
const QString &KUser::getClass() const {
  return p_class;
}

time_t KUser::getLastChange() const {
  return p_change;
}

time_t KUser::getExpire() const {
  return p_expire;
}

#else

const QString &KUser::getOffice1() const {
  return p_office1;
}

const QString &KUser::getOffice2() const {
  return p_office2;
}

const QString &KUser::getAddress() const {
  return p_address;
}

#endif

uid_t KUser::getUID() const {
  return p_uid;
}

gid_t KUser::getGID() const {
  return p_gid;
}

#ifdef HAVE_SHADOW
const QString &KUser::getSPwd() const {
  return s_pwd;
}

long KUser::getLastChange() const {
  return s_lstchg;
}

int KUser::getMin() const {
  return s_min;
}

int KUser::getMax() const {
  return s_max;
}

int KUser::getWarn() const {
  return s_warn;
}

int KUser::getInactive() const {
  return s_inact;
}

int KUser::getExpire() const {
  return s_expire;
}

int KUser::getFlag() const {
  return s_flag;
}
#endif

void KUser::setName(const QString &data) {
  p_name = data;
}

void KUser::setPwd(const QString &data) {
  p_pwd = data;
}

void KUser::setHomeDir(const QString &data) {
  p_dir = data;
}

void KUser::setShell(const QString &data) {
  p_shell = data;
}

void KUser::setFullName(const QString &data) {
  p_fname = data;
}

#if defined(__FreeBSD__) || defined(__bsdi__)
// FreeBSD apparently uses the GECOS fields differently than other Unices.
// Create some better named functions to make the FreeBSD code clear
void KUser::setOffice(const QString &data) {
  p_office = data;
}

void KUser::setWorkPhone(const QString &data) {
  p_ophone = data;
}

void KUser::setHomePhone(const QString &data) {
  p_hphone = data;
}

// New fields needed for the FreeBSD /etc/master.passwd file
void KUser::setClass(const QString &data) {
  p_class = data;
}

void KUser::setLastChange(time_t data) {
  p_change = data;
}

void KUser::setExpire(time_t data) {
  p_expire = data;
}

#else

void KUser::setOffice1(const QString &data) {
  p_office1 = data;
}

void KUser::setOffice2(const QString &data) {
  p_office2 = data;
}

void KUser::setAddress(const QString &data) {
  p_address = data;
}

#endif

void KUser::setUID(uid_t data) {
  p_uid = data;
}

void KUser::setGID(gid_t data) {
  p_gid = data;
}

#ifdef HAVE_SHADOW
void KUser::setSPwd(const QString &data) {
  s_pwd = data;
}

void KUser::setLastChange(long data) {
  s_lstchg = data;
}

void KUser::setMin(int data) {
  s_min = data;
}

void KUser::setMax(int data) {
  s_max = data;
}

void KUser::setWarn(int data) {
  s_warn = data;
}

void KUser::setInactive(int data) {
  s_inact = data;
}

void KUser::setExpire(int data) {
  s_expire = data;
}

void KUser::setFlag(int data) {
  s_flag = data;
}

#endif

void KUser::setCreateHome(bool data) {
  isCreateHome = data;
}

void KUser::setCreateMailBox(bool data) {
  isCreateMailBox = data;
}

void KUser::setCopySkel(bool data) {
  isCopySkel = data;
}

KUsers::KUsers() {
  pw_backuped = FALSE;		
  pn_backuped = FALSE;		
  s_backuped = FALSE;

  pwd_mode = 0644;
  pwd_uid = 0;
  pwd_gid = 0;

  sdw_mode = 0600;
  sdw_uid = 0;
  sdw_gid = 0;

  u.setAutoDelete(TRUE);
  du.setAutoDelete(TRUE);

  if (!load())
    err->display();
}

void KUsers::fillGecos(KUser *user, const char *gecos) {
  int no = 0;
  const char *s = gecos;
  const char *pos = NULL;
  // At least one part of the string exists
  for(;;) {
    pos = strchr(s, ',');
    char val[200];
    if(pos == NULL)
      strcpy(val, s);
    else {
      strncpy(val, s, (int)(pos-s));
      val[(int)(pos-s)] = 0;
    }

    switch(no) {
      case 0: user->setFullName(val); break;
#if defined(__FreeBSD__) || defined(__bsdi__)
      case 1: user->setOffice(val); break;
      case 2: user->setWorkPhone(val); break;
      case 3: user->setHomePhone(val); break;
#else
      case 1: user->setOffice1(val); break;
      case 2: user->setOffice2(val); break;
      case 3: user->setAddress(val); break;
#endif
    }
    if(pos == NULL) break;
    s = pos+1;
    no++;
  }
}

bool KUsers::load() {
  if (!loadpwd())
    return FALSE;

  if (!loadsdw())
    return FALSE;

  return TRUE;
}

// Load passwd file

bool KUsers::loadpwd() {
  passwd *p;
  KUser *tmpKU = 0;
  struct stat st;
  QString filename;				
  QString passwd_filename;			
  QString nispasswd_filename;			
  int rc = 0;					
  int passwd_errno = 0;				
  int nispasswd_errno = 0;			
  char processing_file = '\0';			
  #define PASSWD    0x01			
  #define NISPASSWD 0x02			

  // Read KUser configuration	

//  editDefaults eddlg;
  config->setGroup("template");		
  passwd_filename = config->readEntry("passwdsrc");	
  nispasswd_filename = config->readEntry("nispasswdsrc");	

  // Handle unconfigured environments

  if(passwd_filename.isEmpty() && nispasswd_filename.isEmpty()) {
//    eddlg.setPasswdSrc(config->readEntry("passwdsrc", PASSWORD_FILE));
//    eddlg.setGroupSrc(config->readEntry("groupsrc", GROUP_FILE));
//    if (eddlg.exec() != 0) {
      config->writeEntry("passwdsrc", PASSWORD_FILE);
      config->writeEntry("groupsrc", GROUP_FILE);
//    }
    passwd_filename = config->readEntry("passwdsrc");	
    err->addMsg(i18n("KUser Sources were not configured.\nLocal passwd source set to %1\nLocal group source set to %2\n").arg(config->readEntry("passwdsrc")).arg(config->readEntry("groupsrc")));
    err->display();
  }

  if(!passwd_filename.isEmpty()) {			
    processing_file = processing_file | PASSWD;		
    filename.append(passwd_filename);			
  }

  // Start reading passwd file(s)

  for(int i = 0; i < 2; i++) {
    rc = stat(QFile::encodeName(filename), &st);		
    if(rc != 0) {						
      err->addMsg(i18n("stat call on file %1 failed: %2\nCheck KUser Settings (Sources)\n").arg(filename).arg(strerror(errno)));	
      err->display();						
      if( (processing_file & PASSWD) != 0 ) {			
        passwd_errno = errno;					
        if(!nispasswd_filename.isEmpty()) {			
          processing_file = processing_file & ~PASSWD;		
          processing_file = processing_file | NISPASSWD;	
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
      err->addMsg(i18n("Error opening %1 for reading").arg(filename));	
      return FALSE;
    }

    while ((p = fgetpwent(fpwd)) != NULL) {
#else
    while ((p = getpwent()) != NULL) {
#endif
#ifdef _KU_QUOTA
      kug->getQuotas().addQuota(p->pw_uid);
#endif
      tmpKU = new KUser();
      tmpKU->setUID(p->pw_uid);
      tmpKU->setGID(p->pw_gid);
      tmpKU->setName(p->pw_name);
      tmpKU->setPwd(p->pw_passwd);
      tmpKU->setHomeDir(p->pw_dir);
      tmpKU->setShell(p->pw_shell);
#if defined(__FreeBSD__) || defined(__bsdi__)
      tmpKU->setClass(p->pw_class);
      tmpKU->setLastChange(p->pw_change);
      tmpKU->setExpire(p->pw_expire);
#endif

      if ((p->pw_gecos != 0) && (p->pw_gecos[0] != 0)) 
        fillGecos(tmpKU, p->pw_gecos);
      u.append(tmpKU);
    }

    // End reading passwd_filename				

#ifdef HAVE_FGETPWENT
    fclose(fpwd);
#endif
    if(!nispasswd_filename.isEmpty()) {				
      processing_file = processing_file & ~PASSWD;
      processing_file = processing_file | NISPASSWD;		
      filename.truncate(0);				
      filename.append(nispasswd_filename);		
    } 
    else						
      break;						

  }	// end of processing files, for loop			

  if( (passwd_errno == 0) && (nispasswd_errno == 0) )	
    return (TRUE);
  if( (passwd_errno != 0) && (nispasswd_errno != 0) )	
    return (FALSE);
  else
    return(TRUE);						
}

// Load shadow passwords

bool KUsers::loadsdw() {
#ifdef HAVE_SHADOW
  QString tmp;
  struct spwd *spw;
  KUser *up = NULL;
  FILE *f;

  struct stat st;

  if (!is_shadow)
    return TRUE;

  stat(SHADOW_FILE, &st);
  sdw_mode = st.st_mode & 0666;
  sdw_uid = st.st_uid;
  sdw_gid = st.st_gid;

  if ((f = fopen(SHADOW_FILE, "r")) == NULL) {
    is_shadow = 0;
    printf("Shadow file missing detected\n");
    return TRUE;
  }

  fclose(f);

  setspent();

  while ((spw = getspent())) {     // read a shadow password structure
    if ((up = lookup(spw->sp_namp)) == NULL) {
      err->addMsg(i18n("No /etc/passwd entry for %1.\nEntry will be removed at the next `Save'-operation.")
		  .arg(spw->sp_namp));
      err->display();
      continue;
    }

    up->setSPwd(spw->sp_pwdp);        // cp the encrypted pwd
    up->setLastChange(spw->sp_lstchg);
    up->setMin(spw->sp_min);
    up->setMax(spw->sp_max);
#ifndef _SCO_DS
    up->setWarn(spw->sp_warn);
    up->setInactive(spw->sp_inact);
    up->setExpire(spw->sp_expire);
    up->setFlag(spw->sp_flag);
#endif
  }

  endspent();
#endif // HAVE_SHADOW
  return TRUE;
}

bool KUsers::save() {
  if (!savepwd())
    return FALSE;

  if (!savesdw())
    return FALSE;

  if (!doDelete())
    return FALSE;

  if (!doCreate())
    return FALSE;

  return TRUE;
}

bool KUsers::doCreate() {
  for (unsigned int i=0; i<u.count(); i++) {
    KUser *user = u.at(i);

    if(user->getCreateMailBox()) {
      user->createMailBox();
      user->setCreateMailBox(false);
    }

    if(user->getCreateHome()) {
       user->createHome();
       user->setCreateHome(false);
    }

    if(user->getCopySkel()) {
       user->copySkel();
       user->setCopySkel(false);
       user->createKDE();
    }
  }

  return TRUE;
}

bool KUsers::doDelete() {
	uint ucnt = du.count();
  KUser *user;

  user = du.first();
  for (unsigned int i=0; i<ucnt; i++) {
    user = du.current(); 
    user->removeHome();
    user->removeCrontabs();
    user->removeMailBox();
    user->removeProcesses();
    du.remove();
  }

  return TRUE;
}

// Save password file

bool KUsers::savepwd() {
  FILE *passwd_fd = NULL;			
  FILE *nispasswd_fd = NULL;			
  int minuid = 0;			
  int nis_users_written = 0;		
  uid_t tmp_uid = 0;			
  QString s;
  QString s1;
  QString tmp;
  QString passwd_filename;		
  QString nispasswd_filename;
  QString qs_minuid;			

  const char *c_minuid;			
  const char *pw_filename;		
  const char *pn_filename;		
  char errors_found = '\0';		
    #define NOMINUID    0x01		
    #define NONISPASSWD 0x02		

  // Read KUser configuration info	

  config->setGroup("template");		
  passwd_filename = config->readEntry("passwdsrc");		
  nispasswd_filename = config->readEntry("nispasswdsrc");	
  qs_minuid = config->readEntry("nisminuid");			
  if(!qs_minuid.isEmpty()) {					
    c_minuid = qs_minuid.latin1();				
    minuid = atoi(c_minuid);		
  }

  // Backup file(s)			

  if(!passwd_filename.isEmpty()) {	
    if (!pw_backuped) {			
      pw_filename = passwd_filename.latin1();   
      backup(pw_filename);		
      pw_backuped = TRUE;		
    }
  }
  if(!nispasswd_filename.isEmpty()) {	
    if (!pn_backuped) {			
      pn_filename = nispasswd_filename.latin1();   
      backup(pn_filename);		
      pn_backuped = TRUE;		
    }
  }

  // Open file(s)			

  if(!passwd_filename.isEmpty()) {	
    if ((passwd_fd = fopen(QFile::encodeName(passwd_filename),"w")) == NULL) 
      err->addMsg(i18n("Error opening %1 for writing").arg(passwd_filename));
  }					

  if(!nispasswd_filename.isEmpty()) {	
    if ((nispasswd_fd = fopen(QFile::encodeName(nispasswd_filename),"w")) == NULL) 
      err->addMsg(i18n("Error opening %1 for writing").arg(nispasswd_filename));
  }					

    umask(0077);

    for (unsigned int i=0; i<u.count(); i++) {
      KUser *user = u.at(i);
      tmp_uid = user->getUID();	 	

#if defined(__FreeBSD__) || defined(__bsdi__)
      s = QString("%1:%2:%3:%4:%5:%6:%7:")
        .arg(user->getName())
        .arg(user->getPwd())
	.arg(user->getUID())
        .arg(user->getGID())
	.arg(user->getClass())
        .arg(user->getLastChange())
	.arg(user->getExpire());

      s1 = QString("%1,%2,%3,%4")
         .arg(user->getFullName())
         .arg(user->getOffice())
         .arg(user->getWorkPhone())
         .arg(user->getHomePhone());
#else

      s = QString("%1:%2:%3:%4:")
        .arg(user->getName())
        .arg(user->getPwd())
	.arg(user->getUID())
        .arg(user->getGID());

      s1 = QString("%1,%2,%3,%4")
         .arg(user->getFullName())
	 .arg(user->getOffice1())
	 .arg(user->getOffice2())
	 .arg(user->getAddress());

#endif

      for (int j=(s1.length()-1); j>=0; j--) {
        if (s1[j] != ',')
          break;

        s1.truncate(j);
      }

      s += s1+":"+user->getHomeDir()+":"+user->getShell()+"\n";

      if( (nispasswd_fd != 0) && (minuid != 0) ) {
        if (minuid <= tmp_uid) {
      	  fputs(QFile::encodeName(s), nispasswd_fd);	
          nis_users_written++;		
          continue;				
        }
        else{
      	  fputs(QFile::encodeName(s), passwd_fd);	
          continue;				
        }
      }

      if( (nispasswd_fd != 0) && (minuid == 0) ) {
	errors_found = errors_found | NOMINUID;  
      	fputs(QFile::encodeName(s), passwd_fd);	
        continue;
      }

      if( (nispasswd_fd == 0) && (minuid != 0) ) {
	errors_found = errors_found | NONISPASSWD;	
      	fputs(QFile::encodeName(s), passwd_fd);	
        continue;
      }

      fputs(QFile::encodeName(s), passwd_fd);	

    }		/* end for i loop */

    if(passwd_fd) {					
      fclose(passwd_fd);
      chmod(QFile::encodeName(passwd_filename), pwd_mode);	
      chown(QFile::encodeName(passwd_filename), pwd_uid, pwd_gid);	
    }							

    if(nispasswd_fd) {					
      fclose(nispasswd_fd);				
      chmod(QFile::encodeName(nispasswd_filename), pwd_mode);	
      chown(QFile::encodeName(nispasswd_filename), pwd_uid, pwd_gid);	
    }							

    if( (errors_found & NOMINUID) != 0 ) {	
      err->addMsg(i18n("Unable to process NIS passwd file without a minimum UID specified.\nPlease update KUser Settings (Sources)"));	
      err->display();				
    }						

    if( (errors_found & NONISPASSWD) != 0 ) {
      err->addMsg(i18n("Specifying NIS minimum UID requires NIS file(s).\nPlease update KUser Settings (Sources)"));	
      err->display();				
    }						

  // need to run a utility program to build /etc/passwd, /etc/pwd.db
  // and /etc/spwd.db from /etc/master.passwd
#if defined(__FreeBSD__) || defined(__bsdi__)
  if (system(PWMKDB) != 0) {
    err->addMsg(i18n("Unable to build password database"));
    return FALSE;
  }
#else
  if(nis_users_written > 0) {		
    if (system(PWMKDB) != 0) {
      err->addMsg(i18n("Unable to build password databases"));
      return FALSE;
    }
  }					
#endif

  return TRUE;
}

// Save shadow passwords file

bool KUsers::savesdw() {
#ifdef HAVE_SHADOW
  QString tmp;
  FILE *f;
  struct spwd *spwp;
  struct spwd s;
  KUser *up;

  if (!is_shadow)
    return TRUE;

  if (!s_backuped) {
    backup(SHADOW_FILE);
    s_backuped = TRUE;
  }

  umask(0077);

  if ((f = fopen(SHADOW_FILE, "w")) == NULL) {
    err->addMsg(i18n("Error opening %1 for writing").arg(SHADOW_FILE));
    return FALSE;
  }

  s.sp_namp = (char *)malloc(200);
  s.sp_pwdp = (char *)malloc(200);
    
  for (uint index = 0; index < u.count(); index++) {
    up = u.at(index);
    if (up->getSPwd().isNull()) {
      err->addMsg(i18n("No shadow entry for %1.").arg(up->getName()));
      continue;
    }

    strncpy(s.sp_namp, QFile::encodeName(up->getName()), 200);
    strncpy(s.sp_pwdp, QFile::encodeName(up->getSPwd()), 200);
    s.sp_lstchg = up->getLastChange();
    s.sp_min    = up->getMin();
    s.sp_max    = up->getMax();
#ifndef _SCO_DS
    s.sp_warn   = up->getWarn();
    s.sp_inact  = up->getInactive();
    s.sp_expire = up->getExpire();
    s.sp_flag   = up->getFlag();
#endif
    spwp = &s;
    putspent(spwp, f);
  }
  fclose(f);

  chmod(SHADOW_FILE, sdw_mode);
  chown(SHADOW_FILE, sdw_uid, sdw_gid);

  free(s.sp_namp);
  free(s.sp_pwdp);
#endif // HAVE_SHADOW
  return TRUE;
}

KUser *KUsers::lookup(const QString & name) {
  for (uint i = 0; i<u.count(); i++)
    if (u.at(i)->getName() == name)
      return (u.at(i));
  return NULL;
}

KUser *KUsers::lookup(uid_t uid) {
  for (uint i = 0; i<u.count(); i++)
    if (u.at(i)->getUID() == uid)
      return (u.at(i));
  return NULL;
}

uid_t KUsers::first_free() {
  uid_t t = _KU_FIRST_UID ;

  for (t = _KU_FIRST_UID ; t<65534; t++)
    if (lookup(t) == NULL)
      return t;

  err->addMsg(i18n("You have more than 65534 users!?!? You have run out of uid space!"));
  return (-1);
}

KUsers::~KUsers() {
  u.clear();
  du.clear();
}

uint KUsers::count() const {
  return u.count();
}

KUser *KUsers::operator[](uint num) {
  return u.at(num);
}

KUser *KUsers::first() {
  return u.first();
}

KUser *KUsers::next() {
  return u.next();
}

void KUsers::add(KUser *ku) {
  u.append(ku);
}

void KUsers::del(KUser *au) {
  KUser *nu = new KUser(au);
  du.append(nu);
  u.remove(au);
}

void KUser::createHome() {

  if (mkdir(QFile::encodeName(p_dir), 0700) != 0) {
    err->addMsg(i18n("Cannot create home directory %1\nError: %1").arg(p_dir).arg(QString::fromLocal8Bit(strerror(errno))));
    err->display();
    return;
  }

  if (chown(QFile::encodeName(p_dir), p_uid, p_gid) != 0) {
    err->addMsg(i18n("Cannot change owner of home directory\nError: %1").arg(QString::fromLocal8Bit(strerror(errno))));
    err->display();
    return;
  }

  if (chmod(QFile::encodeName(p_dir), KU_HOMEDIR_PERM) != 0) {
    err->addMsg(i18n("Cannot change permissions on home directory\nError: %1").arg(QString::fromLocal8Bit(strerror(errno))));
    err->display();
    return;
  }
}

int KUser::createKDE() {

	const QStringList levels = QStringList() << "/.kde/" << "share/" << "doc/";	
	QStringList types;		
	QString k_dir = p_dir;
	KStandardDirs kstddirs;	

	for (uint level=0; level<levels.count(); level++) {	/* build kde base dirs */
		k_dir.append(levels[level]);	
		if (tryCreate(k_dir))
			return(-1);
 	}

  types = kstddirs.KStandardDirs::allTypes();		/* retrieve kde deflt types */

	for(uint i=0; i<types.count(); i++) {					/* build kde subdirectories */
		k_dir = p_dir;															/* start with $HOME dir			*/
		k_dir.append(levels[0]);										/* add base dir "/.kde/"		*/
		const char *ctype = types[i].latin1();			/* convert to char for call	*/
  	QString tpath = KStandardDirs::kde_default(ctype);		/* get deflt path */
		k_dir.append(tpath);												/* complete full path name	*/
		if (tryCreate(k_dir))												/* able to create it?				*/
			return(-1);																/* -no, exit stage left			*/
	}

  return(0);

}

int KUser::tryCreate(const QString &dir) {
struct stat sb;
int	rc = 0;

rc = stat(QFile::encodeName(dir), &sb);
	if (rc == 0) {                                      /* dir exists */
   	if (S_ISDIR(sb.st_mode)) {												/* is directory?	*/
																					/* yes it is...		*/
			if (KMessageBox::
				warningContinueCancel(0, i18n("Directory %1 already exists!\nWill make %2 owner and change permissions.\nDo you want to continue?")
.arg(dir).arg(p_name), QString::null, i18n("&Continue")) ==
KMessageBox::Continue) {
																											/* user continued */
 				if (chown(QFile::encodeName(dir), p_uid, p_gid) != 0) {
   				err->addMsg(i18n("Cannot change owner of %1 directory\nError: %2")
						.arg(dir).arg(strerror(errno)));
   				err->display();
 				}
		  	if (chmod(QFile::encodeName(dir), KU_KDEDIRS_PERM) != 0) {
   				err->addMsg(i18n("Cannot change permissions on %1 directory\nError: %2").arg(dir).arg(strerror(errno)));
   				err->display();
 				}
				return(0);
			}
			else {																					/* user cancelled	*/
   			err->addMsg(i18n("Directory %1 left 'as is'.\nVerify ownership and permissions for user %2 who may not be able to log in!").arg(dir).arg(p_name));
   			err->display();
				return(-1);
			}
		}
		else {																			/* exists but not as dir */
   		err->addMsg(i18n("%1 exists and is not a directory. User %2 will not be able to log in!").arg(dir).arg(p_name));
   		err->display();
			return(-1);
		}
	}
	else {																					/* stat rc = -1					*/
		if (errno == ENOENT) {										/* good! dir doesn't exist	*/
 			if (mkdir(QFile::encodeName(dir), 0700) != 0) {
   			err->addMsg(i18n("Cannot create %1 directory\nError: %2").arg(dir)
					.arg(strerror(errno)));
   			err->display();
				return(-1);
 		 	}
 			if (chown(QFile::encodeName(dir), p_uid, p_gid) != 0) {
   			err->addMsg(i18n("Cannot change owner of %1 directory\nError: %2")
					.arg(dir).arg(strerror(errno)));
   			err->display();
 			}
 			if (chmod(QFile::encodeName(dir), KU_KDEDIRS_PERM) != 0) {
   			err->addMsg(i18n("Cannot change permissions on %1 directory\nError: %2")					.arg(dir).arg(strerror(errno)));
   			err->display();
 			}
			return(0);
 		}
		else {																		/* some other error on stat   */
   		err->addMsg(i18n("stat call on %1 failed.\nError: %2").arg(dir)
				.arg(strerror(errno)));
   		err->display();
			return(-1);
		}
 	}
}

int KUser::createMailBox() {
  QString mailboxpath;
  int fd;
  mailboxpath = QString("%1/%2").arg(MAIL_SPOOL_DIR).arg(p_name);
  if((fd = open(QFile::encodeName(mailboxpath), O_CREAT|O_EXCL|O_WRONLY,
                S_IRUSR|S_IWUSR)) < 0) {
    err->addMsg(i18n("Cannot create %1: %2")
                .arg(mailboxpath)
                .arg(QString::fromLocal8Bit(strerror(errno))));
    err->display();
    return -1;
  }

  close(fd);

  if (chown(QFile::encodeName(mailboxpath), p_uid, KU_MAILBOX_GID) != 0) {
    err->addMsg(i18n("Cannot change owner on mailbox: %1\nError: %2")
                .arg(mailboxpath).arg(QString::fromLocal8Bit(strerror(errno))));
    err->display();
    return -1;
  }

  if (chmod(QFile::encodeName(mailboxpath), KU_MAILBOX_PERM) != 0) {
    err->addMsg(i18n("Cannot change permissions on mailbox: %1\nError: %2")
                .arg(mailboxpath).arg(QString::fromLocal8Bit(strerror(errno))));
    err->display();
    return -1;
  }

  return 0;
}

void KUser::copyDir(const QString &srcPath, const QString &dstPath) {
  struct stat st;
  QDir s(srcPath);
  QDir d(dstPath);
  QString prefix(SKEL_FILE_PREFIX);
  int len = prefix.length();

  s.setFilter(QDir::Dirs | QDir::Hidden);

  for (uint i=0; i<s.count(); i++) {
    QString name(s[i]);

    if (name == ".")
      continue;
    if (name == "..")
      continue;

    QString filename(s.filePath(name));
    QDir dir(filename);

    if (stat(QFile::encodeName(filename), &st) != 0)
      printf("errno = %d, '%s'\n", errno, strerror(errno));

    if (name.left(len) == prefix)
      name = name.remove(0, len);

    d.mkdir(name, FALSE);

    if (chown(QFile::encodeName(d.filePath(name)), p_uid, p_gid) != 0) {
      err->addMsg(i18n("Cannot change owner of directory %1\nError: %2")
                  .arg(d.filePath(s[i])).arg(QString::fromLocal8Bit(strerror(errno))));
      err->display();
    }

    if (chmod(QFile::encodeName(d.filePath(name)), st.st_mode & 07777) != 0) {
      err->addMsg(i18n("Cannot change permissions on directory %1\nError: %2")
                  .arg(d.filePath(s[i])).arg(QString::fromLocal8Bit(strerror(errno))));
      err->display();
    }

    copyDir(s.filePath(name), d.filePath(name));
  }

  s.setFilter(QDir::Files | QDir::Hidden);

  for (uint i=0; i<s.count(); i++) {
    QString name(s[i]);

    QString filename(s.filePath(name));

    stat(QFile::encodeName(filename), &st);

    if (name.left(len) == prefix)
      name = name.remove(0, len);

    if (copyFile(filename, d.filePath(name)) == -1) {
      continue;
    }

    if (chown(QFile::encodeName(d.filePath(name)), p_uid, p_gid) != 0) {
      err->addMsg(i18n("Cannot change owner of file %1\nError: %2")
                  .arg(d.filePath(s[i])).arg(QString::fromLocal8Bit(strerror(errno))));
      err->display();
    }

    if (chmod(QFile::encodeName(d.filePath(name)), st.st_mode & 07777) != 0) {
      err->addMsg(i18n("Cannot change permissions on file %1\nError: %2")
                  .arg(d.filePath(s[i])).arg(QString::fromLocal8Bit(strerror(errno))));
      err->display();
    }
  }
}

int KUser::copySkel() {
  QDir s(SKELDIR);
  QDir d(p_dir);

  umask(0777);

  if (!s.exists()) {
    err->addMsg(i18n("Directory %1 does not exist").arg(s.dirName()));
    err->display();
    return (-1);
  }

  if (!d.exists()) {
    err->addMsg(i18n("Directory %1 does not exist").arg(d.dirName()));
    err->display();
    return (-1);
  }

  copyDir(s.absPath(), d.absPath());

  return 0;
}

// Temporarily use rm
//TODO: replace by our own procedure cause calling other programs
//      for things we are know how to do is not a good idea

int KUser::removeHome() {
  struct stat sb;
  QString command;

  if (!stat(QFile::encodeName(p_dir), &sb))
    if (S_ISDIR(sb.st_mode) && sb.st_uid == p_uid) {
#ifdef MINIX
      command = QString("/usr/bin/rm -rf -- %1").arg(p_dir);
#else
      command = QString("/bin/rm -rf -- %1").arg(p_dir);
#endif
      if (system(QFile::encodeName(command)) != 0) {
             err->addMsg(i18n("Cannot remove home directory %1\nError: %2")
                       .arg(command).arg(strerror(errno)));
             err->display();
      }
    } else {
      err->addMsg(i18n("Removal of home directory %1 failed (uid = %2, gid = %3)").arg(p_dir).arg(sb.st_uid).arg(sb.st_gid));
      err->display();
    }
  else {
    err->addMsg(i18n("stat call on file %1 failed\nError: %2")
                 .arg(p_dir).arg(strerror(errno)));
    err->display();
  }

  return 0;
}

//TODO: remove at jobs too.

int KUser::removeCrontabs() {
  QString file;
  QString command;

  file = QString("/var/cron/tabs/%1").arg(p_name);
  if (access(QFile::encodeName(file), F_OK) == 0) {
    command = QString("crontab -u %1 -r").arg(p_name);
    if (system(QFile::encodeName(command)) != 0) {
      err->addMsg(i18n("Cannot remove crontab %1\nError: %2")
                  .arg(command).arg(QString::fromLocal8Bit(strerror(errno))));
      err->display();
     }
  }

  return 0;
}

int KUser::removeMailBox() {
  QString file;

  file = QString("%1/%2").arg(MAIL_SPOOL_DIR).arg(p_name);
  if (remove(QFile::encodeName(file)) != 0) {
    err->addMsg(i18n("Cannot remove mailbox %1\nError: %2")
                .arg(file).arg(QString::fromLocal8Bit(strerror(errno))));
    err->display();
  }

  return 0;
}

int KUser::removeProcesses() {
  // be paranoid -- kill all processes owned by that user, if not root.

  if (p_uid != 0)
    switch (fork()) {
      case 0:
        setuid(p_uid);
        kill(-1, 9);
        _exit(0);
        break;
      case -1:
        err->addMsg(i18n("Cannot fork()"));
        err->display();
        perror("fork");
        break;
    }

  return 0;
}

