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

#include <klocale.h>

#include "ku_global.h"
#include "ku_groupmodel.h"

int KU_GroupModel::rowCount( const QModelIndex & parent ) const
{
  Q_UNUSED(parent)

  return KU_Global::groups()->count();
}

int KU_GroupModel::columnCount( const QModelIndex & parent ) const
{
  Q_UNUSED(parent)

  if ( KU_Global::groups()->getCaps() & KU_Users::Cap_Samba ) 
    return 7;
  else
    return 2;
}

QVariant KU_GroupModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  Q_UNUSED(orientation)

  if ( role != Qt::DisplayRole ) return QVariant();
  switch ( section ) {
    case 0: return(i18n("GID"));
    case 1: return(i18n("Group Name"));
    case 2: return(i18n("Domain SID"));
    case 3: return(i18n("RID"));
    case 4: return(i18n("Type"));
    case 5: return(i18n("Display Name"));
    case 6: return(i18n("Description"));
    default: return QVariant();
  }
}

QVariant KU_GroupModel::data( const QModelIndex & index, int role ) const
{
  if ( !index.isValid() ) return QVariant();
  KU_Group group = KU_Global::groups()->at( index.row() );
  switch( role ) {
    case Qt::DisplayRole:
      switch( index.column() ) {
        case 0: return QString::number( group.getGID() );
        case 1: return group.getName();
        case 2: return ( group.getCaps() & KU_Group::Cap_Samba ) ? 
          group.getSID().getDOM() : QString();
        case 3: return ( group.getCaps() & KU_Group::Cap_Samba ) ?
          QString::number( group.getSID().getRID() ) : QString();
        case 4: 
	  if ( group.getCaps() & KU_Group::Cap_Samba ) {
            switch ( group.getType() ) {
              case 2: return i18n("Domain");
              case 4: return i18n("Local");
              case 5: return i18n("Builtin");
              default: return i18n("Unknown");
             }
           } else {
             return QVariant();
           }
        case 5: return group.getDisplayName();
        case 6: return group.getDesc();
      }	
  }
  return QVariant();
}

void KU_GroupModel::commitDel()
{
//Must do the deleting from the bigger indexes to the smaller ones
  qSort( KU_Global::groups()->mDelSucc );
  for ( int i = KU_Global::groups()->mDelSucc.count()-1; i >= 0; i-- ) {
    removeRows(KU_Global::groups()->mDelSucc.at(i), 1);
  }
  KU_Global::groups()->mDelSucc.clear();
}

void KU_GroupModel::commitAdd()
{
  if ( !KU_Global::groups()->mAddSucc.isEmpty() ) {
    insertRows( rowCount(), KU_Global::groups()->mAddSucc.count() );
    KU_Global::groups()->mAddSucc.clear();
  }
}

void KU_GroupModel::commitMod()
{
  for ( KU_Groups::ModList::Iterator it = KU_Global::groups()->mModSucc.begin(); 
    it != KU_Global::groups()->mModSucc.end(); ++it ) {
    
    KU_Global::groups()->replace(it.key(),*it);
    for( int i = 0; i<columnCount(); i++ ) {
      QModelIndex idx = index( it.key(), i );
      setData( idx, data(idx, Qt::DisplayRole), Qt::DisplayRole );
    }
  }
  KU_Global::groups()->mModSucc.clear();
}

bool KU_GroupModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
  Q_UNUSED(value)
  Q_UNUSED(role)

  emit dataChanged( index, index );
  return true;
}

bool KU_GroupModel::insertRows( int row, int count, const QModelIndex & parent )
{
  beginInsertRows( parent, row, row+count-1 );
  for ( KU_Groups::AddList::Iterator it = KU_Global::groups()->mAddSucc.begin(); 
    it != KU_Global::groups()->mAddSucc.end(); ++it ) {

      KU_Global::groups()->append(*it);
  }
  endInsertRows();
  return true;
}

bool KU_GroupModel::removeRows( int row, int count, const QModelIndex & parent )
{
  beginRemoveRows( parent, row, row+count-1 );
  KU_Global::groups()->removeAt( row );
  endRemoveRows();
  return true;
}

bool KU_GroupSortingProxyModel::lessThan( const QModelIndex & left, const QModelIndex & right ) const
{
  if ( !left.isValid() || !right.isValid() ) return false;
  if ( left.column() == 0 ) {
    gid_t gid1, gid2;
    gid1 = KU_Global::groups()->at( left.row() ).getGID();
    gid2 = KU_Global::groups()->at( right.row() ).getGID();
    return gid1<gid2;
  } else
    return QSortFilterProxyModel::lessThan( left, right );
}

bool KU_GroupSortingProxyModel::filterAcceptsRow( int source_row, const QModelIndex & source_parent ) const
{
    Q_UNUSED(source_parent)

    gid_t gid;
    gid = KU_Global::groups()->at( source_row ).getGID();
    if ( gid >= mFirstGroup )
        return true;
    return false;
}
