#include "mnt.h"

  MntEnt::MntEnt(const char *afsname, const char *adir,
         const char *atype, const char *aopts,
	 int afreq, int apassno) {
    fsname.setStr(afsname);
    dir.setStr(adir);
    type.setStr(atype);
    opts.setStr(aopts);
    freq = afreq;
    passno = apassno;
  }

