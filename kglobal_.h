#ifndef __KGLOBAL_H__
#define __KGLOBAL_H__

#include "globals.h"

#include "kuser.h"
#include "kgroup.h"
#include "quota.h"
#include "mnt.h"

class KUserGlobals {
public:
  KUserGlobals();
  void init();
  ~KUserGlobals();
  
  KUsers &getUsers();
  KGroups &getGroups();
  
#ifdef _KU_QUOTA
  Quotas &getQuotas();
  Mounts &getMounts();
#endif

private:
#ifdef _KU_QUOTA
  Mounts *mounts;
  Quotas *quotas;
#endif

  KUsers *users;
  KGroups *groups;
};

extern KUserGlobals *kug;

#endif
