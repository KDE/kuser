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

#include <klocale.h>

#include "ku_global.h"
#include "kgroupvw.h"


KGroupViewItem::KGroupViewItem(KListView *parent, int index)
 : KListViewItem(parent), mIndex(index)
{
}

int KGroupViewItem::compare( Q3ListViewItem *i, int col, bool ascending ) const
{
  KU_Groups *groups = kug->getGroups();
  switch ( col ) {
    case 0: {
      gid_t gid1, gid2;

      gid1 = groups->at(mIndex).getGID();
      gid2 = groups->at(((KGroupViewItem*) i)->mIndex).getGID();

      if ( gid1 == gid2 ) return 0;
      return ( gid1 < gid2) ? -1: 1;
    }
    case 2: {
      uint rid1,rid2;
      rid1 = groups->at(mIndex).getSID().getRID();
      rid2 = groups->at(((KGroupViewItem*) i)->mIndex).getSID().getRID();
      if ( rid1 == rid2 ) return 0;
      return ( rid1 < rid2) ? -1: 1;
    }
    default:
      return Q3ListViewItem::compare( i, col, ascending );
  }
}

QString KGroupViewItem::text(int num) const
{
  KU_Group group = kug->getGroups()->at(mIndex);
  switch(num)
  {
     case 0: return QString::number(group.getGID());
     case 1: return group.getName();
     case 2: return ( group.getCaps() & KU_Group::Cap_Samba ) ? 
       group.getSID().getDOM() : QString::null;
     case 3: return ( group.getCaps() & KU_Group::Cap_Samba ) ?
      QString::number( group.getSID().getRID() ) : QString::null;
     case 4: {
       if ( group.getCaps() & KU_Group::Cap_Samba ) {
         switch ( group.getType() ) {
           case 2: return i18n("Domain");
           case 4: return i18n("Local");
           case 5: return i18n("Builtin");
           default: return i18n("Unknown");
         }
       } else {
         return QString::null;
       }
     }
     case 5: return group.getDisplayName();
     case 6: return group.getDesc();
  }
  return QString::null;
}


KGroupView::KGroupView(QWidget *parent, const char *name) 
  : KListView( parent ) 
{
  setSelectionMode( Q3ListView::Extended );
}

KGroupView::~KGroupView() 
{
}

void KGroupView::insertItem(int index) 
{
  KGroupViewItem *groupItem = new KGroupViewItem(this, index);
  KListView::insertItem(groupItem);
}

void KGroupView::removeItem(int index) 
{
  KGroupViewItem *groupItem = (KGroupViewItem *)firstChild();

  while(groupItem)
  {
     if (groupItem->index() == index)
     {
        delete groupItem;
        return;
     }
     groupItem = (KGroupViewItem*) groupItem->nextSibling();
  }
}

void KGroupView::init() 
{
  while ( columns() > 2 ) {
    removeColumn( 2 );
  }
  setAllColumnsShowFocus(true);

  if ( columns() < 2 ) {
    addColumn(i18n("GID"));
    setColumnAlignment(0, Qt::AlignRight);
    addColumn(i18n("Group Name"));
  }
  if ( kug->getGroups()->getCaps() & KU_Groups::Cap_Samba ) {
    addColumn(i18n("Domain SID"));
    addColumn(i18n("RID"));
    addColumn(i18n("Type"));
    addColumn(i18n("Display Name"));
    addColumn(i18n("Description"));
  }
}

int KGroupView::getCurrentIndex() 
{
  KGroupViewItem *groupItem = (KGroupViewItem *)currentItem();
  if (!groupItem) return -1;

  return groupItem->index();
}

#include "kgroupvw.moc"
