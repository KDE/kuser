#ifndef _KUSER_H_
#define _KUSER_H_

#include <qstring.h>
#include <qlist.h>
#include "quotatool.h"

class KUser {
public:
  QString
    p_name,                        // parsed pw information
    p_pwd,
    p_dir,
    p_shell,
    p_fname,                        // parsed comment information
    p_office1,
    p_office2,
    p_address;
  unsigned int
    p_uid,
    p_gid;

#ifdef _XU_SHADOW
  QString
    s_pwd;                         // parsed shadow information
  long
    s_lstchg;                      // last password change
  signed int
    s_min,                         // days until pwchange allowed.
    s_max,                         // days before change required 
    s_warn,                        // days warning for expiration 
    s_inact,                       // days before  account  inactive
    s_expire,                      // date when account expires 
    s_flag;                        // reserved for future use
#endif

#ifdef _XU_QUOTA
  QList<Quota>
    quota;
#endif

  KUser();
  KUser(KUser *copy);
  ~KUser();
};

#endif // _KUSER_H_

