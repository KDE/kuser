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

#include <qstring.h>

#include <kmsgbox.h>

#include "maindlg.h"
#include "globals.h"
#include "kgroup.h"
#include "misc.h"

#ifdef _KU_SHADOW
#include <shadow.h>
#endif

#ifdef _KU_QUOTA
#include "mnt.h"
#include "quota.h"
#endif

// This is to simplify compilation for Red Hat Linux systems, where
// gid's for regular users' private groups start at 500 <duncan@kde.org>
#ifdef _KU_FIRST_USER
#define _KU_FIRST_GID _KU_FIRST_USER
#else 
#define _KU_FIRST_GID 1001 
#endif

KGroup::KGroup() {
  u.setAutoDelete(TRUE);
  name = "";
  pwd  = "*";
  gid  = 0;
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
  name = data;
}

void KGroup::setpwd(const char *data) {
  pwd = data;
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

  if (!load())
    err->display();
}

bool KGroups::load() {
  group *p;
  KGroup *tmpKG = 0;

#ifdef _KU_NIS
  FILE *fgrp = fopen(GROUP_FILE, "r");
  QString tmp;
  if (fgrp == 0) {
    ksprintf(&tmp, i18n("Error opening %s for reading"), GROUP_FILE);
    err->addMsg(tmp, STOP);
    return FALSE;
  }

  while ((p = fgetgrent(fgrp)) != NULL) {
#else
  setgrent();
  
  while ((p = getgrent()) != NULL) {
#endif
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

#ifdef _KU_NIS
  fclose(fgrp);
#else
  endgrent();
#endif

  return TRUE;
}

bool KGroups::save() {
  FILE *grp;
  QString tmpS;
  QString tmpN;
  QString tmp;

  if (!g_saved) {
    backup(GROUP_FILE);
    g_saved = TRUE;
  }

  if ((grp = fopen(GROUP_FILE,"w")) == NULL) {
    tmp.sprintf(i18n("Error opening %s for writing"), GROUP_FILE);
    err->addMsg(tmp, STOP);
    return (FALSE);
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
#ifdef GRMKDB
  if (system(GRMKDB) != 0) {
    err->addMsg("Unable to build group database", STOP);
    return FALSE;
  }
#endif
  return (TRUE);
}

KGroup *KGroups::lookup(const char *name) {
  for (uint i = 0; i<g.count(); i++)
    if (g.at(i)->getname() == name)
      return (g.at(i));
  return NULL;
}

KGroup *KGroups::lookup(unsigned int gid) {
  for (uint i = 0; i<g.count(); i++)
    if (g.at(i)->getgid() == gid)
      return (g.at(i));
  return NULL;
}

int KGroups::first_free() {
  uint i = 0;
  uint t = _KU_FIRST_GID ;

  for (t= _KU_FIRST_GID ; t<65534; t++) {
    while ((i<g.count()) && (g.at(i)->getgid() != t))
      i++;

    if (i == g.count())
      return (t);
  }

  err->addMsg(i18n("You have more than 65534 groups!?!? You have ran out of gid space!"), STOP);
  return (-1);
}

KGroups::~KGroups() {
  g.clear();
}

uint KGroups::getNumber() {
  return (g.count());
}

KGroup *KGroups::get(uint num) {
  return (g.at(num));
}

KGroup *KGroups::first() {
  return (g.first());
}

KGroup *KGroups::next() {
  return (g.next());
}

void KGroups::add(KGroup *ku) {
  g.append(ku);
}

void KGroups::del(KGroup *au) {
  g.remove(au);
}

uint KGroups::count() {
	return g.count();
}
