#include "includes.h"

extern "C" int unlink __P ((__const char *__name));

KUser *user_lookup(const char *name) {                                               
  for (uint i = 0; i<users.count(); i++)                                       
    if (!strcmp(name,users.at(i)->p_name))                                     
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
  struct mntent *m;
  FILE *fp;
  MntEnt *mnt = NULL;

  is_quota = 0;

  fp = setmntent(MNTTAB, "r");
  while ((m = getmntent(fp)) != (struct mntent *)0)
    if (hasmntopt(m, MNTOPT_USRQUOTA) != (char *)0) {
      mnt = new MntEnt(m->mnt_fsname, m->mnt_dir, m->mnt_type,
                       m->mnt_opts, m->mnt_freq, m->mnt_passno);
      mounts.append(mnt);
     is_quota = 1;
    }
  endmntent(fp);
}

void init() {
  getmounts();

  pwd_read();

#ifdef _XU_SHADOW
  if (is_shadow)
    sdw_read();
#endif

#ifdef _XU_QUOTA
  if (is_quota != 0)
puts("quota_read");
    quota_read();
#endif
}
