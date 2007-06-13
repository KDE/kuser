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

#ifndef _KU_GROUP_H_
#define _KU_GROUP_H_

#include <sys/types.h>


#include <QList>
#include <QSharedDataPointer>

#include "globals.h"
#include "ku_prefs.h"
#include "sid.h"

class KU_Group_Private : public QSharedData
{
public:
KU_Group_Private();

int Caps;

QString Name;
QString Pwd;
gid_t GID;
QStringList users;

  //Samba
class SID SID;
int Type;
QString DisplayName;
QString Desc;
};

class KU_Group {
public:
  KU_Group();
  KU_Group(KU_Group *group);
  ~KU_Group();

  enum Cap {
    Cap_Samba = 1
  };

  bool operator==(const KU_Group &other) const;
  void copy(const KU_Group *group);

  KU_PROPERTY(int, Caps);

  KU_PROPERTY(QString, Name);
  KU_PROPERTY(QString, Pwd);
  KU_PROPERTY(gid_t, GID );

  //Samba
  KU_PROPERTY(SID, SID);
  KU_PROPERTY(int, Type);
  KU_PROPERTY(QString, DisplayName);
  KU_PROPERTY(QString, Desc);

  bool addUser(const QString &name);
  bool removeUser(const QString &name);
  bool lookup_user(const QString &name) const;
  uint count() const;
  QString user(uint i) const;
  void clear();
private:
  QSharedDataPointer<KU_Group_Private> d;
};

class KU_Groups : public QList<KU_Group> {
public:
  enum Cap {
    Cap_ReadOnly = 1,
    Cap_Passwd = 2,
    Cap_Shadow = 4,
    Cap_Samba = 8
  };

  typedef QList<KU_Group> AddList;
  typedef QList<int> DelList;
  typedef QMap<int,KU_Group> ModList;

  AddList mAddSucc;
  DelList mDelSucc;
  ModList mModSucc;

  KU_Groups( KU_PrefsBase *cfg );
  virtual ~KU_Groups();

  int getCaps() const { return caps; }
  const QString &getDOMSID() const;

  int lookup( const QString &name ) const;
  int lookup( gid_t gid ) const;
  int lookup_sam( const SID &sid ) const;
  int lookup_sam( const QString &sid ) const;
  int lookup_sam( uint rid ) const;

  void add(const KU_Group &group);
  void del(int index);
  void mod(int index, const KU_Group &newgroup);
  void commit();
  void cancelMods();

  enum {
        NO_FREE = (gid_t) -1
  };

  virtual gid_t first_free() const;
  virtual uint first_free_sam() const;
  virtual bool reload() = 0;
  virtual bool dbcommit() = 0;

  const QString &errorString() const { return mErrorString; }
  const QString &errorDetails() const { return mErrorDetails; }

protected:
  KU_PrefsBase *mCfg;

  QString mErrorString, mErrorDetails;
  
  AddList mAdd;
  DelList mDel;
  ModList mMod;
  int caps;
  QString domsid;
};

#endif // _KU_GROUP_H_
