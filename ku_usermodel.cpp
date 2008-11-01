/*
 *  Copyright (c) 2006 Szombathelyi György <gyurco@freemail.hu>
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

#include <KColorScheme>
#include <klocale.h>
#include "ku_global.h"
#include "ku_usermodel.h"

int KU_UserModel::rowCount( const QModelIndex & parent ) const
{
  Q_UNUSED(parent)

  return KU_Global::users()->count();
}

int KU_UserModel::columnCount( const QModelIndex & parent ) const
{
  Q_UNUSED(parent)

  if ( KU_Global::users()->getCaps() & KU_Users::Cap_Samba ) 
    return 11;
  else
    return 5;
}

QVariant KU_UserModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  Q_UNUSED(orientation)

  if ( role != Qt::DisplayRole ) return QVariant();
  switch ( section ) {
    case 0: return(i18n("UID"));
    case 1: return(i18n("User Login"));
    case 2: return(i18n("Full Name"));
    case 3: return(i18n("Home Directory"));
    case 4: return(i18n("Login Shell"));
    case 5: return(i18n("Domain SID"));
    case 6: return(i18n("RID"));
    case 7: return(i18n("Samba Login Script"));
    case 8: return(i18n("Samba Profile Path"));
    case 9: return(i18n("Samba Home Drive"));
    case 10: return(i18n("Samba Home Path"));
    default: return QVariant();
  }
}

QVariant KU_UserModel::data( const QModelIndex & index, int role ) const
{
  if ( !index.isValid() ) return QVariant();
  KU_User user = KU_Global::users()->at( index.row() );
  switch( role ) {
    case Qt::DisplayRole:
      switch( index.column() ) {
        case 0: return user.getCaps() & KU_User::Cap_POSIX ?
          QString::number( user.getUID() ) : QString();
        case 1: return user.getName();
        case 2: return user.getFullName();
        case 3: return user.getHomeDir();
        case 4: return user.getShell();
        case 5: return user.getSID().getDOM();
        case 6: return user.getCaps() & KU_User::Cap_Samba ?
          QString::number( user.getSID().getRID() ) : QString();
        case 7: return user.getLoginScript();
        case 8: return user.getProfilePath();
        case 9: return user.getHomeDrive();
        case 10: return user.getHomePath();
        default: return QVariant();
      }
    case Qt::TextColorRole:
      if ( user.getDisabled() ) return KColorScheme( QPalette::Active, KColorScheme::View ).foreground( KColorScheme::VisitedText );
  }
  return QVariant();
}

void KU_UserModel::commitDel()
{
//Must do the deleting from the bigger indexes to the smaller ones
  qSort( KU_Global::users()->mDelSucc );
  for ( int i = KU_Global::users()->mDelSucc.count()-1; i >= 0; i-- ) {
    removeRows(KU_Global::users()->mDelSucc.at(i), 1);
  }
  KU_Global::users()->mDelSucc.clear();
}

void KU_UserModel::commitAdd()
{
  if ( !KU_Global::users()->mAddSucc.isEmpty() ) {
    insertRows( rowCount(), KU_Global::users()->mAddSucc.count() );
    KU_Global::users()->mAddSucc.clear();
  }
}

void KU_UserModel::commitMod()
{
  for ( KU_Users::ModList::Iterator it = KU_Global::users()->mModSucc.begin(); 
    it != KU_Global::users()->mModSucc.end(); ++it ) {
    
    KU_Global::users()->replace(it.key(),*it);
    for( int i = 0; i<columnCount(); i++ ) {
      QModelIndex idx = index( it.key(), i );
      setData( idx, data(idx, Qt::DisplayRole), Qt::DisplayRole );
    }
  }
  KU_Global::users()->mModSucc.clear();
}

bool KU_UserModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
  Q_UNUSED(value)
  Q_UNUSED(role)

  emit dataChanged( index, index );
  return true;
}

bool KU_UserModel::insertRows( int row, int count, const QModelIndex & parent )
{
  beginInsertRows( parent, row, row+count-1 );
  for ( KU_Users::AddList::Iterator it = KU_Global::users()->mAddSucc.begin(); 
    it != KU_Global::users()->mAddSucc.end(); ++it ) {
    
      KU_Global::users()->append(*it);
  }
  endInsertRows();
  return true;
}

bool KU_UserModel::removeRows( int row, int count, const QModelIndex & parent )
{
  beginRemoveRows( parent, row, row+count-1 );
  KU_Global::users()->removeAt( row );
  endRemoveRows();
  return true;
}

bool KU_UserSortingProxyModel::lessThan( const QModelIndex & left, const QModelIndex & right ) const
{
  if ( !left.isValid() || !right.isValid() ) return false;
  if ( left.column() == 0 ) {
    uid_t uid1, uid2;
    uid1 = KU_Global::users()->at( left.row() ).getUID();
    uid2 = KU_Global::users()->at( right.row() ).getUID();
    return uid1<uid2;
  } else
    return QSortFilterProxyModel::lessThan( left, right );
}

bool KU_UserSortingProxyModel::filterAcceptsRow( int source_row, const QModelIndex & source_parent ) const
{
    Q_UNUSED(source_parent)

    uid_t uid;
    uid = KU_Global::users()->at( source_row ).getUID();
    if ( uid >= mFirstUser )
      return true;
    return false;
}
