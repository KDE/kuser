#ifndef _XU_MNT_H_
#define _XU_MNT_H_

#include <qstring.h>
#include <qlist.h>

//#include <mntent.h>

class MntEnt {
public:
  QString fsname, dir, type, opts;
  int freq, passno;
  
  MntEnt() {
  }
  
  MntEnt(const char *afsname, const char *adir,
         const char *atype, const char *aopts,
	 int afreq, int apassno);
};

#endif // _XU_MNT_H_
