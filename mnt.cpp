#include "mnt.h"

MntEnt::MntEnt(const char *afsname, const char *adir,
               const char *atype, const char *aopts,
               const char *aquotafilename){
    fsname.setStr(afsname);
    dir.setStr(adir);
    type.setStr(atype);
    opts.setStr(aopts);
    quotafilename.setStr(aquotafilename);
  }

