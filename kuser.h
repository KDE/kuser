#ifndef _KUSER_H_
#define _KUSER_H_

#include <qstring.h>
#include <qlist.h>

#ifdef _KU_QUOTA
#include "quota.h"
#endif

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

#ifdef _KU_SHADOW
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

  KUser();
  KUser(KUser *copy);
  ~KUser();
};

class KUsers {
public:
  KUsers();
  ~KUsers();
  KUser *user_lookup(const char *name);
  KUser *user_lookup(unsigned int uid);
  unsigned int first_free();
  void save();
  KUser *first();
  KUser *next();
  uint getUsersNumber();
  KUser *getUser(uint num);

  void addUser(KUser *ku);
  void delUser(KUser *au);

protected:
  int p_saved;
  int s_saved;
  QList<KUser> u;
};

#endif // _KUSER_H_

