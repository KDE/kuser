#ifndef _KGROUP_H_
#define _KGROUP_H_

#include <qstring.h>
#include <qlist.h>

#ifdef _KU_QUOTA
#include "quota.h"
#endif

class KGroup {
public:
  KGroup();
  KGroup(KGroup *copy);
  ~KGroup();

  QString getname();
  QString getpwd();
  unsigned int getgid();

  void setname(const char *data);
  void setpwd(const char *data);
  void setgid(unsigned int data);

  void addUser(const char *aname);
  bool removeUser(const char *aname);
  QString *lookup_user(const char *aname);
  uint getUsersNumber();
  QString getUserName(uint i);
  void clearUsers();

protected:
  QString
    name,
    pwd;
  unsigned int
    gid;

  QList<QString> u;
};

class KGroups {
public:
  KGroups();
  ~KGroups();
  KGroup *group_lookup(const char *name);
  KGroup *group_lookup(unsigned int gid);
  int first_free();
  bool save();
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
