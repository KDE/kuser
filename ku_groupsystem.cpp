/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
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

#include <errno.h>
#include <grp.h>

#include <kdebug.h>
#include <kmessagebox.h>

#include "ku_groupsystem.h"
#include "ku_misc.h"

KU_GroupSystem::KU_GroupSystem( KU_PrefsBase *cfg ) : KU_Groups( cfg )
{
  caps = Cap_ReadOnly | Cap_Passwd;

  reload();
}

KU_GroupSystem::~KU_GroupSystem()
{
  mGroups.clear();
}

bool KU_GroupSystem::reload()
{
  struct group *p;
  KU_Group *tmpKG = 0;

  setgrent();
  while ((p = getgrent()) != NULL) {
    tmpKG = new KU_Group();
    tmpKG->setGID(p->gr_gid);
    tmpKG->setName(QString::fromLocal8Bit(p->gr_name));
    tmpKG->setPwd(QString::fromLocal8Bit(p->gr_passwd));

    char *u_name;
    int i = 0;
    while ((u_name = p->gr_mem[i])!=0) {
      tmpKG->addUser(QString::fromLocal8Bit(u_name));
      i++;
    }

    mGroups.append(tmpKG);
  }

  endgrent();
  return true;
}
