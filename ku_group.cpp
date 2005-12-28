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

#include <kdebug.h>

#include "ku_group.h"

KU_Group::KU_Group()
{
  pwd = QString::fromLatin1("*");
  gid = 0;
  type = 2;
  caps = 0;
}

KU_Group::KU_Group(KU_Group *group) 
{
  copy( group );
}

KU_Group::~KU_Group()
{
}

void KU_Group::copy( const KU_Group *group )
{
  if ( group != this ) {
    caps    = group->caps;
    name    = group->name;
    pwd     = group->pwd;
    gid     = group->gid;
    sid     = group->sid;
    type    = group->type;
    displayname = group->displayname;
    desc    = group->desc;
    u       = group->u;
  }
}

void KU_Group::setCaps( int data )
{
  caps = data;
}

int KU_Group::getCaps()
{
  return caps;
}

const QString &KU_Group::getName() const 
{
  return name;
}

const QString &KU_Group::getPwd() const 
{
  return pwd;
}

gid_t KU_Group::getGID() const 
{
  return gid;
}

const SID &KU_Group::getSID() const
{
  return sid;
}

int KU_Group::getType() const
{
  return type;
}

const QString &KU_Group::getDisplayName() const
{
  return displayname;
}

const QString &KU_Group::getDesc() const
{
  return desc;
}

void KU_Group::setName(const QString &data) 
{
  name = data;
}

void KU_Group::setPwd(const QString &data) 
{
  pwd = data;
}

void KU_Group::setGID(gid_t data) 
{
  gid = data;
}

void KU_Group::setSID(const SID &data)
{
  sid = data;
}

void KU_Group::setType(int data)
{
  type = data;
}

void KU_Group::setDisplayName(const QString &data)
{
  displayname = data;
}

void KU_Group::setDesc(const QString &data)
{
  desc = data;
}

bool KU_Group::lookup_user(const QString &name) 
{
  return (u.find(name) != u.end());
}

bool KU_Group::addUser(const QString &name) 
{
  if (!lookup_user(name)) {
    u.append(name);
    return true;
  } else
    return false;
}

bool KU_Group::removeUser(const QString &name) 
{
  return ( u.remove(name) > 0 );
}

uint KU_Group::count() const 
{
  return u.count();
}

QString KU_Group::user(uint i) 
{
  return u[i];
}

void KU_Group::clear() 
{
  u.clear();
}

KU_Groups::KU_Groups(KU_PrefsBase *cfg) 
{
  mGroups.setAutoDelete(TRUE);
  mCfg = cfg;
}

KU_Group *KU_Groups::lookup(const QString &name) 
{
  KU_Group *group;
  Q3PtrListIterator<KU_Group> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getName() != name ) ++it;
  return group;
}

KU_Group *KU_Groups::lookup(gid_t gid) 
{
  KU_Group *group;
  Q3PtrListIterator<KU_Group> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getGID() != gid ) ++it;
  return group;
}

KU_Group *KU_Groups::lookup_sam( const SID &sid )
{
  KU_Group *group;
  Q3PtrListIterator<KU_Group> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getSID() != sid ) ++it;
  return group;
}

KU_Group *KU_Groups::lookup_sam( const QString &sid )
{
  KU_Group *group;
  Q3PtrListIterator<KU_Group> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getSID().getSID() != sid ) ++it;
  return group;
}

KU_Group *KU_Groups::lookup_sam( uint rid )
{
  KU_Group *group;
  Q3PtrListIterator<KU_Group> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getSID().getRID() != rid ) ++it;
  return group;
}

gid_t KU_Groups::first_free() 
{
  gid_t t;

  for (t = mCfg->firstGID(); t<65534; t++)
    if (lookup(t) == NULL)
      return t;

  return NO_FREE;
}

uint KU_Groups::first_free_sam()
{
  uint t;

  for (t = 30000; t<65534; t++)
    if (lookup_sam(t) == NULL)
      return t;

  return 0;
}

KU_Groups::~KU_Groups() 
{
  mGroups.clear();
}

KU_Group *KU_Groups::operator[](uint num) 
{
  return mGroups.at(num);
}

KU_Group *KU_Groups::first() 
{
  return mGroups.first();
}

KU_Group *KU_Groups::next() 
{
  return mGroups.next();
}

uint KU_Groups::count() const 
{
  return mGroups.count();
}

const QString &KU_Groups::getDOMSID() const
{
  return domsid;
}

void KU_Groups::add(KU_Group *group) 
{
  kdDebug() << "adding group: " << group->getName() << " gid: " << group->getGID() << endl;
  mAdd.append( group );
}

void KU_Groups::del(KU_Group *group) 
{
  kdDebug() << "deleting group: " << group->getName() << " gid: " << group->getGID() << endl;
  mDel.append( group );
}

void KU_Groups::mod(KU_Group *gold, const KU_Group &gnew)
{
  kdDebug() << "modify group " << gnew.getName() << " gid: " << gnew.getGID() << endl;
  mMod.insert( gold, gnew );
}

void KU_Groups::commit()
{
  kdDebug() << "KU_Groups::commit()" << endl;
  KU_Group *group;
  DelIt dit( mDelSucc );
  AddIt ait( mAddSucc );
  ModIt mit = mModSucc.begin();
    
  while ( mit != mModSucc.end() ) {
    *(mit.key()) = mit.data();
    mit++;
  }
  while ( (group = dit.current()) != 0 ) {
    ++dit;
    mGroups.remove( group );
  }
  while ( (group = ait.current()) != 0 ) {
    ++ait;
    mGroups.append( group );
  }
  cancelMods();  
}

void KU_Groups::cancelMods()
{
  KU_Group *group;
  while ( (group = mAdd.first()) ) {
    delete group;
    mAdd.remove();
  }
  mDel.clear();
  mMod.clear();
}
