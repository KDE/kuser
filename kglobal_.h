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

#ifndef __KGLOBAL_H__
#define __KGLOBAL_H__

#include "globals.h"

#include "kuser.h"
#include "kgroup.h"
#include "quota.h"
#include "mnt.h"

/** Initializate and store pointers to the objects that manages users, groups, quotas, mounts, etc. */
class KUserGlobals {
public:
  /** Constructor, do nothing. */
  KUserGlobals();
  
  /** Initializate users (KUsers) and groups (KGroups) and if quota is enabled, mounts and quotas. */
  void init();
  
  /** Destructor delete the objectes created on init(). */
  ~KUserGlobals();
  
  /** Get the pointer to the users */  
  KUsers &getUsers();
  KGroups &getGroups();
  
#ifdef _KU_QUOTA
  Quotas &getQuotas();
  Mounts &getMounts();
#endif

private:
#ifdef _KU_QUOTA
  Mounts *mounts;
  Quotas *quotas;
#endif

  KUsers *users;
  KGroups *groups;
};

extern KUserGlobals *kug;

#endif
