#ifndef _KUSER_H_
#define _KUSER_H_

#include <qstring.h>
#include <qlist.h>

#ifdef _KU_QUOTA
#include "quota.h"
#endif

#ifdef __FreeBSD__
#undef _KU_SHADOW
#endif

class KUser {
public:
  KUser();
  ~KUser();

  QString getp_name();
  QString getp_pwd();
  QString getp_dir();
  QString getp_shell();
  QString getp_fname();
#ifdef __FreeBSD__
  QString getp_office();
  QString getp_ophone();
  QString getp_hphone();
  QString getp_class();
  time_t getp_change();
  time_t getp_expire();
#else
  QString getp_office1();
  QString getp_office2();
  QString getp_address();
#endif
  unsigned int getp_uid();
  unsigned int getp_gid();

#ifdef _KU_SHADOW
  QString gets_pwd();
  long gets_lstchg();
  int gets_min();
  int gets_max();
  int gets_warn();
  int gets_inact();
  int gets_expire();
  int gets_flag();
#endif

  void setp_name(const char *data);
  void setp_pwd(const char *data);
  void setp_dir(const char *data);
  void setp_shell(const char *data);
  void setp_fname(const char *data);
#ifdef __FreeBSD__
  void setp_office(const char *data);
  void setp_ophone(const char *data);
  void setp_hphone(const char *data);
  void setp_class(const char *data);
  void setp_expire(time_t data);
  void setp_change(time_t data);
#else
  void setp_office1(const char *data);
  void setp_office2(const char *data);
  void setp_address(const char *data);
#endif
  void setp_uid(unsigned int data);
  void setp_gid(unsigned int data);

#ifdef _KU_SHADOW
  void sets_pwd(const char *data);
  void sets_lstchg(long data);
  void sets_min(int data);
  void sets_max(int data);
  void sets_warn(int data);
  void sets_inact(int data);
  void sets_expire(int data);
  void sets_flag(int data);
#endif

protected:
  QString
    p_name,                        // parsed pw information
    p_pwd,
    p_dir,
    p_shell,
    p_fname,                        // parsed comment information
#ifdef __FreeBSD__
    p_office,
    p_ophone,
    p_hphone,
    p_class;
  time_t
    p_change,
    p_expire;
#else
    p_office1,
    p_office2,
    p_address;
#endif
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
};

class KUsers {
public:
  KUsers();
  ~KUsers();
  KUser *user_lookup(const char *name);
  KUser *user_lookup(unsigned int uid);
  int first_free();

  bool load();

  bool save();

  KUser *first();
  KUser *next();
  uint getUsersNumber();
  KUser *getUser(uint num);

  void addUser(KUser *ku);
  void delUser(KUser *au);

protected:
  void fillGecos(KUser *user, const char *gecos);
  bool loadpwd();
  bool loadsdw();

  bool savepwd();
  bool savesdw();

  int p_backuped;
  int s_backuped;
  QList<KUser> u;
};

#endif // _KUSER_H_

