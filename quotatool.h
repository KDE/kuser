#ifndef _KU_QUOTATOOL_H_
#define _KU_QUOTATOOL_H_

#ifdef _KU_QUOTA

class Quota {
public:
  long int
    fcur,
    fsoft,
    fhard,
    icur,
    isoft,
    ihard;
  Quota();
  Quota(int afcur, int afsoft, int afhard, int aicur, int aisoft, int aihard);
  Quota(const Quota *q);
  Quota& operator= (const Quota& q);
};

extern void quota_read();
extern void quota_write();

#endif // _KU_QUOTA

#endif // _KU_EDQUOTA_H_

