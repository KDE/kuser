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

#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>

#include "kuserprefs.h"
#include "sid.h"

namespace KU {

class KGroup {
public:
  KGroup();
  KGroup(KGroup *group);
  ~KGroup();

  enum Cap {
    Cap_Samba = 1
  };

  void copy(const KGroup *group);
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

class KGroups {
public:
  enum Cap {
    Cap_ReadOnly = 1,
    Cap_Passwd = 2,
    Cap_Shadow = 4,
    Cap_Samba = 8
  };

  typedef QPtrListIterator<KGroup> DelIt;
  typedef QPtrListIterator<KGroup> AddIt;
  typedef QMapIterator<KGroup*, KGroup> ModIt;
  
  QPtrList<KGroup> mDelSucc;
  QPtrList<KGroup> mAddSucc;
  QMap<KGroup*, KGroup> mModSucc;
  
  KGroups( KUserPrefsBase *cfg );
  virtual ~KGroups();
  
  int getCaps() const { return caps; }
  const QString &getDOMSID() const;

  KGroup *lookup( const QString &name );
  KGroup *lookup( gid_t gid );
  KGroup *lookup_sam( const SID &sid );
  KGroup *lookup_sam( const QString &sid );
  KGroup *lookup_sam( uint rid );
  
  KGroup *first();
  KGroup *next();
  KGroup *operator[](uint num);
  uint count() const;

  void add(KGroup *group);
  void del(KGroup *group);
  void mod(KGroup *gold, const KGroup &gnew);
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
  KUserPrefsBase *mCfg;
  QPtrList<KGroup> mGroups;
  
  QPtrList<KGroup> mDel;
  QPtrList<KGroup> mAdd;
  QMap<KGroup*, KGroup> mMod;
  int caps;
  QString domsid;
};

}

#endif // _KU_GROUP_H_
