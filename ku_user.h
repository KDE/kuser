/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi GyĂśrgy <gyurco@freemail.hu>
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


#include <QSharedDataPointer>

#include "globals.h"
#include "ku_prefs.h"
#include "sid.h"

class KU_Users;

class KU_User_Private: public QSharedData
{
public:
KU_User_Private();

int Caps;
QString Name;
QString Surname;
QString Email;
QString Pwd;
QString HomeDir;
QString Shell;
QString FullName;
uid_t UID;
uid_t GID;
bool Disabled;

//gecos
//--BSD gecos
QString Office;
QString WorkPhone;
QString HomePhone;
QString Class;
//--BSD end
QString Office1;
QString Office2;
QString Address;

//shadow
QString SPwd;
time_t Expire;
time_t LastChange;
int Min;
int Max;
int Warn;
int Inactive;
int Flag;

//samba
QString LMPwd;
QString NTPwd;
QString LoginScript;
QString ProfilePath;
QString HomeDrive;
QString HomePath;
QString Workstations;
QString Domain;
class SID SID;
class SID PGSID;

//Administrative
bool CreateHome;
bool CreateMailBox;
bool CopySkel;
bool DeleteHome;
bool DeleteMailBox;
};

class KU_User {
public:

  enum Cap {
    Cap_POSIX = 1,
    Cap_Samba = 2
  };
  KU_User();
  KU_User(const KU_User *user);
  ~KU_User();

  void copy(const KU_User *user);
  bool operator ==(const KU_User &other) const;

//General
  KU_PROPERTY(int,Caps);
  KU_PROPERTY(QString,Name);
  KU_PROPERTY(QString,Surname);
  KU_PROPERTY(QString,Email);
  KU_PROPERTY(QString,Pwd);
  KU_PROPERTY(QString,HomeDir);
  KU_PROPERTY(QString,Shell);
  KU_PROPERTY(QString,FullName);
  KU_PROPERTY(uid_t,UID);
  KU_PROPERTY(uid_t,GID);
  KU_PROPERTY(bool,Disabled);

//gecos
//--BSD gecos
  KU_PROPERTY(QString,Office);
  KU_PROPERTY(QString,WorkPhone);
  KU_PROPERTY(QString,HomePhone);
  KU_PROPERTY(QString,Class);
//--BSD end
  KU_PROPERTY(QString,Office1);
  KU_PROPERTY(QString,Office2);
  KU_PROPERTY(QString,Address);

//shadow
  KU_PROPERTY(QString,SPwd);
  KU_PROPERTY(time_t,Expire);
  KU_PROPERTY(time_t,LastChange);
  KU_PROPERTY(int,Min);
  KU_PROPERTY(int,Max);
  KU_PROPERTY(int,Warn);
  KU_PROPERTY(int,Inactive);
  KU_PROPERTY(int,Flag);

//samba
  KU_PROPERTY(QString, LMPwd);
  KU_PROPERTY(QString, NTPwd);
  KU_PROPERTY(QString, LoginScript);
  KU_PROPERTY(QString, ProfilePath);
  KU_PROPERTY(QString, HomeDrive);
  KU_PROPERTY(QString, HomePath);
  KU_PROPERTY(QString, Workstations);
  KU_PROPERTY(QString, Domain);
  KU_PROPERTY(SID, SID);
  KU_PROPERTY(SID, PGSID);

//Administrative
  KU_PROPERTY(bool, CreateHome);
  KU_PROPERTY(bool, CreateMailBox);
  KU_PROPERTY(bool, CopySkel);
  KU_PROPERTY(bool, DeleteHome);
  KU_PROPERTY(bool, DeleteMailBox);

protected:
  friend class KU_Users;

  int createHome();
  int tryCreate(const QString &dir);
  int createMailBox();
  int copySkel();

  int removeHome();
  int removeCrontabs();
  int removeMailBox();
  int removeProcesses();
private:
  QSharedDataPointer<KU_User_Private> d;
};

class KU_Users : public QList<KU_User> {
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
  typedef QList<KU_User> AddList;
  typedef QList<int> DelList;
  typedef QMap<int, KU_User> ModList;

  AddList mAddSucc;
  DelList mDelSucc;
  ModList mModSucc;

  KU_Users(KU_PrefsBase *cfg);
  virtual ~KU_Users();

  int getCaps() const { return caps; }
  const QString &getDOMSID() const;

  int lookup(const QString & name) const;
  int lookup(uid_t uid) const;
  int lookup_sam( const SID &sid ) const;
  int lookup_sam( const QString &sid ) const;
  int lookup_sam( uint rid ) const;

  void add( const KU_User &user );
  void del( int index );
  void mod( int index, const KU_User &newuser );
  void commit();

  void cancelMods();

  enum {
    NO_FREE = (uid_t) -1
  };

  /**
  * May be reimplemented in descendant classes.
  * It should return the first available UID, or KU_Users::NO_FREE if no more UID.
  */
  virtual uid_t first_free() const;
  /**
  * May be reimplemented in descendant classes.
  * It should return the first available user RID, or 0 if no more RID.
  */
  virtual uint first_free_sam() const;
  /**
  * Must be reimplemented in various backends. It should encode @param password
  * into the appropriate fields in @param user.
  */
  virtual void createPassword( KU_User &user, const QString &password ) = 0;
  /**
  * Must load the users from the storage backend.
  */
  virtual bool reload() = 0;
  /**
  * Must write changes (in mDel, mAdd and mMod) to the storage backend. It must
  * write successful modifications into mDelSucc, mAddSucc and mModSucc.
  */
  virtual bool dbcommit() = 0;

  const QString &errorString() const { return mErrorString; }
  const QString &errorDetails() const { return mErrorDetails; }
  bool doCreate( KU_User *user );
  bool doDelete( KU_User *user );
protected:
  int caps;
  KU_PrefsBase *mCfg;

  AddList mAdd;
  DelList mDel;
  ModList mMod;

  QString domsid;
  QString mErrorString, mErrorDetails;

  void parseGecos( const char *gecos, QString &name,
    QString &field1, QString &field2, QString &field3 );
  void fillGecos( KU_User &user, const char *gecos );
};

#endif // _KU_USER_H_
