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

#include "kuservw.h"

class KListViewUser : public QListViewItem
{
public:
  KListViewUser(QListView *parent, KUser *aku);
  virtual QString text ( int ) const;

  KUser *user;
};

KListViewUser::KListViewUser(QListView *parent, KUser *aku)
 : QListViewItem(parent), user(aku)
{
}

QString KListViewUser::text(int num) const
{
  switch(num)
  {
     case 0: return QString::fromLatin1("%1 ").arg(user->getUID(),6);
     case 1: return user->getName();
     case 2: return user->getFullName();
  }
  return QString::null;
}

KUserView::KUserView(QWidget *parent, const char *name) 
  : KListView( parent, name )
{
  init();
}

KUserView::~KUserView()
{
}

void KUserView::insertItem(KUser *aku) {
  KListViewUser *userItem = new KListViewUser(this, aku);
  KListView::insertItem(userItem);
  setSelected(userItem, true);
}

void KUserView::removeItem(KUser *aku) {
  KListViewUser *userItem = (KListViewUser *)firstChild();
  
  while(userItem)
  {
     if (userItem->user == aku)
     {
        delete userItem;
        return;
     }
     userItem = (KListViewUser*) userItem->nextSibling();
  }
}

void KUserView::init()
{
  setAllColumnsShowFocus(true);
  addColumn(i18n("UID"));
  setColumnAlignment(0, AlignRight);
  addColumn(i18n("User Login"));
  addColumn(i18n("Full Name"));
}

KUser *KUserView::getCurrentUser() {
  KListViewUser *userItem = (KListViewUser *)currentItem();
  if (!userItem) return 0;
  
  return userItem->user;
}

#include "kuservw.moc"
