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

#include "kglobal_.h"

KUserGlobals::KUserGlobals() {
}

void KUserGlobals::init() {
#ifdef _KU_QUOTA
  mounts = new Mounts;
  quotas = new Quotas;
#endif
  
  users = new KUsers;
  groups = new KGroups;
}

KUserGlobals::~KUserGlobals() {
  // FIXME: If the KUserGlobals object is deleted without having called init() before the following code is very likely to segfault.
#ifdef _KU_QUOTA
  delete mounts;
  delete quotas;
#endif
  delete users;
  delete groups;
}

KUsers &KUserGlobals::getUsers() {
  return (*users);
}

KGroups &KUserGlobals::getGroups() {
  return (*groups);
}

#ifdef _KU_QUOTA
Mounts &KUserGlobals::getMounts() {
  return (*mounts);
}

Quotas &KUserGlobals::getQuotas() {
  return (*quotas);
}
#endif

