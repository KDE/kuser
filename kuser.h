#ifndef _KUSER_H_
#define _KUSER_H_

#include <sys/types.h>

#include <qstring.h>
#include <qlist.h>

#include "globals.h"

#ifdef _KU_QUOTA
#include "quota.h"
#endif

#if defined(__FreeBSD__) || defined(__bsdi__)
#undef _KU_SHADOW
#endif

class KUsers;

class KUser {
public:
  KUser();
  KUser(const KUser *user);
  ~KUser();

  void copy(const KUser *user);

  const QString &getName() const;
  const QString &getPwd() const;
  const QString &getHomeDir() const;
  const QString &getShell() const;
  const QString &getFullName() const;

  uid_t getUID() const;
  uid_t getGID() const;

#if defined(__FreeBSD__) || defined(__bsdi__)
  const QString &getOffice() const;
  const QString &getWorkPhone() const;
  const QString &getHomePhone() const;
  const QString &getClass() const;
  time_t getLastChange() const;
  time_t getExpire() const;
#else
  const QString &getOffice1() const;
  const QString &getOffice2() const;
  const QString &getAddress() const;

#endif

#ifdef _KU_SHADOW
  const QString &getSPwd() const;
  long getLastChange() const;
  int getMin() const;
  int getMax() const;
  int getWarn() const;
  int getInactive() const;
  int getExpire() const;
  int getFlag() const;
#endif

  void setName(const QString &data);
  void setPwd(const QString &data);
  void setHomeDir(const QString &data);
  void setShell(const QString &data);
  void setFullName(const QString &data);

  void setUID(uid_t data);
  void setGID(uid_t data);

#if defined(__FreeBSD__) || defined(__bsdi__)
  void setOffice(const QString &data);
  void setWorkPhone(const QString &data);
  void setHomePhone(const QString &data);
  void setClass(const QString &data);
  void setExpire(time_t data);
  void setLastChange(time_t data);
#else
  void setOffice1(const QString &data);
  void setOffice2(const QString &data);
  void setAddress(const QString &data);
#endif

#ifdef _KU_SHADOW
  void setSPwd(const QString &data);
  void setLastChange(long data);
  void setMin(int data);
  void setMax(int data);
  void setWarn(int data);
  void setInactive(int data);
  void setExpire(int data);
  void setFlag(int data);
#endif

  int getCreateHome();
  int getCreateMailBox();
  int getCopySkel();

  void setCreateHome(int data);
  void setCreateMailBox(int data);
  void setCopySkel(int data);

protected:
  friend class KUsers;

  void createHome();
  int createKDE();
  int tryCreate(const QString &dir);
  int createMailBox();
  int copySkel();

  int removeHome();
  int removeCrontabs();
  int removeMailBox();
  int removeProcesses();

  void copyDir(const QString &srcPath, const QString &dstPath);

  QString
    p_name,                        // parsed pw information
    p_pwd,
    p_dir,
    p_shell,
    p_fname,                        // parsed comment information
#if defined(__FreeBSD__) || defined(__bsdi__)
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
  uid_t p_uid;
  gid_t p_gid;

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
  int
    isCreateHome,              // create homedir
    isCreateMailBox,           // create mailbox
    isCopySkel;                // copy skeleton
};

class KUsers {
public:
  KUsers();
  ~KUsers();
  KUser *lookup(const QString & name);
  KUser *lookup(uid_t uid);
  uid_t first_free();

  bool load();

  bool save();

  KUser *first();
  KUser *next();
  uint count() const;
  KUser *operator[](uint num);

  void add(KUser *ku);
  void del(KUser *au);

protected:
  bool p_backuped;
  bool s_backuped;

  mode_t pwd_mode;
  mode_t sdw_mode;

  uid_t pwd_uid;
  gid_t pwd_gid;

  uid_t sdw_uid;
  gid_t sdw_gid;

  QList<KUser> u;
  QList<KUser> du;

  bool doCreate();
  bool doDelete();
  void fillGecos(KUser *user, const char *gecos);
  bool loadpwd();
  bool loadsdw();

  bool savepwd();
  bool savesdw();
};

#endif // _KUSER_H_

