/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi GyĂśrgy <gyurco@freemail.hu>
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

#include <kapplication.h>

#include "ku_global.h"
#include "ku_userfiles.h"
#include "ku_groupfiles.h"
#include "ku_userldap.h"
#include "ku_groupldap.h"
#include "ku_usersystem.h"
#include "ku_groupsystem.h"

KU_Global::KU_Global()
{
  cfg = 0;

  users = 0;
  groups = 0;
}

void KU_Global::initCfg( const QString &connection )
{
  if ( cfg ) {
    cfg->writeConfig();
    delete cfg;
  }
  cfg = new KU_PrefsBase( kapp->sharedConfig(), connection );
  cfg->readConfig();
}

void KU_Global::init()
{
  if ( users ) delete users;
  if ( groups ) delete groups;
  SID::setAlgRidBase( cfg->samridbase() );
  kDebug() << "Algorithmic RID base: " << SID::getAlgRidBase() << endl;
  switch ( cfg->source() ) {
    case KU_PrefsBase::EnumSource::Files:
      users = new KU_UserFiles( cfg );
      groups = new KU_GroupFiles( cfg );
      break;
    case KU_PrefsBase::EnumSource::LDAP:
      users = new KU_UserLDAP( cfg );
      groups = new KU_GroupLDAP( cfg );
      break;
    case KU_PrefsBase::EnumSource::System:
      users = new KU_UserSystem( cfg );
      groups = new KU_GroupSystem( cfg );
      break;
  }
}

KU_Global::~KU_Global()
{
  delete users;
  delete groups;
  delete cfg;
}

KU_Users *KU_Global::getUsers()
{
  return users;
}

KU_Groups *KU_Global::getGroups()
{
  return groups;
}
