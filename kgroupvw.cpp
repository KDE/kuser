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

#include "kglobal_.h"
#include "misc.h"

#include "kgroupvw.h"


KGroupViewItem::KGroupViewItem(KListView *parent, KU::KGroup *aku)
 : KListViewItem(parent), mGroup(aku)
{
}

int KGroupViewItem::compare( QListViewItem *i, int col, bool ascending ) const
{
  switch ( col ) {
    case 0: {
      gid_t gid1, gid2;

      gid1 = mGroup->getGID();
      gid2 = ((KGroupViewItem*) i)->mGroup->getGID();

      if ( gid1 == gid2 ) return 0;
      return ( gid1 < gid2) ? -1: 1;
    }
    case 2: {
      uint rid1,rid2;
      rid1 = mGroup->getSID().getRID();
      rid2 = ((KGroupViewItem*) i)->mGroup->getSID().getRID();
      if ( rid1 == rid2 ) return 0;
      return ( rid1 < rid2) ? -1: 1;
    }
    default:
      return QListViewItem::compare( i, col, ascending );
  }
}

QString KGroupViewItem::text(int num) const
{
  switch(num)
  {
     case 0: return QString::number(mGroup->getGID());
     case 1: return mGroup->getName();
     case 2: return ( mGroup->getCaps() & KU::KGroup::Cap_Samba ) ? 
       mGroup->getSID().getDOM() : QString::null;
     case 3: return ( mGroup->getCaps() & KU::KGroup::Cap_Samba ) ?
      QString::number( mGroup->getSID().getRID() ) : QString::null;
     case 4: {
       if ( mGroup->getCaps() & KU::KGroup::Cap_Samba ) {
         switch ( mGroup->getType() ) {
           case 2: return i18n("Domain");
           case 4: return i18n("Local");
           case 5: return i18n("Builtin");
           default: return i18n("Unknown");
         }
       } else {
         return QString::null;
       }
     }
     case 5: return mGroup->getDisplayName();
     case 6: return mGroup->getDesc();
  }
  return QString::null;
}


KGroupView::KGroupView(QWidget *parent, const char *name) 
  : KListView( parent, name ) 
{
  setSelectionMode( QListView::Extended );
}

KGroupView::~KGroupView() 
{
}

void KGroupView::insertItem(KU::KGroup *aku) 
{
  KGroupViewItem *groupItem = new KGroupViewItem(this, aku);
  KListView::insertItem(groupItem);
}

void KGroupView::removeItem(KU::KGroup *aku) 
{
  KGroupViewItem *groupItem = (KGroupViewItem *)firstChild();

  while(groupItem)
  {
     if (groupItem->group() == aku)
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
    setColumnAlignment(0, AlignRight);
    addColumn(i18n("Group Name"));
  }
  if ( kug->getGroups().getCaps() & KU::KGroups::Cap_Samba ) {
    addColumn(i18n("Domain SID"));
    addColumn(i18n("RID"));
    addColumn(i18n("Type"));
    addColumn(i18n("Display Name"));
    addColumn(i18n("Description"));
  }
}

KU::KGroup *KGroupView::getCurrentGroup() 
{
  KGroupViewItem *groupItem = (KGroupViewItem *)currentItem();
  if (!groupItem) return 0;

  return groupItem->group();
}

#include "kgroupvw.moc"
