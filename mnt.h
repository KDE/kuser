#ifndef _KU_MNT_H_
#define _KU_MNT_H_

#include <qstring.h>
#include <qlist.h>

class MntEnt {
public:
  MntEnt() {
  }
  
  MntEnt(const QString &afsname, const QString &adir,
         const QString &atype, const QString &aopts,
         const QString &aquotafilename);
  ~MntEnt();

  const QString &getfsname() const;
  const QString &getdir() const;
  const QString &gettype() const;
  const QString &getopts() const;
  const QString &getquotafilename() const;

  void setfsname(const QString &data);
  void setdir(const QString &data);
  void settype(const QString &data);
  void setopts(const QString &data);
  void setquotafilename(const QString &data);

public:
  QString fsname;
  QString dir;
  QString type;
  QString opts;
  QString quotafilename;
};

class Mounts {
public:
  Mounts();
  ~Mounts();

  MntEnt *operator[](uint num);
  uint getMountsNumber();
protected:
  QList<MntEnt> m;
};

#endif // _KU_MNT_H_
