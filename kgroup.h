/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#ifndef _KGROUP_H_
#define _KGROUP_H_

#include <sys/types.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>

#ifdef _KU_QUOTA
#include "quota.h"
#endif

class KGroup {
public:
  KGroup();
  KGroup(KGroup *copy);
  ~KGroup();

  const QString &getName() const;
  const QString &getPwd() const;
  gid_t getGID() const;

  void setName(const QString &data);
  void setPwd(const QString &data);
  void setGID(gid_t data);

  void addUser(const QString &name);
  void removeUser(const QString &name);
  bool lookup_user(const QString &name);
  uint count() const;
  QString user(uint i);
  void clear();

protected:
  QString
    name,
    pwd;
  gid_t gid;

  QStringList u;
};

class KGroups {
public:
  KGroups();
  ~KGroups();

  KGroup *lookup(const QString &name);
  KGroup *lookup(gid_t gid);
  int first_free();

  bool save();
  bool load();

  KGroup *first();
  KGroup *next();
  KGroup *operator[](uint num);

  void add(KGroup *ku);
  void del(KGroup *au);

  uint count() const;

protected:
  int gr_backuped;
  int gn_backuped;
  QPtrList<KGroup> g;

  int mode;
  uid_t uid;
  gid_t gid;
};

#endif // _KGROUP_H_

