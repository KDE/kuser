#include <qstring.h>
#include <kmsgbox.h>

#ifdef _KU_SHADOW
#include <shadow.h>
#endif
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

#include "maindlg.h"
#include "globals.h"
#include "kuser.h"
#include "misc.h"

#ifdef _KU_QUOTA
#include "mnt.h"
#include "quota.h"
#endif

// class KUser

KUser::KUser() {
  p_name.setStr("");
  p_pwd.setStr("*");
  p_dir.setStr("");
  p_shell.setStr("");
  p_fname.setStr("");
  p_office1.setStr("");
  p_office2.setStr("");
  p_address.setStr("");
  p_uid     = 0;
  p_gid     = 100;

#ifdef _KU_SHADOW   
  s_pwd.setStr("");
  s_lstchg  = 0;
  s_min     = 0;
  s_max     = -1;
  s_warn    = 7;
  s_inact   = -1;
  s_expire  = -1;
  s_flag    = 0;
#endif
}
  
KUser::~KUser() {
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

QString KUser::getp_office1() {
  return (p_office1);
}

QString KUser::getp_office2() {
  return (p_office2);
}

QString KUser::getp_address() {
  return (p_address);
}

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
  p_name.setStr(data);
}

void KUser::setp_pwd(const char *data) {
  p_pwd.setStr(data);
}

void KUser::setp_dir(const char *data) {
  p_dir.setStr(data);
}

void KUser::setp_shell(const char *data) {
  p_shell.setStr(data);
}

void KUser::setp_fname(const char *data) {
  p_fname.setStr(data);
}

void KUser::setp_office1(const char *data) {
  p_office1.setStr(data);
}

void KUser::setp_office2(const char *data) {
  p_office2.setStr(data);
}

void KUser::setp_address(const char *data) {
  p_address.setStr(data);
}

void KUser::setp_uid(unsigned int data) {
  p_uid = data;
}

void KUser::setp_gid(unsigned int data) {
  p_gid = data;
}

#ifdef _KU_SHADOW
void KUser::sets_pwd(const char *data) {
  s_pwd.setStr(data);
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

// class KUsers

KUsers::KUsers() {
  p_backuped = 0;
  s_backuped = 0;

  u.setAutoDelete(TRUE);

  load();
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
      case 1: user->setp_office1(val); break;
      case 2: user->setp_office2(val); break;
      case 3: user->setp_address(val); break;
    }
    if(pos == NULL) break;
    s = pos+1;
    no++;
  }
}

bool KUsers::load() {
  if (!loadpwd())
    return (FALSE);

  if (!loadsdw())
    return (FALSE);

  return (TRUE);
}

// Load passwd file

bool KUsers::loadpwd() {
  passwd *p;
  KUser *tmpKU = 0;

  // Start reading passwd file

  setpwent();

  while ((p = getpwent())!=NULL) {
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

    if ((p->pw_gecos != 0) && (p->pw_gecos[0] != 0))
      fillGecos(tmpKU, p->pw_gecos);

    u.append(tmpKU);
  }

  // End reading passwd file

  endpwent();

  return (TRUE);
}

// Load shadow passwords

bool KUsers::loadsdw() {
#ifdef _KU_SHADOW
  QString tmp;
  FILE *f;
  struct spwd *spw;
  KUser *up = NULL;

  if (is_shadow) {
    if ((f = fopen(SHADOW_FILE, "r")) == NULL) {
      is_shadow = 0; 
      tmp.sprintf(_("Error opening %s"), SHADOW_FILE);
      KMsgBox::message(0, _("Error"), tmp, KMsgBox::STOP);
      return (FALSE);
    }

    while ((spw = fgetspent(f))) {     // read a shadow password structure
      if ((up = user_lookup(spw->sp_namp)) == NULL) {
        tmp.sprintf(_("No /etc/passwd entry for %s.\nEntry will be removed at the next `Save'-operation."),
		    spw->sp_namp);
        KMsgBox::message(0, _("Error"), tmp, KMsgBox::STOP);
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

    fclose(f);
  }

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

  char other[200];

  if (!p_backuped) {
    backup(PASSWORD_FILE);
    p_backuped = TRUE;
  }

  if ((passwd = fopen(PASSWORD_FILE,"w")) == NULL) {
    sprintf(other, _("Error opening %s for writing"), PASSWORD_FILE);
    err->addMsg(other, STOP);
    return (FALSE);
  }

  for (unsigned int i=0; i<u.count(); i++) {
    KUser *user = u.at(i);
    s.sprintf("%s:%s:%i:%i:",  (const char *)user->getp_name(),
		 (const char *)user->getp_pwd(), (const char *)user->getp_uid(),
		 (const char *)user->getp_gid());

    s1.sprintf("%s,%s,%s,%s", (const char *)user->getp_fname(), (const char *)user->getp_office1()
	       ,(const char *)user->getp_office2(), (const char *)user->getp_address());

    for (int j=(s1.length()-1); j>=0; j--) {
      if (s1[j] != ',')
	break;

      s1.truncate(j);
    }

    s+=s1+":"+user->getp_dir()+":"+user->getp_shell()+"\n";
    fputs((const char *)s, passwd);
  }
  fclose(passwd);

  chmod(PASSWORD_FILE, PASSWORD_FILE_MASK);
  return (TRUE);
}

// Save shadow passwords file

bool KUsers::savesdw() {
#ifdef _KU_SHADOW
  QString tmp;
  FILE *f;
  spwd *spwp;
  spwd s;
  KUser *up;


  if (is_shadow) {
    if (!s_backuped) {
      backup(SHADOW_FILE);
      s_backuped = TRUE;
    }

    if ((f = fopen(SHADOW_FILE, "w")) == NULL) {
      tmp.sprintf(_("Error opening %s for writing"), SHADOW_FILE);
      err->addMsg((const char *)tmp, STOP);
      return (FALSE);
    }

    s.sp_namp = (char *)malloc(200);
    s.sp_pwdp = (char *)malloc(200);
    
    for (uint index = 0; index < u.count(); index++) {
      up = u.at(index);
      if (!(const char *)up->gets_pwd()) {
        tmp.sprintf(_("No shadow entry for %s."), (const char *)up->getp_name());
        err->addMsg(tmp, STOP);
	continue;
      }

      strcpy(s.sp_namp, (const char *)up->getp_name());
      strcpy(s.sp_pwdp, (const char *)up->gets_pwd());
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
  }
  free(s.sp_namp);
  free(s.sp_pwdp);
#endif // _KU_SHADOW
  return (TRUE);
}

KUser *KUsers::user_lookup(const char *name) {
  for (uint i = 0; i<u.count(); i++)
    if (u.at(i)->getp_name() == name)
      return (u.at(i));
  return (NULL);
}

KUser *KUsers::user_lookup(unsigned int uid) {
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

  err->addMsg(_("You have more than 65534 users!?!? You have ran out of uid space!"), STOP);
  return (-1);
}

KUsers::~KUsers() {
  u.clear();
}

uint KUsers::getUsersNumber() {
  return (u.count());
}

KUser *KUsers::getUser(uint num) {
  return (u.at(num));
}

KUser *KUsers::first() {
  return (u.first());
}

KUser *KUsers::next() {
  return (u.next());
}

void KUsers::addUser(KUser *ku) {
  u.append(ku);
}

void KUsers::delUser(KUser *au) {
  u.remove(au);
}
