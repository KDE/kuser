#ifndef _XU_QUOTATOOL_H_
#define _XU_QUOTATOOL_H_

#ifdef _XU_QUOTA

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

#endif // _XU_QUOTA

#endif // _XU_EDQUOTA_H_

