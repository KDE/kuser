/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
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

#include "ku_groupsystem.h"

#include <errno.h>
#include <grp.h>

#include <kdebug.h>

#include "ku_misc.h"

KU_GroupSystem::KU_GroupSystem( KU_PrefsBase *cfg ) : KU_Groups( cfg )
{
  caps = Cap_ReadOnly | Cap_Passwd;
}

KU_GroupSystem::~KU_GroupSystem()
{
}

bool KU_GroupSystem::reload()
{
  struct group *p;
  KU_Group group;

  setgrent();
  while ((p = getgrent()) != NULL) {
    group.setGID(p->gr_gid);
    group.setName(QString::fromLocal8Bit(p->gr_name));
    group.setPwd(QString::fromLocal8Bit(p->gr_passwd));

    char *u_name;
    int i = 0;
    while ((u_name = p->gr_mem[i])!=0) {
      group.addUser(QString::fromLocal8Bit(u_name));
      i++;
    }

    append(group);
    group = KU_Group();
  }

  endgrent();
  return true;
}
