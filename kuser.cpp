#include "globals.h"

#include <sys/file.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
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

#include <qstring.h>
#include <qdir.h>

#include <kmsgbox.h>
#include <kstring.h>

#include "maindlg.h"
#include "kuser.h"
#include "misc.h"

#ifdef _KU_SHADOW
#include <shadow.h>
#endif

#ifdef _KU_QUOTA
#include "mnt.h"
#include "quota.h"
#endif


// class KUser

KUser::KUser() {
  p_name  = "";
  p_pwd   = "*";
  p_dir   = "";
  p_shell = "";
  p_fname = "";
#ifdef __FreeBSD__
  p_office = "";
  p_ophone = "";
  p_hphone = "";
  p_class  = "";
  p_change = 0;
  p_expire = 0;
#else
  p_office1 = "";
  p_office2 = "";
  p_address = "";
#endif
  p_uid     = 0;
  p_gid     = 100;

#ifdef _KU_SHADOW   
  s_pwd     = "";
  s_lstchg  = 0;
  s_min     = 0;
  s_max     = -1;
  s_warn    = 7;
  s_inact   = -1;
  s_expire  = 99999;
  s_flag    = 0;
#endif

  isCreateHome = 0;
  isCreateMailBox = 0;
  isCopySkel = 0;
}
  
KUser::~KUser() {
}

int KUser::getCreateHome() {
  return isCreateHome;
}

int KUser::getCreateMailBox() {
  return isCreateMailBox;
}

int KUser::getCopySkel() {
  return isCopySkel;
}

QString KUser::getp_name() {
  return (p_name);
}

QString KUser::getp_pwd() {
  return (p_pwd);
}

QString KUser::getp_dir() {
  return (p_dir);
}

QString KUser::getp_shell() {
  return (p_shell);
}

QString KUser::getp_fname() {
  return (p_fname);
}

#ifdef __FreeBSD__
// FreeBSD apparently uses the GECOS fields differently than other Unices.
// Create some better named functions to make the FreeBSD code clear
QString KUser::getp_office() {
  return (p_office);
}

QString KUser::getp_ophone() {
  return (p_ophone);
}

QString KUser::getp_hphone() {
  return (p_hphone);
}

// New fields needed for the FreeBSD /etc/master.passwd file
QString KUser::getp_class() {
  return (p_class);
}

time_t KUser::getp_change() {
  return (p_change);
}

time_t KUser::getp_expire() {
  return (p_expire);
}

#else

QString KUser::getp_office1() {
  return (p_office1);
}

QString KUser::getp_office2() {
  return (p_office2);
}

QString KUser::getp_address() {
  return (p_address);
}

#endif

unsigned int KUser::getp_uid() {
  return (p_uid);
}

unsigned int KUser::getp_gid() {
  return (p_gid);
}

#ifdef _KU_SHADOW
QString KUser::gets_pwd() {
  return (s_pwd);
}

long KUser::gets_lstchg() {
  return(s_lstchg);
}

int KUser::gets_min() {
  return (s_min);
}

int KUser::gets_max() {
  return (s_max);
}

int KUser::gets_warn() {
  return (s_warn);
}

int KUser::gets_inact() {
  return (s_inact);
}

int KUser::gets_expire() {
  return (s_expire);
}

int KUser::gets_flag() {
  return (s_flag);
}
#endif

void KUser::setp_name(const char *data) {
  p_name = data;
}

void KUser::setp_pwd(const char *data) {
  p_pwd = data;
}

void KUser::setp_dir(const char *data) {
  p_dir = data;
}

void KUser::setp_shell(const char *data) {
  p_shell = data;
}

void KUser::setp_fname(const char *data) {
  p_fname = data;
}

#ifdef __FreeBSD__
// FreeBSD apparently uses the GECOS fields differently than other Unices.
// Create some better named functions to make the FreeBSD code clear
void KUser::setp_office(const char *data) {
  p_office = data;
}

void KUser::setp_ophone(const char *data) {
  p_ophone = data;
}

void KUser::setp_hphone(const char *data) {
  p_hphone = data;
}

// New fields needed for the FreeBSD /etc/master.passwd file
void KUser::setp_class(const char *data) {
  p_class = data;
}

void KUser::setp_change(time_t data) {
  p_change = data;
}

void KUser::setp_expire(time_t data) {
  p_expire = data;
}

#else

void KUser::setp_office1(const char *data) {
  p_office1 = data;
}

void KUser::setp_office2(const char *data) {
  p_office2 = data;
}

void KUser::setp_address(const char *data) {
  p_address = data;
}

#endif

void KUser::setp_uid(unsigned int data) {
  p_uid = data;
}

void KUser::setp_gid(unsigned int data) {
  p_gid = data;
}

#ifdef _KU_SHADOW
void KUser::sets_pwd(const char *data) {
  s_pwd = data;
}

void KUser::sets_lstchg(long data) {
  s_lstchg = data;
}

void KUser::sets_min(int data) {
  s_min = data;
}

void KUser::sets_max(int data) {
  s_max = data;
}

void KUser::sets_warn(int data) {
  s_warn = data;
}

void KUser::sets_inact(int data) {
  s_inact = data;
}

void KUser::sets_expire(int data) {
  s_expire = data;
}

void KUser::sets_flag(int data) {
  s_flag = data;
}

#endif

void KUser::setCreateHome(int data) {
  isCreateHome = data;
}

void KUser::setCreateMailBox(int data) {
  isCreateMailBox = data;
}

void KUser::setCopySkel(int data) {
  isCopySkel = data;
}

KUsers::KUsers() {
  p_backuped = 0;
  s_backuped = 0;

  u.setAutoDelete(TRUE);

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
      case 0: user->setp_fname(val); break;
#ifdef __FreeBSD__
      case 1: user->setp_office(val); break;
      case 2: user->setp_ophone(val); break;
      case 3: user->setp_hphone(val); break;
#else
      case 1: user->setp_office1(val); break;
      case 2: user->setp_office2(val); break;
      case 3: user->setp_address(val); break;
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

  // Start reading passwd file

#ifdef _KU_NIS
  // We are reading our PASSWORD_FILE
  QString tmp;
  FILE *fpwd = fopen(PASSWORD_FILE,"r");
  if(fpwd == NULL) {
     ksprintf(&tmp, i18n("Error opening %s for reading"), PASSWORD_FILE);
     err->addMsg(tmp, STOP);
     return FALSE;
  }

  while ((p = fgetpwent(fpwd)) != NULL) {
#else
   setpwent();

   while ((p = getpwent()) != NULL) {
#endif
#ifdef _KU_QUOTA
    quotas->addQuota(p->pw_uid);
#endif
    tmpKU = new KUser();
    tmpKU->setp_uid(p->pw_uid);
    tmpKU->setp_gid(p->pw_gid);
    tmpKU->setp_name(p->pw_name);
    tmpKU->setp_pwd(p->pw_passwd);
    tmpKU->setp_dir(p->pw_dir);
    tmpKU->setp_shell(p->pw_shell);
#ifdef __FreeBSD__
    tmpKU->setp_class(p->pw_class);
    tmpKU->setp_change(p->pw_change);
    tmpKU->setp_expire(p->pw_expire);
#endif

    if ((p->pw_gecos != 0) && (p->pw_gecos[0] != 0))
      fillGecos(tmpKU, p->pw_gecos);

    u.append(tmpKU);
  }

  // End reading passwd file

#ifdef _KU_NIS
  fclose(fpwd);
#else
  endpwent();
#endif

  return (TRUE);
}

// Load shadow passwords

bool KUsers::loadsdw() {
#ifdef _KU_SHADOW
  QString tmp;
  struct spwd *spw;
  KUser *up = NULL;
  FILE *f;

  if (!is_shadow)
    return FALSE;

  if ((f = fopen(SHADOW_FILE, "r")) == NULL) {
    is_shadow = 0;
    printf("Shadow file missing detected\n");
    return TRUE;
  }

  fclose(f);

  setspent();

  while ((spw = getspent())) {     // read a shadow password structure
    if ((up = lookup(spw->sp_namp)) == NULL) {
      ksprintf(&tmp, i18n("No /etc/passwd entry for %s.\nEntry will be removed at the next `Save'-operation."),
                  spw->sp_namp);
      KMsgBox::message(0, i18n("Error"), tmp, KMsgBox::STOP);
      continue;
    }

    up->sets_pwd(spw->sp_pwdp);        // cp the encrypted pwd
    up->sets_lstchg(spw->sp_lstchg);
    up->sets_min(spw->sp_min);
    up->sets_max(spw->sp_max);
    up->sets_warn(spw->sp_warn);
    up->sets_inact(spw->sp_inact);
    up->sets_expire(spw->sp_expire);
    up->sets_flag(spw->sp_flag);
  }

  endspent();

  return (TRUE);
#endif // _KU_SHADOW
  return (FALSE);
}

bool KUsers::save() {
  if (!savepwd())
    return (FALSE);
  if (!savesdw())
    return (FALSE);

  return (TRUE);
}

// Save password file

bool KUsers::savepwd() {
  FILE *passwd;
  QString s;
  QString s1;

  QString tmp;

  if (!p_backuped) {
    backup(PASSWORD_FILE);
    p_backuped = TRUE;
  }

  if ((passwd = fopen(PASSWORD_FILE,"w")) == NULL) {
    ksprintf(&tmp, i18n("Error opening %s for writing"), PASSWORD_FILE);
    err->addMsg(tmp, STOP);
    return (FALSE);
  }

  for (unsigned int i=0; i<u.count(); i++) {
    KUser *user = u.at(i);

#ifdef __FreeBSD__
    ksprintf(&s, "%s:%s:%i:%i:%s:%i:%i:", (const char *)user->getp_name(),
             (const char *)user->getp_pwd(), user->getp_uid(),
             user->getp_gid(), (const char *)user->getp_class(),
             user->getp_change(), user->getp_expire());

    ksprintf(&s1, "%s,%s,%s,%s", (const char *)user->getp_fname(),
              (const char *)user->getp_office(),
              (const char *)user->getp_ophone(),
              (const char *)user->getp_hphone());
#else

    ksprintf(&s, "%s:%s:%i:%i:",  (const char *)user->getp_name(),
		 (const char *)user->getp_pwd(), (const char *)user->getp_uid(),
		 (const char *)user->getp_gid());

    ksprintf(&s1, "%s,%s,%s,%s", (const char *)user->getp_fname(), (const char *)user->getp_office1()
	       ,(const char *)user->getp_office2(), (const char *)user->getp_address());

#endif

    for (int j=(s1.length()-1); j>=0; j--) {
      if (s1[j] != ',')
        break;

      s1.truncate(j);
    }

    s+=s1+":"+user->getp_dir()+":"+user->getp_shell()+"\n";
    fputs((const char *)s, passwd);

    if(user->getCreateMailBox()) {
      user->createMailBox();
      user->setCreateMailBox(0);
    }
    if(user->getCreateHome()) {
       user->createHome();
       user->setCreateHome(0);
    }
    if(user->getCopySkel()) {
       user->copySkel();
       user->setCopySkel(0);
    }
  }
  fclose(passwd);

  chmod(PASSWORD_FILE, PASSWORD_FILE_MASK);
#ifdef PWMKDB
  // need to run a utility program to build /etc/passwd, /etc/pwd.db
  // and /etc/spwd.db from /etc/master.passwd
  if (system(PWMKDB) != 0) {
     ksprintf(&tmp, i18n("Unable to build password database"));
     err->addMsg(tmp, STOP);
     return (FALSE);
  }
#endif
  return (TRUE);
}

// Save shadow passwords file

bool KUsers::savesdw() {
#ifdef _KU_SHADOW
  QString tmp;
  FILE *f;
  struct spwd *spwp;
  struct spwd s;
  KUser *up;

  if (!is_shadow)
    return FALSE;

  if (!s_backuped) {
    backup(SHADOW_FILE);
    s_backuped = TRUE;
  }

  if ((f = fopen(SHADOW_FILE, "w")) == NULL) {
    ksprintf(&tmp, i18n("Error opening %s for writing"), SHADOW_FILE);
    err->addMsg(tmp, STOP);
    return (FALSE);
  }

  s.sp_namp = (char *)malloc(200);
  s.sp_pwdp = (char *)malloc(200);
    
  for (uint index = 0; index < u.count(); index++) {
    up = u.at(index);
    if (!(const char *)up->gets_pwd()) {
      ksprintf(&tmp, i18n("No shadow entry for %s."), (const char *)up->getp_name());
      err->addMsg(tmp, STOP);
      continue;
    }

    strncpy(s.sp_namp, (const char *)up->getp_name(), 200);
    strncpy(s.sp_pwdp, (const char *)up->gets_pwd(), 200);
    s.sp_lstchg = up->gets_lstchg();
    s.sp_min    = up->gets_min();
    s.sp_max    = up->gets_max();
    s.sp_warn   = up->gets_warn();
    s.sp_inact  = up->gets_inact();
    s.sp_expire = up->gets_expire();
    s.sp_flag   = up->gets_flag();

    spwp = &s;
    putspent(spwp, f);
  }
  fclose(f);

  chmod(SHADOW_FILE, SHADOW_FILE_MASK);
  free(s.sp_namp);
  free(s.sp_pwdp);
#endif // _KU_SHADOW
  return (TRUE);
}

KUser *KUsers::lookup(const char *name) {
  for (uint i = 0; i<u.count(); i++)
    if (u.at(i)->getp_name() == name)
      return (u.at(i));
  return (NULL);
}

KUser *KUsers::lookup(unsigned int uid) {
  for (uint i = 0; i<u.count(); i++)
    if (u.at(i)->getp_uid() == uid)
      return (u.at(i));
  return (NULL);
}

int KUsers::first_free() {
  uint i = 0;
  uint t = 1001;

  for (t=1001; t<65534; t++) {
    while ((i<u.count()) && (u.at(i)->getp_uid() != t))
      i++;

    if (i == u.count())
      return (t);
    i = 0;
  }

  err->addMsg(i18n("You have more than 65534 users!?!? You have ran out of uid space!"), STOP);
  return (-1);
}

KUsers::~KUsers() {
  u.clear();
}

uint KUsers::getNumber() {
  return (u.count());
}

KUser *KUsers::get(uint num) {
  return (u.at(num));
}

KUser *KUsers::first() {
  return (u.first());
}

KUser *KUsers::next() {
  return (u.next());
}

void KUsers::add(KUser *ku) {
  u.append(ku);
}

void KUsers::del(KUser *au) {
  u.remove(au);
}

void KUser::createHome() {
  QDir d = QDir::root();

  if (d.cd(getp_dir())) {
    QString tmp;
    ksprintf(&tmp, i18n("Directory %s already exists"), (const char *)getp_dir());
    err->addMsg(tmp, STOP);
    err->display();
  }

  if (mkdir((const char *)getp_dir(), 0700) != 0) {
    QString tmp;
    ksprintf(&tmp, i18n("Cannot create home directory\nError: %s"), strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
  }

  if (chown((const char *)getp_dir(), getp_uid(), getp_gid()) != 0) {
    QString tmp;
    ksprintf(&tmp, i18n("Cannot change owner of home directory\nError: %s"), strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
  }

  if (chmod(getp_dir(), 0755) != 0) {
    QString tmp;
    ksprintf(&tmp, i18n("Cannot change permissions on home directory\nError: %s"), strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
  }
}

int KUser::createMailBox() {
  QString mailboxpath;
  int fd;
  ksprintf(&mailboxpath, "%s/%s", MAIL_SPOOL_DIR, (const char *)p_name);
  if((fd = open((const char *)mailboxpath, O_CREAT|O_EXCL|O_WRONLY,
                S_IRUSR|S_IWUSR)) < 0) {
    QString tmp;
    ksprintf(&tmp, "Cannot create %s: %s", (const char *)mailboxpath,
             strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
    return -1;
  }
  if(fchown(fd, getp_uid(), 0) < 0) {
    QString tmp;
    ksprintf(&tmp, "Cannot chown %s: %s", (const char *)mailboxpath,
             strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
    return -1;
  }
  return 0;
}

int KUser::copySkel() {
  QDir s(SKELDIR);
  QDir d(getp_dir());
  QString tmp;
  QString prefix(SKEL_FILE_PREFIX);
  int len = prefix.length();

  s.setFilter(QDir::Files | QDir::Hidden);

  if (!s.exists()) {
    QString tmp;
    ksprintf(&tmp, i18n("Directory %s does not exist"), (const char *)s.dirName());    err->addMsg(tmp, STOP);
    err->display();
    return (-1);
  }

  if (!d.exists()) {
    QString tmp;
    ksprintf(&tmp, i18n("Directory %s does not exist"), (const char *)d.dirName());    err->addMsg(tmp, STOP);
    err->display();
    return (-1);
  }

  for (uint i=0; i<s.count(); i++) {
    QString filename(s[i]);
    if (filename.left(len) == prefix) {
      filename = filename.remove(0, len);
    }
    if (copyFile(s.filePath(s[i]), d.filePath(filename)) == -1) {
      err->display();
      continue;
    }

    if (chown(d.filePath(filename), p_uid, p_gid) != 0) {
      QString tmp;
      ksprintf(&tmp, i18n("Cannot change owner of file %s\nError: %s"), (const char *)d.filePath(s[i]), strerror(errno));
      err->addMsg(tmp, STOP);
      err->display();
    }

    if (chmod(d.filePath(filename), 0644) != 0) {
      QString tmp;
      ksprintf(&tmp, i18n("Cannot change permissions on file %s\nError: %s"), (const char *)d.filePath(s[i]), strerror(errno));
      err->addMsg(tmp, STOP);
      err->display();
    }
  }

  return 0;
}

// Temporarily use rm
// TODO: replace by our own procedure cause calling other programs
//       for things we are know how to do is not a good idea

int KUser::removeHome() {
  struct stat sb;
  QString command;

  if (!stat((const char *)p_dir, &sb))
    if (S_ISDIR(sb.st_mode) && sb.st_uid == p_uid) {
#ifdef MINIX
      ksprintf(&command, "/usr/bin/rm -rf -- %s", (const char *)p_dir);
#else
      ksprintf(&command, "/bin/rm -rf -- %s", (const char *)p_dir);
#endif
    if (system((const char *)command) != 0) {
      QString tmp;
      ksprintf(&tmp, i18n("Cannot remove home directory %s\nError: %s"),
               (const char *)command, strerror(errno));
      err->addMsg(tmp, STOP);
      err->display();
     }
   }

  return 0;
}

// TODO: remove at jobs too.

int KUser::removeCrontabs() {
  QString file;
  QString command;

  ksprintf(&file, "/var/cron/tabs/%s",
           (const char*)p_name);
  if (access((const char *)file, F_OK) == 0) {
  	ksprintf(&command, "crontab -u %s -r",
             (const char*)p_name);
    if (system((const char *)command) != 0) {
      QString tmp;
      ksprintf(&tmp, i18n("Cannot remove crontab %s\nError: %s"),
               (const char *)command, strerror(errno));
      err->addMsg(tmp, STOP);
      err->display();
     }
  }

  return 0;
}

int KUser::removeMailBox() {
  QString file;

  ksprintf(&file, "%s/%s", MAIL_SPOOL_DIR,
           (const char*)p_name);
  if (remove((const char *)file) != 0) {
    QString tmp;
    ksprintf(&tmp, i18n("Cannot remove mailbox %s\nError: %s"),
             (const char *)file, strerror(errno));
    err->addMsg(tmp, STOP);
    err->display();
  }

  return 0;
}

int KUser::removeProcesses() {
  // be paranoid -- kill all processes owned by that user, if not root.
  if (p_uid)
    switch (fork()) {
      case 0:
        setuid(p_uid);
        kill(-1, 9);
        break;
      case -1:
        err->addMsg(i18n("Cannot fork()"), STOP);
        err->display();
        perror("fork");
        break;
    }

  return 0;
}
