#ifndef _KGROUP_H_
#define _KGROUP_H_

#include <sys/types.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qlist.h>

#ifdef _KU_QUOTA
#include "quota.h"
#endif

class KGroup {
public:
  KGroup();
  KGroup(KGroup *copy);
  ~KGroup();

  const QString &getName() const;
  const QString &getPwd() const;
  gid_t getGID() const;

  void setName(const QString &data);
  void setPwd(const QString &data);
  void setGID(gid_t data);

  void addUser(const QString &name);
  void removeUser(const QString &name);
  bool lookup_user(const QString &name);
  uint count() const;
  QString user(uint i);
  void clear();

protected:
  QString
    name,
    pwd;
  gid_t gid;

  QStringList u;
};

class KGroups {
public:
  KGroups();
  ~KGroups();

  KGroup *lookup(const QString &name);
  KGroup *lookup(gid_t gid);
  gid_t first_free();

  bool save();
  bool load();

  KGroup *first();
  KGroup *next();
  KGroup *operator[](uint num);

  void add(KGroup *ku);
  void del(KGroup *au);

  uint count() const;

protected:
  int g_saved;
  QList<KGroup> g;

  int mode;
  uid_t uid;
  gid_t gid;
};

#endif // _KGROUP_H_

