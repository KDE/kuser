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
  p_gid     = 0;

#ifdef _KU_SHADOW   
  s_pwd.setStr("");
  s_lstchg  = 0;
  s_min     = 0;
  s_max     = 99999;
  s_warn    = 0;
  s_inact   = 99999;
  s_expire  = 99999;
  s_flag    = 0;
#endif
}
  
KUser::KUser(KUser *copy) {
    p_name    = copy->p_name;
    p_pwd     = copy->p_pwd;
    p_dir     = copy->p_dir;
    p_shell   = copy->p_shell;
    p_fname   = copy->p_fname;
    p_office1 = copy->p_office1;
    p_office2 = copy->p_office2;
    p_address = copy->p_address;
    p_uid     = copy->p_uid;
    p_gid     = copy->p_gid;

#ifdef _KU_SHADOW   
  if (is_shadow == 1) {
    s_pwd     = copy->s_pwd;
    s_lstchg  = copy->s_lstchg;
    s_min     = copy->s_min;
    s_max     = copy->s_max;
    s_warn    = copy->s_warn;
    s_inact   = copy->s_inact;
    s_expire  = copy->s_expire;
    s_flag    = copy->s_flag;
  }
#endif
}

KUser::~KUser() {
}

KUsers::KUsers() {
  p_saved = 0;
  s_saved = 0;

  u.setAutoDelete(TRUE);

  passwd *p;
  KUser *tmpKU = 0;
  uint ofs = 0;
  uint i = 0;
  char uname[200];
  char other[200];

  setpwent();

  while ((p = getpwent())!=NULL) {
#ifdef _KU_QUOTA
    quotas->addQuota(p->pw_uid);
#endif
    tmpKU = new KUser();
    tmpKU->p_uid = p->pw_uid;
    tmpKU->p_gid = p->pw_gid;
    tmpKU->p_name.setStr(p->pw_name);
    tmpKU->p_pwd.setStr(p->pw_passwd);
    tmpKU->p_dir.setStr(p->pw_dir);
    tmpKU->p_shell.setStr(p->pw_shell);

    if ((!p->pw_gecos) || (!p->pw_gecos[0]))
    {
      tmpKU->p_fname.setStr("");
      tmpKU->p_office1.setStr("");
      tmpKU->p_office2.setStr("");
      tmpKU->p_address.setStr("");
    } else {
      strncpy(other, p->pw_gecos, 200);
      uname[0] = 0;
      for (i = 0; (i<strlen(other))&&(other[i]!=','); i++)
        uname[i] = other[i];
      ofs = ++i;
      uname[i-1] = 0;
      tmpKU->p_fname.setStr(uname);
      uname[0] = 0;
      if (ofs < strlen(other)) {
        for (i = ofs; (i<strlen(other))&&(other[i]!=','); i++)
          uname[i-ofs] = other[i];
        uname[i-ofs] = 0;
        tmpKU->p_address.setStr(uname);
        uname[0] = 0;
        ofs = ++i;
        if ((ofs+1) < strlen(other)) {
          for (i = ofs; (i<strlen(other))&&(other[i]!=','); i++)
            uname[i-ofs] = other[i];
          uname[i-ofs] = 0;
          tmpKU->p_office1.setStr(uname);
          ofs = ++i;
          uname[0] = 0;
          if ((ofs+1) < strlen(other)) {
            for (i = ofs; i<strlen(other); i++)
              uname[i-ofs] = other[i];
            uname[i-ofs] = 0;
            tmpKU->p_office2.setStr(uname);
          }
        }
      }
    }

    u.append(tmpKU);
  }

  endpwent();

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
      return;
    }

    while ((spw = fgetspent(f)))      // read a shadow password structure
    {
      if ((up = user_lookup(spw->sp_namp)) != NULL) {
        up->s_pwd.setStr(spw->sp_pwdp);        // cp the encrypted pwd
        up->s_lstchg   = spw->sp_lstchg;
        up->s_min      = spw->sp_min;
        up->s_max      = spw->sp_max;
        up->s_warn     = spw->sp_warn;
        up->s_inact    = spw->sp_inact;
        up->s_expire   = spw->sp_expire;
        up->s_flag     = spw->sp_flag;
      }
      else {
        tmp.sprintf(_("No /etc/passwd entry for %s.\n\
Entry will be removed at the next `Save'-operation."), spw->sp_namp);
        KMsgBox::message(0, _("Error"), tmp, KMsgBox::STOP);
      }
    }

    fclose(f);
  }
#endif // _KU_SHADOW
}

void KUsers::save() {
  FILE *passwd;
  QString tmpS;
  char other[200];

  if (!p_saved) {
    backup(PASSWORD_FILE);
    p_saved = TRUE;
  }

  if ((passwd = fopen(PASSWORD_FILE,"w")) == NULL) {
    sprintf(other, _("Error opening %s for writing"), PASSWORD_FILE);
    KMsgBox::message(0, _("Error"), other, KMsgBox::STOP);
  }

  for (unsigned int i=0; i<u.count(); i++) {
    tmpS.sprintf("%s:%s:%i:%i:%s,%s,%s,%s:%s:%s\n", (const char *)u.at(i)->p_name,
            (const char *)u.at(i)->p_pwd, (const char *)u.at(i)->p_uid, (const char *)u.at(i)->p_gid, (const char *)u.at(i)->p_fname,
            (const char *)u.at(i)->p_office1, (const char *)u.at(i)->p_office2, (const char *)u.at(i)->p_address,
            (const char *)u.at(i)->p_dir, (const char *)u.at(i)->p_shell);
    fputs(tmpS, passwd);
  }
  fclose(passwd);

  chmod(PASSWORD_FILE, PASSWORD_FILE_MASK);

#ifdef _KU_SHADOW
  QString tmp;
  FILE *f;
  spwd *spwp;
  spwd s;
  KUser *up;


  if (is_shadow) {
    if (!s_saved) {
      backup(SHADOW_FILE);
      s_saved = TRUE;
    }

    if ((f = fopen(SHADOW_FILE, "w")) == NULL) {
      tmp.sprintf(_("Error opening %s for writing"), SHADOW_FILE);
      KMsgBox::message(0, _("Error"), (const char *)tmp, KMsgBox::STOP);
    }

    s.sp_namp = (char *)malloc(200);
    s.sp_pwdp = (char *)malloc(200);
    
    for (uint index = 0; index < u.count(); index++)
    {
      up = u.at(index);
      if (!(const char *)up->s_pwd)
      {
        tmp.sprintf(_("No shadow entry for %s."), (const char *)up->p_name);
        KMsgBox::message(0, _("Error"), tmp, KMsgBox::STOP);
      }
      else {
        strcpy(s.sp_namp, (const char *)up->p_name);
        strcpy(s.sp_pwdp, (const char *)up->s_pwd);
        s.sp_lstchg = up->s_lstchg;
        s.sp_min    = up->s_min;
        s.sp_max    = up->s_max;
        s.sp_warn   = up->s_warn;
        s.sp_inact  = up->s_inact;
        s.sp_expire = up->s_expire;
        s.sp_flag   = up->s_flag;

        spwp = &s;
        putspent(spwp, f);
      }
    }
    fclose(f);

    chmod(SHADOW_FILE, SHADOW_FILE_MASK);
  }
  free(s.sp_namp);
  free(s.sp_pwdp);
#endif // _KU_SHADOW
}

KUser *KUsers::user_lookup(const char *name) {
  for (uint i = 0; i<u.count(); i++)
    if (name == u.at(i)->p_name)
      return (u.at(i));
  return (NULL);
}

KUser *KUsers::user_lookup(unsigned int uid) {
  for (uint i = 0; i<u.count(); i++)
    if (uid == u.at(i)->p_uid)
      return (u.at(i));
  return (NULL);
}

unsigned int KUsers::first_free() {
  unsigned int t = 1001;

  for (uint i=0;i<u.count();i++)
  {
    if (u.at(i)->p_uid == t)
    {
      t++;
      i = 0;
      continue;
    }
  }

  return t;
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
