/*
 *  Copyright (c) 2004 Szombathelyi Gy�rgy <gyurco@freemail.hu>
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
 
#ifndef _SID_H_
#define _SID_H_

#include <qstring.h>
 
class SID {
    
public:
  SID();
  SID( const QString &sid );
  SID( const SID &sid );
  ~SID();

  bool operator== ( const SID &sid ) const;
  bool operator!= ( const SID &sid ) const;
  bool isEmpty() const;
  void setSID( const QString &sid );
  void setRID( const QString &rid );
  void setRID( uint rid );
  void setDOM( const QString &dom );
  QString getSID() const;
  uint getRID() const ;
  QString getDOM() const;

private:
  void updateSID();
  QString mSid, mDom;
  uint mRid;
};

#endif //_SID_H_