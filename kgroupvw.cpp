/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Maintained by Adriaan de Groot <groot@kde.org>
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

#include "misc.h"

#include "kgroupvw.h"

class KListViewGroup : public QListViewItem
{
public:
  KListViewGroup(QListView *parent, KGroup *aku);
  virtual QString text ( int ) const;

  KGroup *group;
};

KListViewGroup::KListViewGroup(QListView *parent, KGroup *aku)
 : QListViewItem(parent), group(aku)
{
}

QString KListViewGroup::text(int num) const
{
  switch(num)
  {
     case 0: return QString::fromLatin1("%1 ").arg(group->getGID(),6);
     case 1: return group->getName();
  }
  return QString::null;
}


KGroupView::KGroupView(QWidget *parent, const char *name) 
  : KListView( parent, name ) 
{
  init();
}

KGroupView::~KGroupView() {
}

void KGroupView::insertItem(KGroup *aku) {
  KListViewGroup *groupItem = new KListViewGroup(this, aku);
  KListView::insertItem(groupItem);
  setSelected(groupItem, true);
}

void KGroupView::removeItem(KGroup *aku) {
  KListViewGroup *groupItem = (KListViewGroup *)firstChild();
  
  while(groupItem)
  {
     if (groupItem->group == aku)
     {
        delete groupItem;
        return;
     }
     groupItem = (KListViewGroup*) groupItem->nextSibling();
  }
}

void KGroupView::init() 
{
  setAllColumnsShowFocus(true);
  addColumn(i18n("GID"));
  setColumnAlignment(0, AlignRight);
  addColumn(i18n("Group Name"));
}

KGroup *KGroupView::getCurrentGroup() {
  KListViewGroup *groupItem = (KListViewGroup *)currentItem();
  if (!groupItem) return 0;

  return groupItem->group;
}

#include "kgroupvw.moc"
