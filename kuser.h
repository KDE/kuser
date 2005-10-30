/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Maintained by Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#ifndef _KU_USER_H_
#define _KU_USER_H_

#include <sys/types.h>

#include <qstring.h>
#include <qptrlist.h>

#include "globals.h"
#include "sid.h"

namespace KU {

class KUsers;

class KUser {
public:
  enum Cap {
    Cap_POSIX = 1,
    Cap_Samba = 2
  };
  KUser();
  KUser(const KUser *user);
  ~KUser();

  void copy(const KUser *user);
  void setCaps( int data );
  int getCaps();

//General
  const QString &getName() const;
  const QString &getSurname() const;
  const QString &getEmail() const;
  const QString &getPwd() const;
  const QString &getHomeDir() const;
  const QString &getShell() const;
  const QString &getFullName() const;

  uid_t getUID() const;
  uid_t getGID() const;
  bool getDisabled() const;

  void setName(const QString &data);
  void setSurname(const QString &data);
  void setEmail(const QString &data);
  void setPwd(const QString &data);
  void setHomeDir(const QString &data);
  void setShell(const QString &data);
  void setFullName(const QString &data);

  void setUID(uid_t data);
  void setGID(uid_t data);
  void setDisabled(bool data);

//gecos
//--BSD gecos
  const QString &getOffice() const;
  const QString &getWorkPhone() const;
  const QString &getHomePhone() const;
  const QString &getClass() const;
//--BSD end
  const QString &getOffice1() const;
  const QString &getOffice2() const;
  const QString &getAddress() const;

//--BSD
  void setOffice(const QString &data);
  void setWorkPhone(const QString &data);
  void setHomePhone(const QString &data);
  void setClass(const QString &data);
//--BSD end
  void setOffice1(const QString &data);
  void setOffice2(const QString &data);
  void setAddress(const QString &data);

//shadow
  const QString &getSPwd() const;
  time_t getExpire() const;
  time_t getLastChange() const;
  int getMin() const;
  int getMax() const;
  int getWarn() const;
  int getInactive() const;
  int getFlag() const;

  void setSPwd(const QString &data);
  void setLastChange(time_t data);
  void setMin(int data);
  void setMax(int data);
  void setWarn(int data);
  void setInactive(int data);
  void setExpire(time_t data);
  void setFlag(int data);

//samba
  const QString &getLMPwd() const; //  sam_lmpwd,
  const QString &getNTPwd() const; //sam_ntpwd,
  const QString &getLoginScript() const; //sam_loginscript,
  const QString &getProfilePath() const; //  sam_profile,
  const QString &getHomeDrive() const; //sam_homedrive,
  const QString &getHomePath() const; //sam_homepath;
  const QString &getWorkstations() const; //sam_workstations
  const QString &getDomain() const; //sam_domain
  const SID &getSID() const; //sid,
  const SID &getPGSID() const; //pgroup_sid;
  
  void setLMPwd( const QString &data ); //  sam_lmpwd,
  void setNTPwd( const QString &data ); //sam_ntpwd,
  void setLoginScript( const QString &data ); //sam_loginscript,
  void setProfilePath( const QString &data); //  sam_profile,
  void setHomeDrive( const QString &data ); //sam_homedrive,
  void setHomePath( const QString &data ); //sam_homepath;
  void setWorkstations( const QString &data ); //sam_workstations
  void setDomain( const QString &data ); //sam_domain
  void setSID( const SID &data ); //sid,
  void setPGSID( const SID &data ); //pgroup_sid;
  
//Administrative
  bool getCreateHome();
  bool getCreateMailBox();
  bool getCopySkel();
  bool getDeleteHome();
  bool getDeleteMailBox();

  void setCreateHome(bool data);
  void setCreateMailBox(bool data);
  void setCopySkel(bool data);
  void setDeleteHome(bool data);
  void setDeleteMailBox(bool data);

protected:
  friend class KUsers;

  int createHome();
  int tryCreate(const QString &dir);
  int createMailBox();
  int copySkel();

  int removeHome();
  int removeCrontabs();
  int removeMailBox();
  int removeProcesses();

  void copyDir(const QString &srcPath, const QString &dstPath);

  int caps;
  QString
    p_name,                        // parsed pw information
    p_surname,
    p_email,
    p_pwd,
    p_dir,
    p_shell,
    p_fname,                        // parsed comment information
    p_office1,
    p_office2,
    p_address,
//BSD  
    p_office,
    p_ophone,
    p_hphone,
    p_class;
  time_t
    p_change,
    p_expire;
//BSD end
  uid_t p_uid;
  gid_t p_gid;

  QString
    s_pwd,                         // parsed shadow password
    sam_lmpwd,
    sam_ntpwd,
    sam_loginscript,
    sam_profile,
    sam_homedrive,
    sam_homepath,
    sam_workstations,
    sam_domain;
  SID 
    sid,
    pgroup_sid;
  signed int
    s_min,                         // days until pwchange allowed.
    s_max,                         // days before change required
    s_warn,                        // days warning for expiration
    s_inact,                       // days before  account  inactive
    s_flag;                        // reserved for future use
  bool
    isDisabled,                // account disabled?
    isCreateHome,              // create homedir
    isCreateMailBox,           // create mailbox
    isCopySkel,                // copy skeleton
    isDeleteHome,              // delete home dir
    isDeleteMailBox;           // delete mailbox
};

class KUsers {
public:
  enum Cap {
    Cap_ReadOnly = 1,
    Cap_Passwd = 2,
    Cap_Shadow = 4,
    Cap_InetOrg = 8,
    Cap_Samba = 16,
    Cap_Disable_POSIX = 32,
    Cap_BSD = 64
  };
  typedef QPtrListIterator<KUser> DelIt;
  typedef QPtrListIterator<KUser> AddIt;
  typedef QMapIterator<KUser*, KUser> ModIt;
  
  QPtrList<KUser> mDelSucc;
  QPtrList<KUser> mAddSucc;
  QMap<KUser*, KUser> mModSucc;
  
  KUsers(KUserPrefsBase *cfg);
  virtual ~KUsers();
  KUser *lookup(const QString & name);
  KUser *lookup(uid_t uid);
  KUser *lookup_sam( const SID &sid );
  KUser *lookup_sam( const QString &sid );
  KUser *lookup_sam( uint rid );
  
  int getCaps() { return caps; }
  const QString &getDOMSID() const;
  
  KUser *first();
  KUser *next();
  uint count() const;
  KUser *operator[](uint num);

  void add( KUser *user );
  void del( KUser *user );
  void mod( KUser *uold, const KUser &unew );
  void commit();
  void cancelMods();
  
  enum {
    NO_FREE = (uid_t) -1
  };

  /**
  * May be reimplemented in descendant classes. 
  * It should return the first available UID, or KUsers::NO_FREE if no more UID.
  */
  virtual uid_t first_free();
  /**
  * May be reimplemented in descendant classes. 
  * It should return the first available user RID, or 0 if no more RID.
  */
  virtual uint first_free_sam();
  /**
  * Must be reimplemented in various backends. It should encode @param password 
  * into the appropriate fields in @param user.
  */
  virtual void createPassword( KUser *user, const QString &password ) = 0;
  /**
  * Must load the users from the storage backend.
  */
  virtual bool reload() = 0;
  /**
  * Must write changes (in mDel, mAdd and mMod) to the storage backend. It must
  * write successful modifications into mDelSucc, mAddSucc and mModSucc.
  */
  virtual bool dbcommit() = 0;
  
protected:
  QPtrList<KUser> mUsers;
  int caps;
  KUserPrefsBase *mCfg;
  
  QPtrList<KUser> mDel;
  QPtrList<KUser> mAdd;
  QMap<KUser*, KUser> mMod;

  QString domsid;
  
  bool doCreate( KUser *user );
  bool doDelete( KUser *user );
  void parseGecos( const char *gecos, QString &name,
    QString &field1, QString &field2, QString &field3 );
  void fillGecos( KUser *user, const char *gecos );
  
};

} //namespace KU

#endif // _KU_USER_H_
