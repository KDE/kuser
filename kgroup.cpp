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

#include <qstring.h>

#include <kdebug.h>

#include "kglobal_.h"
#include "kgroup.h"
#include "misc.h"

KU::KGroup::KGroup()
{
  pwd = QString::fromLatin1("*");
  gid = 0;
  type = 2;
  caps = 0;
}

KU::KGroup::KGroup(KU::KGroup *group) 
{
  copy( group );
}

KU::KGroup::~KGroup()
{
}

void KU::KGroup::copy( const KU::KGroup *group )
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

void KU::KGroup::setCaps( int data )
{
  caps = data;
}

int KU::KGroup::getCaps()
{
  return caps;
}

const QString &KU::KGroup::getName() const 
{
  return name;
}

const QString &KU::KGroup::getPwd() const 
{
  return pwd;
}

gid_t KU::KGroup::getGID() const 
{
  return gid;
}

const SID &KU::KGroup::getSID() const
{
  return sid;
}

int KU::KGroup::getType() const
{
  return type;
}

const QString &KU::KGroup::getDisplayName() const
{
  return displayname;
}

const QString &KU::KGroup::getDesc() const
{
  return desc;
}

void KU::KGroup::setName(const QString &data) 
{
  name = data;
}

void KU::KGroup::setPwd(const QString &data) 
{
  pwd = data;
}

void KU::KGroup::setGID(gid_t data) 
{
  gid = data;
}

void KU::KGroup::setSID(const SID &data)
{
  sid = data;
}

void KU::KGroup::setType(int data)
{
  type = data;
}

void KU::KGroup::setDisplayName(const QString &data)
{
  displayname = data;
}

void KU::KGroup::setDesc(const QString &data)
{
  desc = data;
}

bool KU::KGroup::lookup_user(const QString &name) 
{
  return (u.find(name) != u.end());
}

bool KU::KGroup::addUser(const QString &name) 
{
  if (!lookup_user(name)) {
    u.append(name);
    return true;
  } else
    return false;
}

bool KU::KGroup::removeUser(const QString &name) 
{
  return ( u.remove(name) > 0 );
}

uint KU::KGroup::count() const 
{
  return u.count();
}

QString KU::KGroup::user(uint i) 
{
  return u[i];
}

void KU::KGroup::clear() 
{
  u.clear();
}

KU::KGroups::KGroups(KUserPrefsBase *cfg) 
{
  mGroups.setAutoDelete(TRUE);
  mCfg = cfg;
}

KU::KGroup *KU::KGroups::lookup(const QString &name) 
{
  KU::KGroup *group;
  QPtrListIterator<KU::KGroup> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getName() != name ) ++it;
  return group;
}

KU::KGroup *KU::KGroups::lookup(gid_t gid) 
{
  KU::KGroup *group;
  QPtrListIterator<KU::KGroup> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getGID() != gid ) ++it;
  return group;
}

KU::KGroup *KU::KGroups::lookup_sam( const SID &sid )
{
  KU::KGroup *group;
  QPtrListIterator<KU::KGroup> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getSID() != sid ) ++it;
  return group;
}

KU::KGroup *KU::KGroups::lookup_sam( const QString &sid )
{
  KU::KGroup *group;
  QPtrListIterator<KU::KGroup> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getSID().getSID() != sid ) ++it;
  return group;
}

KU::KGroup *KU::KGroups::lookup_sam( uint rid )
{
  KU::KGroup *group;
  QPtrListIterator<KU::KGroup> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getSID().getRID() != rid ) ++it;
  return group;
}

gid_t KU::KGroups::first_free() 
{
  gid_t t;

  for (t = mCfg->firstGID(); t<65534; t++)
    if (lookup(t) == NULL)
      return t;

  return NO_FREE;
}

uint KU::KGroups::first_free_sam()
{
  uint t;

  for (t = 30000; t<65534; t++)
    if (lookup_sam(t) == NULL)
      return t;

  return 0;
}

KU::KGroups::~KGroups() 
{
  mGroups.clear();
}

KU::KGroup *KU::KGroups::operator[](uint num) 
{
  return mGroups.at(num);
}

KU::KGroup *KU::KGroups::first() 
{
  return mGroups.first();
}

KU::KGroup *KU::KGroups::next() 
{
  return mGroups.next();
}

uint KU::KGroups::count() const 
{
  return mGroups.count();
}

const QString &KU::KGroups::getDOMSID() const
{
  return domsid;
}

void KU::KGroups::add(KU::KGroup *group) 
{
  kdDebug() << "adding group: " << group->getName() << " gid: " << group->getGID() << endl;
  mAdd.append( group );
}

void KU::KGroups::del(KU::KGroup *group) 
{
  kdDebug() << "deleting group: " << group->getName() << " gid: " << group->getGID() << endl;
  mDel.append( group );
}

void KU::KGroups::mod(KU::KGroup *gold, const KU::KGroup &gnew)
{
  kdDebug() << "modify group " << gnew.getName() << " gid: " << gnew.getGID() << endl;
  mMod.insert( gold, gnew );
}

void KU::KGroups::commit()
{
  kdDebug() << "KU::KGroups::commit()" << endl;
  KU::KGroup *group;
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

void KU::KGroups::cancelMods()
{
  KU::KGroup *group;
  while ( (group = mAdd.first()) ) {
    delete group;
    mAdd.remove();
  }
  mDel.clear();
  mMod.clear();
}
