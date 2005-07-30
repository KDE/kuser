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
 *  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#include <qstring.h>

#include <kdebug.h>

#include "kglobal_.h"
#include "kgroup.h"
#include "misc.h"

KGroup::KGroup()
{
  pwd = QString::fromLatin1("*");
  gid = 0;
  type = 2;
  caps = 0;
}

KGroup::KGroup(KGroup *group) 
{
  copy( group );
}

KGroup::~KGroup()
{
}

void KGroup::copy( const KGroup *group )
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

void KGroup::setCaps( int data )
{
  caps = data;
}

int KGroup::getCaps()
{
  return caps;
}

const QString &KGroup::getName() const 
{
  return name;
}

const QString &KGroup::getPwd() const 
{
  return pwd;
}

gid_t KGroup::getGID() const 
{
  return gid;
}

const SID &KGroup::getSID() const
{
  return sid;
}

int KGroup::getType() const
{
  return type;
}

const QString &KGroup::getDisplayName() const
{
  return displayname;
}

const QString &KGroup::getDesc() const
{
  return desc;
}

void KGroup::setName(const QString &data) 
{
  name = data;
}

void KGroup::setPwd(const QString &data) 
{
  pwd = data;
}

void KGroup::setGID(gid_t data) 
{
  gid = data;
}

void KGroup::setSID(const SID &data)
{
  sid = data;
}

void KGroup::setType(int data)
{
  type = data;
}

void KGroup::setDisplayName(const QString &data)
{
  displayname = data;
}

void KGroup::setDesc(const QString &data)
{
  desc = data;
}

bool KGroup::lookup_user(const QString &name) 
{
  return (u.find(name) != u.end());
}

bool KGroup::addUser(const QString &name) 
{
  if (!lookup_user(name)) {
    u.append(name);
    return true;
  } else
    return false;
}

bool KGroup::removeUser(const QString &name) 
{
  return ( u.remove(name) > 0 );
}

uint KGroup::count() const 
{
  return u.count();
}

QString KGroup::user(uint i) 
{
  return u[i];
}

void KGroup::clear() 
{
  u.clear();
}

KGroups::KGroups(KUserPrefsBase *cfg) 
{
  mGroups.setAutoDelete(TRUE);
  mCfg = cfg;
}

KGroup *KGroups::lookup(const QString &name) 
{
  KGroup *group;
  Q3PtrListIterator<KGroup> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getName() != name ) ++it;
  return group;
}

KGroup *KGroups::lookup(gid_t gid) 
{
  KGroup *group;
  Q3PtrListIterator<KGroup> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getGID() != gid ) ++it;
  return group;
}

KGroup *KGroups::lookup_sam( const SID &sid )
{
  KGroup *group;
  Q3PtrListIterator<KGroup> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getSID() != sid ) ++it;
  return group;
}

KGroup *KGroups::lookup_sam( const QString &sid )
{
  KGroup *group;
  Q3PtrListIterator<KGroup> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getSID().getSID() != sid ) ++it;
  return group;
}

KGroup *KGroups::lookup_sam( uint rid )
{
  KGroup *group;
  Q3PtrListIterator<KGroup> it( mGroups );
  
  while ( (group = it.current()) != 0 && group->getSID().getRID() != rid ) ++it;
  return group;
}

gid_t KGroups::first_free() 
{
  gid_t t;

  for (t = mCfg->firstGID(); t<65534; t++)
    if (lookup(t) == NULL)
      return t;

  return NO_FREE;
}

uint KGroups::first_free_sam()
{
  uint t;

  for (t = 30000; t<65534; t++)
    if (lookup_sam(t) == NULL)
      return t;

  return 0;
}

KGroups::~KGroups() 
{
  mGroups.clear();
}

KGroup *KGroups::operator[](uint num) 
{
  return mGroups.at(num);
}

KGroup *KGroups::first() 
{
  return mGroups.first();
}

KGroup *KGroups::next() 
{
  return mGroups.next();
}

uint KGroups::count() const 
{
  return mGroups.count();
}

const QString &KGroups::getDOMSID() const
{
  return domsid;
}

void KGroups::add(KGroup *group) 
{
  kdDebug() << "adding group: " << group->getName() << " gid: " << group->getGID() << endl;
  mAdd.append( group );
}

void KGroups::del(KGroup *group) 
{
  kdDebug() << "deleting group: " << group->getName() << " gid: " << group->getGID() << endl;
  mDel.append( group );
}

void KGroups::mod(KGroup *gold, const KGroup &gnew)
{
  kdDebug() << "modify group " << gnew.getName() << " gid: " << gnew.getGID() << endl;
  mMod.insert( gold, gnew );
}

void KGroups::commit()
{
  kdDebug() << "KGroups::commit()" << endl;
  KGroup *group;
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

void KGroups::cancelMods()
{
  KGroup *group;
  while ( (group = mAdd.first()) ) {
    delete group;
    mAdd.remove();
  }
  mDel.clear();
  mMod.clear();
}
