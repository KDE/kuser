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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/
 
#include "sid.h"
 
SID::SID()
{
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
  return ( mSid.isEmpty() && mDom.isEmpty() );
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

QString SID::getSID() const
{
  return mSid;
}

uint SID::getRID() const
{
  return mRid;
}

QString SID::getDOM() const
{
  return mDom;
}
