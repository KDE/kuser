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

#include <kdebug.h>
#include <QSharedData>

#include "ku_group.h"


KU_Group_Private::KU_Group_Private()
{
  Pwd = QString::fromLatin1("*");
  GID = 0;
  Type = 2;
  Caps = 0;
}

KU_Group::KU_Group()
{
  d = new KU_Group_Private;
}

KU_Group::KU_Group(KU_Group *group)
{
  d = new KU_Group_Private;
  copy( group );
}

KU_Group::~KU_Group()
{
}

void KU_Group::copy( const KU_Group *group )
{
  if ( group != this ) {
    *this = *group;
  }
}

bool KU_Group::operator ==(const KU_Group &other) const
{
  if ( getGID() == other.getGID() &&
       getName() == other.getName() )
    return true;
  else
    return false;
}

KU_PROPERTY_IMPL(KU_Group,int, Caps)

KU_PROPERTY_IMPL(KU_Group,QString, Name)
KU_PROPERTY_IMPL(KU_Group,QString, Pwd)
KU_PROPERTY_IMPL(KU_Group,gid_t, GID )

  //Samba
KU_PROPERTY_IMPL(KU_Group,SID, SID)
KU_PROPERTY_IMPL(KU_Group,int, Type)
KU_PROPERTY_IMPL(KU_Group,QString, DisplayName)
KU_PROPERTY_IMPL(KU_Group,QString, Desc)

bool KU_Group::lookup_user(const QString &name) const
{
  return d->users.contains(name);
}

bool KU_Group::addUser(const QString &name)
{
  if (!lookup_user(name)) {
    d->users.append(name);
    return true;
  } else
    return false;
}

bool KU_Group::removeUser(const QString &name)
{
  return ( d->users.removeAll(name) > 0 );
}

uint KU_Group::count() const
{
  return d->users.count();
}

QString KU_Group::user(uint i) const
{
  return d->users[i];
}

void KU_Group::clear()
{
  d->users.clear();
}

KU_Groups::KU_Groups(KU_PrefsBase *cfg)
{
  mCfg = cfg;
}

int KU_Groups::lookup(const QString &name) const
{
  for ( int i = 0; i<count(); i++ ) {
    if ( at(i).getName() == name ) return i;
  }
  return -1;
}

int KU_Groups::lookup(gid_t gid) const
{
  for ( int i = 0; i<count(); i++ ) {
    if ( at(i).getGID() == gid ) return i;
  }
  return -1;
}

int KU_Groups::lookup_sam( const SID &sid ) const
{
  for ( int i = 0; i<count(); i++ ) {
    if ( at(i).getSID() == sid ) return i;
  }
  return -1;
}

int KU_Groups::lookup_sam( const QString &sid ) const
{
  for ( int i = 0; i<count(); i++ ) {
    if ( at(i).getSID().getSID() == sid ) return i;
  }
  return -1;
}

int KU_Groups::lookup_sam( uint rid ) const
{
  for ( int i = 0; i<count(); i++ ) {
    if ( at(i).getSID().getRID() == rid ) return i;
  }
  return -1;
}

gid_t KU_Groups::first_free() const
{
  gid_t t;

  for (t = mCfg->firstGID(); t<65534; t++)
    if (lookup(t) == -1)
      return t;

  return NO_FREE;
}

uint KU_Groups::first_free_sam() const
{
  uint t;

  for (t = 30000; t<65534; t++)
    if (lookup_sam(t) == -1)
      return t;

  return 0;
}

KU_Groups::~KU_Groups()
{
}

const QString &KU_Groups::getDOMSID() const
{
  return domsid;
}

void KU_Groups::add(const KU_Group &group)
{
  kDebug() << "adding group: " << group.getName() << " gid: " << group.getGID();
  mAdd.append( group );
}

void KU_Groups::del(int index)
{
  kDebug() << "deleting group: " << at(index).getName() << " gid: " << at(index).getGID();
  mDel.append( index );
}

void KU_Groups::mod(int index, const KU_Group &newgroup)
{
  kDebug() << "modify group " << newgroup.getName() << " gid: " << newgroup.getGID();
  mMod.insert( index, newgroup );
}

void KU_Groups::commit()
{
  kDebug() << "KU_Groups::commit()";

  for ( ModList::Iterator it = mModSucc.begin(); it != mModSucc.end(); ++it ) {
    replace(it.key(),*it);
  }
  for ( AddList::Iterator it = mAddSucc.begin(); it != mAddSucc.end(); ++it ) {
    append(*it);
  }
  for ( DelList::Iterator it = mDelSucc.begin(); it != mDelSucc.end(); ++it ) {
    removeAt(*it);
  }
  cancelMods();
}

void KU_Groups::cancelMods()
{
  mAdd.clear();
  mDel.clear();
  mMod.clear();
}
