#ifndef _KU_QUOTATOOL_H_
#define _KU_QUOTATOOL_H_

#ifdef _KU_QUOTA

#include <qlist.h>
#include <qintdict.h>

#include "globals.h"

class QuotaMnt {
public:
  QuotaMnt();
  QuotaMnt(int afcur, int afsoft, int afhard, int aicur, int aisoft, int aihard);
  QuotaMnt(int afcur, int afsoft, int afhard, int aicur, int aisoft, int aihard, int aftime, int aitime);
  QuotaMnt(const QuotaMnt *q);
  ~QuotaMnt();

  long getfcur();
  long getfsoft();
  long getfhard();
  long geticur();
  long getisoft();
  long getihard();
  long getftime();
  long getitime();

  void setfcur(long data);
  void setfsoft(long data);
  void setfhard(long data);
  void seticur(long data);
  void setisoft(long data);
  void setihard(long data);
  void setftime(long data);
  void setitime(long data);

public:
  long
    fcur,
    fsoft,
    fhard,
    icur,
    isoft,
    ihard,
    ftime,
    itime;
};

class Quota {
public:
  Quota(unsigned int auid, bool doget = TRUE);
  ~Quota();

  QuotaMnt *getQuotaMnt(uint mntnum);
  uint getMountsNumber();
  unsigned int getUid();
  bool save();
protected:
  unsigned int uid;
  QList<QuotaMnt> q;
};

class Quotas {
public:
  Quotas();
  ~Quotas();

  Quota *getQuota(unsigned int uid);
  void addQuota(unsigned int uid);
  void addQuota(Quota *aq);
  void delQuota(unsigned int uid);
  bool save();
 protected:
  QIntDict<Quota> q;
};

#endif // _KU_QUOTA

#endif // _KU_EDQUOTA_H_
