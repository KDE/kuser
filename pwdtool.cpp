#include "includes.h"
#include "misc.h"

int p_saved = 0;

void pwd_read() {

  passwd *p;
  KUser *tmpKU = 0;
  uint ofs = 0;
  uint i = 0;
  char uname[200];
  char other[200];

  setpwent();

  while ((p = getpwent())!=NULL) {
    tmpKU = new KUser();

    tmpKU->p_uid = p->pw_uid;
    tmpKU->p_gid = p->pw_gid;
    tmpKU->p_name.setStr(p->pw_name);
    tmpKU->p_pwd.setStr(p->pw_passwd);
    tmpKU->p_dir.setStr(p->pw_dir);
    tmpKU->p_shell.setStr(p->pw_shell);

#ifdef _KU_DEBUG
printf("Reading user: %s\n", (const char *)tmpKU->p_name);
#endif

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

    users.append(tmpKU);
  }

  endpwent();

  return;
}

void pwd_write() {
  FILE *passwd;
  QString tmpS;
  char other[200];

  if (!p_saved) {
    backup(PASSWORD_FILE);
    p_saved = TRUE;
  }

  if ((passwd = fopen(PASSWORD_FILE,"w")) == NULL) {
    sprintf(other, _("Error opening %s for writing"), PASSWORD_FILE);
    QMessageBox::message(_("Error"), other, "Ok");
  }

  for (unsigned int i=0; i<users.count(); i++) {
    tmpS.sprintf("%s:%s:%i:%i:%s,%s,%s,%s:%s:%s\n", (const char *)users.at(i)->p_name,
            (const char *)users.at(i)->p_pwd, (const char *)users.at(i)->p_uid, (const char *)users.at(i)->p_gid, (const char *)users.at(i)->p_fname,
            (const char *)users.at(i)->p_office1, (const char *)users.at(i)->p_office2, (const char *)users.at(i)->p_address,
            (const char *)users.at(i)->p_dir, (const char *)users.at(i)->p_shell);
    fputs(tmpS, passwd);
  }
  fclose(passwd);

  chmod(PASSWORD_FILE, PASSWORD_FILE_MASK);
}

