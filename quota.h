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

#ifndef _KU_QUOTATOOL_H_
#define _KU_QUOTATOOL_H_

#ifdef _KU_QUOTA

#include <qptrlist.h>
#include <qintdict.h>

class QuotaMnt {
public:
  QuotaMnt();
  QuotaMnt(int afcur, int afsoft, int afhard, int aicur, int aisoft, int aihard);
  QuotaMnt(int afcur, int afsoft, int afhard, int aicur, int aisoft, int aihard, int aftime, int aitime);
  QuotaMnt(const QuotaMnt *q);
  ~QuotaMnt();

  long getfcur();
  long getfsoft();
  long getfhard();
  long geticur();
  long getisoft();
  long getihard();
  long getftime();
  long getitime();

  void setfcur(long data);
  void setfsoft(long data);
  void setfhard(long data);
  void seticur(long data);
  void setisoft(long data);
  void setihard(long data);
  void setftime(long data);
  void setitime(long data);

public:
  long
    fcur,
    fsoft,
    fhard,
    icur,
    isoft,
    ihard,
    ftime,
    itime;
};

class Quota {
public:
  Quota(uint auid, bool doget = TRUE);
  ~Quota();

  QuotaMnt *operator[](uint mntnum);
  uint getMountsNumber();
  uint getUid();
  bool save();
protected:
  uint uid;
  QPtrList<QuotaMnt> q;
};

class Quotas {
public:
  Quotas();
  ~Quotas();

  Quota *operator[](uint uid);
  void addQuota(uint uid);
  void addQuota(Quota *aq);
  void delQuota(uint uid);
  bool save();
 protected:
  QIntDict<Quota> q;
};

#endif // _KU_QUOTA

#endif // _KU_EDQUOTA_H_
