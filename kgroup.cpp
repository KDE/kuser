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

KGroups::KGroups() {
  g_saved = 0;

  g.setAutoDelete(TRUE);

  group *p;
  KGroup *tmpKG = 0;

  setgrent();

  while ((p = getgrent())!=NULL) {
    tmpKG = new KGroup();
    tmpKG->gid = p->gr_gid;
    tmpKG->name.setStr(p->gr_name);
    tmpKG->pwd.setStr(p->gr_passwd);

    char *u_name;
    int i = 0;
    while ((u_name = p->gr_mem[i])!=0) {
      tmpKG->u.append(new QString(u_name));
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
    tmpN.setNum(g.at(i)->gid);
    tmpS = g.at(i)->name+':'+g.at(i)->pwd+':'+tmpN+':';
    for (uint j=0; j<g.at(i)->u.count(); j++) {
       if (j != 0)
	 tmpS += ',';

       tmpS += (*g.at(i)->u.at(j));
    }
    tmpS += '\n';
    fputs(tmpS, grp);
  }
  fclose(grp);

  chmod(GROUP_FILE, GROUP_FILE_MASK);
}

KGroup *KGroups::group_lookup(const char *name) {
  for (uint i = 0; i<g.count(); i++)
    if (name == g.at(i)->name)
      return (g.at(i));
  return (NULL);
}

KGroup *KGroups::group_lookup(unsigned int uid) {
  for (uint i = 0; i<g.count(); i++)
    if (uid == g.at(i)->gid)
      return (g.at(i));
  return (NULL);
}

unsigned int KGroups::first_free() {
  unsigned int t = 1001;

  for (uint i=0;i<g.count();i++)
  {
    if (g.at(i)->gid == t)
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
