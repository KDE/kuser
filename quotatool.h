#ifndef _XU_QUOTATOOL_H_
#define _XU_QUOTATOOL_H_

#ifdef _XU_QUOTA

#include "includes.h"                                                          

class Quota {
public:
  long int
    fcur,
    fsoft,
    fhard,
    icur,
    isoft,
    ihard;
  Quota() {
    fcur = 0; fsoft = 0; fhard = 0;
    icur = 0; isoft = 0; ihard = 0;
  }
  
  Quota(int afcur, int afsoft, int afhard, int aicur, int aisoft, int aihard) {
    fcur = afcur; fsoft = afsoft; fhard = afhard;
    icur = aicur; isoft = aisoft; ihard = aihard;
  }
};

extern void quota_read();
extern void quota_write();

#endif // _XU_QUOTA

#endif // _XU_EDQUOTA_H_
