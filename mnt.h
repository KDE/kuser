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

#ifndef _KU_MNT_H_
#define _KU_MNT_H_

#include <qstring.h>
#include <qptrlist.h>

class MntEnt {
public:
  MntEnt() {
  }
  
  MntEnt(const QString &afsname, const QString &adir,
         const QString &atype, const QString &aopts,
         const QString &aquotafilename);
  ~MntEnt();

  QString getfsname() const;
  QString getdir() const;
  QString gettype() const;
  QString getopts() const;
  QString getquotafilename() const;

  void setfsname(const QString &data);
  void setdir(const QString &data);
  void settype(const QString &data);
  void setopts(const QString &data);
  void setquotafilename(const QString &data);

public:
  QString fsname;
  QString dir;
  QString type;
  QString opts;
  QString quotafilename;
};

class Mounts {
public:
  Mounts();
  ~Mounts();

  MntEnt *operator[](uint num);
  uint getMountsNumber();
protected:
  QPtrList<MntEnt> m;
};

#endif // _KU_MNT_H_
