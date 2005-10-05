/*
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
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
 
#include "sid.h"
#include <kdebug.h> 
//From Samba
/* Take the bottom bit. */
#define RID_MULTIPLIER 2

/* The two common types. */
#define USER_RID_TYPE 0
#define GROUP_RID_TYPE 1

uint SID::mAlgRidBase = 1000;

SID::SID()
{
  mRid = 0; mSid = QString::null; mDom = QString::null;
}

SID::SID( const SID &sid )
{
  setSID( sid.getSID() );
}

SID::SID( const QString &sid )
{
  setSID( sid );
}

SID::~SID()
{
}

uint SID::uid2rid( uint uid )
{
  return( (( uid*RID_MULTIPLIER ) + mAlgRidBase ) | USER_RID_TYPE );
}

uint SID::gid2rid( uint gid )
{
  return( (( gid*RID_MULTIPLIER ) + mAlgRidBase ) | GROUP_RID_TYPE );
}

bool SID::operator == ( const SID &sid ) const
{
  return ( mSid  == sid.mSid && mDom == sid.mDom );
}

bool SID::operator != ( const SID&sid ) const
{
  return ( mSid  != sid.mSid || mDom != sid.mDom );
}

bool SID::isEmpty() const
{
  return ( ( mSid.isEmpty() || mRid == 0 ) && mDom.isEmpty() );
}

void SID::updateSID()
{
  mSid = mDom + QString::fromLatin1("-") + QString::number( mRid );
}

void SID::setSID( const QString &sid )
{
  int pos;
  QString rid;
  
  mSid = sid;
  pos = sid.findRev( '-' );
  mDom = sid.left( pos );
  rid = sid.right( sid.length() - pos - 1 );
  mRid = rid.toUInt();
}

void SID::setRID( const QString &rid )
{
  mRid = rid.toUInt();
  updateSID();
}

void SID::setRID( uint rid )
{
  mRid = rid;
  updateSID();
}

void SID::setDOM( const QString &dom )
{
  mDom = dom;
  updateSID();
}

const QString& SID::getSID() const
{
  return mSid;
}

uint SID::getRID() const
{
  return mRid;
}

const QString& SID::getDOM() const
{
  return mDom;
}
