/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi GyĂśrgy <gyurco@freemail.hu>
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

#include <kglobal.h>
#include <kmessagebox.h>

#include "ku_global.h"
#include "ku_userfiles.h"
#include "ku_groupfiles.h"
#include "ku_userldap.h"
#include "ku_groupldap.h"
#include "ku_usersystem.h"
#include "ku_groupsystem.h"

void KU_Global::initCfg( const QString &connection )
{
  if ( mCfg ) {
    mCfg->writeConfig();
    delete mCfg;
  }
  KSharedConfig::Ptr config( KGlobal::config() );
  mCfg = new KU_PrefsBase( config, connection );
  mCfg->readConfig();
}

void KU_Global::displayUsersError()
{
  if ( mUsers->errorDetails().isEmpty() )
    KMessageBox::error( 0, mUsers->errorString() );
  else
    KMessageBox::detailedError( 0, mUsers->errorString(), mUsers->errorDetails() );
}

void KU_Global::displayGroupsError()
{
  if ( mGroups->errorDetails().isEmpty() )
    KMessageBox::error( 0, mGroups->errorString() );
  else
    KMessageBox::detailedError( 0, mGroups->errorString(), mGroups->errorDetails() );
}

void KU_Global::init()
{
  delete mUsers;
  delete mGroups;

  SID::setAlgRidBase( mCfg->samridbase() );
  kDebug() << "Algorithmic RID base: " << SID::getAlgRidBase();
  switch ( mCfg->source() ) {
    case KU_PrefsBase::EnumSource::Files:
      mUsers = new KU_UserFiles( mCfg );
      mGroups = new KU_GroupFiles( mCfg );
      break;
    case KU_PrefsBase::EnumSource::LDAP:
      mUsers = new KU_UserLDAP( mCfg );
      mGroups = new KU_GroupLDAP( mCfg );
      break;
    case KU_PrefsBase::EnumSource::System:
      mUsers = new KU_UserSystem( mCfg );
      mGroups = new KU_GroupSystem( mCfg );
      break;
    default:
      Q_ASSERT(0);
  }
  if ( !mUsers->reload() ) displayUsersError();
  if ( !mGroups->reload() ) displayGroupsError();
}

KU_Users *KU_Global::users()
{
  return mUsers;
}

KU_Groups *KU_Global::groups()
{
  return mGroups;
}

KU_PrefsBase *KU_Global::kcfg()
{
  return mCfg;
}

KU_Users *KU_Global::mUsers;
KU_Groups *KU_Global::mGroups;

KU_PrefsBase *KU_Global::mCfg;
