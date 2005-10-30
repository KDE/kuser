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

#include <kapplication.h>
 
#include "kglobal_.h"
#include "kuserfiles.h"
#include "kgroupfiles.h"
#include "kuserldap.h"
#include "kgroupldap.h"
#include "kusersystem.h"
#include "kgroupsystem.h"

KUserGlobals::KUserGlobals() 
{
  cfg = 0;
  
  users = 0;
  groups = 0;
}

void KUserGlobals::initCfg( const QString &connection )
{
  if ( cfg ) {
    cfg->writeConfig();
    delete cfg;
  }    
  cfg = new KUserPrefsBase( kapp->sharedConfig(), connection );
  cfg->readConfig();
}

void KUserGlobals::init() 
{
  if ( users ) delete users;
  if ( groups ) delete groups;
  SID::setAlgRidBase( cfg->samridbase() );
  kdDebug() << "Algorithmic RID base: " << SID::getAlgRidBase() << endl;
  switch ( cfg->source() ) {
    case KUserPrefsBase::EnumSource::Files:
      users = new KUserFiles( cfg );
      groups = new KGroupFiles( cfg );
      break;
    case KUserPrefsBase::EnumSource::LDAP:
      users = new KUserLDAP( cfg );
      groups = new KGroupLDAP( cfg );
      break;
    case KUserPrefsBase::EnumSource::System:
      users = new KUserSystem( cfg );
      groups = new KGroupSystem( cfg );
      break;
  }
}

KUserGlobals::~KUserGlobals() 
{
  delete users;
  delete groups;
  delete cfg;
}

KU::KUsers &KUserGlobals::getUsers() 
{
  return (*users);
}

KU::KGroups &KUserGlobals::getGroups() 
{
  return (*groups);
}
