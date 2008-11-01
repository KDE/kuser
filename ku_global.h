/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi GyĂśrgy <gyurco@freemail.hu>
 *  Maintained by Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
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

#ifndef _KU_GLOBAL_H_
#define _KU_GLOBAL_H_

#include "ku_user.h"
#include "ku_group.h"

class KU_Global {
public:
  static void init();
  static void initCfg( const QString &connection );
  static void displayUsersError();
  static void displayGroupsError();

  static KU_PrefsBase *kcfg();
  static KU_Users *users();
  static KU_Groups *groups();

private:
  KU_Global();
  ~KU_Global();

  static KU_Users *mUsers;
  static KU_Groups *mGroups;

  static KU_PrefsBase *mCfg;
};

//extern KU_Global *kug;

#endif //_KU_GLOBAL_H_
