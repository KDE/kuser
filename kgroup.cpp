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
#include "kgroup.h"
#include "misc.h"

#ifdef _KU_QUOTA
#include "mnt.h"
#include "quota.h"
#endif

KGroup::KGroup() {
  u.setAutoDelete(TRUE);
  name.setStr("");
  pwd.setStr("*");
  gid     = 0;
}
  
KGroup::KGroup(KGroup *copy) {
  name    = copy->name;
  pwd     = copy->pwd;
  gid     = copy->gid;

  u.setAutoDelete(TRUE);
}

KGroup::~KGroup() {
  u.clear();
}

QString KGroup::getname() {
  return (name);
}

QString KGroup::getpwd() {
  return (pwd);
}

unsigned int KGroup::getgid() {
  return (gid);
}

void KGroup::setname(const char *data) {
  name.setStr(data);
}

void KGroup::setpwd(const char *data) {
  pwd.setStr(data);
}

void KGroup::setgid(unsigned int data) {
  gid = data;
}

QString *KGroup::lookup_user(const char *aname) {
  for (uint i = 0; i<u.count(); i++)
    if (aname == (*u.at(i)))
      return (u.at(i));
  return (NULL);
}

void KGroup::addUser(const char *aname) {
  u.append(new QString(aname));
}

bool KGroup::removeUser(const char *aname) {
  QString *q;

  for (uint i=0;i<u.count();i++)
    if ((*(q = u.at(i))) == aname) {
      u.remove(q);
      return (TRUE);
    }
  return (FALSE);
}

uint KGroup::getUsersNumber() {
  return (u.count());
}

QString KGroup::getUserName(uint i) {
  return (*u.at(i));
}

void KGroup::clearUsers() {
  u.clear();
}

KGroups::KGroups() {
  g_saved = 0;

  g.setAutoDelete(TRUE);

  group *p;
  KGroup *tmpKG = 0;

  setgrent();

  while ((p = getgrent())!=NULL) {
    tmpKG = new KGroup();
    tmpKG->setgid(p->gr_gid);
    tmpKG->setname(p->gr_name);
    tmpKG->setpwd(p->gr_passwd);

    char *u_name;
    int i = 0;
    while ((u_name = p->gr_mem[i])!=0) {
      tmpKG->addUser(u_name);
      i++;
    }

    g.append(tmpKG);
  }

  endgrent();
}

#define GROUP_FILE "/etc/group"
#define GROUP_FILE_MASK S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH

void KGroups::save() {
  FILE *grp;
  QString tmpS;
  QString tmpN;
  char other[200];

  if (!g_saved) {
    backup(GROUP_FILE);
    g_saved = TRUE;
  }

  if ((grp = fopen(GROUP_FILE,"w")) == NULL) {
    sprintf(other, _("Error opening %s for writing"), GROUP_FILE);
    KMsgBox::message(0, _("Error"), other, KMsgBox::STOP);
  }

  for (unsigned int i=0; i<g.count(); i++) {
    tmpN.setNum(g.at(i)->getgid());
    tmpS = g.at(i)->getname()+':'+g.at(i)->getpwd()+':'+tmpN+':';
    for (uint j=0; j<g.at(i)->getUsersNumber(); j++) {
       if (j != 0)
	 tmpS += ',';

       tmpS += g.at(i)->getUserName(j);
    }
    tmpS += '\n';
    fputs(tmpS, grp);
  }
  fclose(grp);

  chmod(GROUP_FILE, GROUP_FILE_MASK);
}

KGroup *KGroups::group_lookup(const char *name) {
  for (uint i = 0; i<g.count(); i++)
    if (name == g.at(i)->getname())
      return (g.at(i));
  return (NULL);
}

KGroup *KGroups::group_lookup(unsigned int gid) {
  for (uint i = 0; i<g.count(); i++)
    if (gid == g.at(i)->getgid())
      return (g.at(i));
  return (NULL);
}

unsigned int KGroups::first_free() {
  unsigned int t = 1001;

  for (uint i=0;i<g.count();i++)
  {
    if (g.at(i)->getgid() == t)
    {
      t++;
      i = 0;
      continue;
    }
  }

  return t;
}

KGroups::~KGroups() {
  g.clear();
}

uint KGroups::getGroupsNumber() {
  return (g.count());
}

KGroup *KGroups::getGroup(uint num) {
  return (g.at(num));
}

KGroup *KGroups::first() {
  return (g.first());
}

KGroup *KGroups::next() {
  return (g.next());
}

void KGroups::addGroup(KGroup *ku) {
  g.append(ku);
}

void KGroups::delGroup(KGroup *au) {
  g.remove(au);
}
