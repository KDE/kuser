#include "includes.h"
#include "config.h"

#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif

#ifdef HAVE_SYS_MNTENT_H
#include <sys/mntent.h>
#define BAD_GETMNTENT
#endif

#ifdef HAVE_SYS_MNTTAB_H
#include <sys/mnttab.h>
#endif

#ifdef HAVE_SYS_FS_UFS_QUOTA_H
#include <sys/fs/ufs_quota.h>
#define CORRECT_FSTYPE(type) (!strcmp(type,MNTTYPE_UFS))
#define QUOTAFILENAME "quotas"
#else
#define CORRECT_FSTYPE(type) (!strcmp(type,MNTTYPE_EXT2))
#define QUOTAFILENAME "quota.user"
#endif

#include "misc.h"
#include "pwdtool.h"
#include "sdwtool.h"
#include "quotatool.h"

KUser *user_lookup(const char *name) {                                               
  for (uint i = 0; i<users.count(); i++)                                       
    if (name == users.at(i)->p_name)                                     
      return (users.at(i));                                                    
  return (NULL);                                                               
}                                                                               

QString readentry(const QString &name) {
  if (config->hasKey(name))
    return config->readEntry(name);
  else
    return (QString)("");
}

int readnumentry(const QString &name) {
  if (config->hasKey(name))
    return config->readNumEntry(name);
  else
    return (0);
}

uint first_free()
{
  uint t = 501;

  for (uint i=0;i<users.count();i++)
  {
    if (users.at(i)->p_uid == t)
    {
      t++;
      i = 0;
      continue;
    }
  }

  return t;
}

void backup(char const *name)
{
  sprintf(tmp, "%s%s", name, XU_BACKUP_EXT);
  unlink(tmp);

  if (rename(name, tmp) == -1)
  {
    sprintf(tmp, _("Can't create backup file for %s"), name);
    QMessageBox::message(tmp, "Ok");
    exit (1);
  }
}

char *convertdate(char *buf, const long int base, const long int days) {
  long int tmpd;
  tm *t;

  if ((base+days)<24855l)
    if (days >0) {
      tmpd = (base+days)*24*60*60;
      t = localtime(&tmpd);

      sprintf(buf, "%d %d %d", t->tm_mday, t->tm_mon+1, t->tm_year+1900);
    }
    else {
      strcpy(buf, "Nothing");
    }
  else {
    strcpy(buf, "Never");
  }

  return buf;
}

QLabel *addLabel(QWidget *parent, const char *name, int x, int y, int w, int h, const char *text) {

  QLabel *tmpLabel = new QLabel(parent, name);
  tmpLabel->setGeometry(x, y, w, h);
  tmpLabel->setAutoResize(TRUE);
  tmpLabel->setText(text);

  return (tmpLabel);
}

QLineEdit *addLineEdit(QWidget *parent, const char *name, int x, int y, int w, int h, const char *text) {

  QLineEdit *tmpLE = new QLineEdit(parent, name);
  tmpLE->setGeometry(x, y, w, h);
  tmpLE->setText(text);

  return (tmpLE);
}

char *updateString(char *d, const char *t) {
  free(d);
  d = (char *)malloc(strlen(t)+1);  
  strcpy(d,t);
  return d;
}

int getValue(long int &data, const char *text, const char *msg) {
  char *check;
  long int value = strtol(text, &check, 0);
  if (check[0]) {
    QMessageBox::message(_("Error"), msg, "Ok");
    return (-1);
  }
  data = value;

  return (0);
}

int getValue(int &data, const char *text, const char *msg) {
  char *check;
  long int value = strtol(text, &check, 0);
  if (check[0]) {
    QMessageBox::message(_("Error"), msg, "Ok");
    return (-1);
  }
  data = (int)value;

  return (0);
}

int getValue(unsigned int &data, const char *text, const char *msg) {
  char *check;
  long int value = strtol(text, &check, 0);
  if (check[0]) {
    QMessageBox::message(_("Error"), msg, "Ok");
    return (-1);
  }
  data = (unsigned int)value;

  return (0);
}

void getmounts() {
#ifdef _XU_QUOTA

#ifdef _KU_DEBUG
printf("getmounts\n");
#endif

#ifdef BAD_GETMNTENT
  struct mnttab *m = NULL;
#else
  struct mntent *m = NULL;
#endif
  FILE *fp;
  MntEnt *mnt = NULL;
  QString quotafilename;

  if (is_quota == 0)
    return;

  is_quota = 0;

#ifdef BAD_GETMNTENT
  fp = fopen(MNTTAB, "r");
puts("fp opened");
  m = (struct mnttab *)malloc(sizeof(mnttab));

  while ((getmntent(fp, m)) == 0) {
printf("getmntent %p\n", m);
    if (strstr(m->mnt_mntopts, "quota") == NULL)
      continue;

    if (!CORRECT_FSTYPE((const char *)m->mnt_fstype))
      continue;

    quotafilename.sprintf("%s%s%s", m->mnt_mountp,
                          (m->mnt_mountp[strlen(m->mnt_mountp) - 1] == '/') ? "" : "/",
                          QUOTAFILENAME);
#else
  fp = setmntent(MNTTAB, "r");
  while ((m = getmntent(fp)) != (struct mntent *)0) {
    if (strstr(m->mnt_opts, "quota") == NULL)
      continue;

    if (!CORRECT_FSTYPE((const char *)m->mnt_type))
      continue;

    quotafilename.sprintf("%s%s%s", m->mnt_dir,
                          (m->mnt_dir[strlen(m->mnt_dir) - 1] == '/') ? "" : "/",
                          QUOTAFILENAME);
#endif

    QFile *f = new QFile(quotafilename);
    if (f->exists() == FALSE)
      continue;

#ifdef BAD_GETMNTENT
    mnt = new MntEnt(m->mnt_special, m->mnt_mountp, m->mnt_fstype,
                     m->mnt_mntopts, quotafilename);
#else
    mnt = new MntEnt(m->mnt_fsname, m->mnt_dir, m->mnt_type,
                     m->mnt_opts, quotafilename);
#endif
    mounts.append(mnt);
    is_quota = 1;
  }
#ifdef BAD_GETMNTENT
  fclose(fp);
#else
  endmntent(fp);
#endif
#endif
}

void init() {
#ifdef _KU_DEBUG
printf("init\n");
#endif

  getmounts();

  pwd_read();

#ifdef _XU_SHADOW
  if (is_shadow != 0)
    sdw_read();
#endif

#ifdef _XU_QUOTA
  if (is_quota != 0)

#ifdef _KU_DEBUG
puts("quota_read");
#endif

    quota_read();

#ifdef _KU_DEBUG
puts("quota_read done");
#endif
#endif
}
