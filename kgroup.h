#ifndef _KGROUP_H_
#define _KGROUP_H_

#include <qstring.h>
#include <qlist.h>

#ifdef _KU_QUOTA
#include "quota.h"
#endif

class KGroup {
public:
  QString
    name,
    pwd;
  unsigned int
    gid;

  QString *lookup_user(const char *aname);

  QList<QString> u;

  KGroup();
  KGroup(KGroup *copy);
  ~KGroup();

  void addUser(const char *aname);
  bool removeUser(const char *aname);
};

class KGroups {
public:
  KGroups();
  ~KGroups();
  KGroup *group_lookup(const char *name);
  KGroup *group_lookup(unsigned int uid);
  unsigned int first_free();
  void save();
  KGroup *first();
  KGroup *next();
  uint getGroupsNumber();
  KGroup *getGroup(uint num);

  void addGroup(KGroup *ku);
  void delGroup(KGroup *au);

protected:
  int g_saved;
  QList<KGroup> g;
};

#endif // _KGROUP_H_

