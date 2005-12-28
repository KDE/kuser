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

#ifndef _KU_GROUP_H_
#define _KU_GROUP_H_

#include <sys/types.h>

#include <QString>
#include <QStringList>
#include <Q3PtrList>

#include "ku_prefs.h"
#include "sid.h"

class KU_Group {
public:
  KU_Group();
  KU_Group(KU_Group *group);
  ~KU_Group();

  enum Cap {
    Cap_Samba = 1
  };

  void copy(const KU_Group *group);
  void setCaps( int data );
  int getCaps();

  const QString &getName() const;
  const QString &getPwd() const;
  gid_t getGID() const;
  const SID &getSID() const;
  int getType() const;
  const QString &getDisplayName() const;
  const QString &getDesc() const;

  void setName(const QString &data);
  void setPwd(const QString &data);
  void setGID(gid_t data);
  void setSID(const SID &data);
  void setType(int data);
  void setDisplayName(const QString &data);
  void setDesc(const QString &data);

  bool addUser(const QString &name);
  bool removeUser(const QString &name);
  bool lookup_user(const QString &name);
  uint count() const;
  QString user(uint i);
  void clear();

protected:
  QString
    name,
    pwd;
  gid_t gid;

//samba attributes  
  SID sid;
  int type;
  int caps;
  QString displayname;
  QString desc;
  
  QStringList u;
};

class KU_Groups {
public:
  enum Cap {
    Cap_ReadOnly = 1,
    Cap_Passwd = 2,
    Cap_Shadow = 4,
    Cap_Samba = 8
  };

  typedef Q3PtrListIterator<KU_Group> DelIt;
  typedef Q3PtrListIterator<KU_Group> AddIt;
  typedef QMap<KU_Group*, KU_Group>::iterator ModIt;
  
  Q3PtrList<KU_Group> mDelSucc;
  Q3PtrList<KU_Group> mAddSucc;
  QMap<KU_Group*, KU_Group> mModSucc;
  
  KU_Groups( KU_PrefsBase *cfg );
  virtual ~KU_Groups();
  
  int getCaps() const { return caps; }
  const QString &getDOMSID() const;

  KU_Group *lookup( const QString &name );
  KU_Group *lookup( gid_t gid );
  KU_Group *lookup_sam( const SID &sid );
  KU_Group *lookup_sam( const QString &sid );
  KU_Group *lookup_sam( uint rid );
  
  KU_Group *first();
  KU_Group *next();
  KU_Group *operator[](uint num);
  uint count() const;

  void add(KU_Group *group);
  void del(KU_Group *group);
  void mod(KU_Group *gold, const KU_Group &gnew);
  void commit();
  void cancelMods();
  
  enum {
	NO_FREE = (gid_t) -1
  };

  virtual gid_t first_free();
  virtual uint first_free_sam();
  virtual bool reload() = 0;
  virtual bool dbcommit() = 0;

protected:
  KU_PrefsBase *mCfg;
  Q3PtrList<KU_Group> mGroups;
  
  Q3PtrList<KU_Group> mDel;
  Q3PtrList<KU_Group> mAdd;
  QMap<KU_Group*, KU_Group> mMod;
  int caps;
  QString domsid;
};

#endif // _KU_GROUP_H_
