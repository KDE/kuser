#include "kglobal_.h"

KUserGlobals::KUserGlobals() {
}

void KUserGlobals::init() {
#ifdef _KU_QUOTA
  mounts = new Mounts;
  quotas = new Quotas;
#endif
  
  users = new KUsers;
  groups = new KGroups;
}

KUserGlobals::~KUserGlobals() {
}

KUsers &KUserGlobals::getUsers() {
  return (*users);
}

KGroups &KUserGlobals::getGroups() {
  return (*groups);
}

#ifdef _KU_QUOTA
Mounts &KUserGlobals::getMounts() {
  return (*mounts);
}

Quotas &KUserGlobals::getQuotas() {
  return (*quotas);
}
#endif

