#ifndef _KU_MNT_H_
#define _KU_MNT_H_

#include <qstring.h>
#include <qlist.h>

class MntEnt {
public:
  QString fsname;
  QString dir;
  QString type;
  QString opts;
  QString quotafilename;
  
  MntEnt() {
  }
  
  MntEnt(const char *afsname, const char *adir,
         const char *atype, const char *aopts,
         const char *aquotafilename);
  ~MntEnt();
};

class Mounts {
public:
  Mounts();
  ~Mounts();

  MntEnt *getMount(uint num);
  uint getMountsNumber();
protected:
  QList<MntEnt> m;
};

#endif // _KU_MNT_H_
