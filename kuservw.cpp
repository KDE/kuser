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

#include "kuservw.h"


KUserViewItem::KUserViewItem(KListView *parent, KU::KUser *aku)
 : KListViewItem(parent), mUser(aku)
{
}

int KUserViewItem::compare( QListViewItem *i, int col, bool ascending ) const
{
  if ( col == 0 ) {
    uid_t uid1, uid2;

    uid1 = mUser->getUID();
    uid2 = ((KUserViewItem*) i)->mUser->getUID();

    if ( uid1 == uid2 ) return 0;
    return ( uid1 < uid2) ? -1: 1;
  } else {
    return QListViewItem::compare( i, col, ascending );
  }
}

void KUserViewItem::paintCell( QPainter *p, const QColorGroup &cg,
                               int column, int width, int alignment )
{
    QColorGroup _cg( cg );
    QColor c = _cg.text();

    if ( mUser->getDisabled() )
        _cg.setColor( QColorGroup::Text, KGlobalSettings::visitedLinkColor() );

    KListViewItem::paintCell( p, _cg, column, width, alignment );

    _cg.setColor( QColorGroup::Text, c );
}

QString KUserViewItem::text(int num) const
{
  switch(num)
  {
     case 0: return mUser->getCaps() & KU::KUser::Cap_POSIX ? 
      QString::number( mUser->getUID() ) : QString::null;
     case 1: return mUser->getName();
     case 2: return mUser->getFullName();
     case 3: return mUser->getHomeDir();
     case 4: return mUser->getShell();
     case 5: return mUser->getSID().getDOM();
     case 6: return mUser->getCaps() & KU::KUser::Cap_Samba ? 
      QString::number( mUser->getSID().getRID() ) : QString::null;
     case 7: return mUser->getLoginScript();
     case 8: return mUser->getProfilePath();
     case 9: return mUser->getHomeDrive();
     case 10: return mUser->getHomePath();
  }

  return QString::null;
}

KUserView::KUserView(QWidget *parent, const char *name)
  : KListView( parent, name )
{
  setSelectionMode( QListView::Extended );
}

KUserView::~KUserView()
{
}

void KUserView::insertItem(KU::KUser *aku) {
  KUserViewItem *userItem = new KUserViewItem(this, aku);
  KListView::insertItem(userItem);
}

void KUserView::removeItem(KU::KUser *aku) {
  KUserViewItem *userItem = (KUserViewItem *)firstChild();

  while(userItem)
  {
     if (userItem->user() == aku)
     {
        delete userItem;
        return;
     }
     userItem = (KUserViewItem*) userItem->nextSibling();
  }
}

void KUserView::init()
{
  while ( columns() > 5 ) {
    removeColumn( 5 );
  }

  setAllColumnsShowFocus(true);
  if ( columns() < 5 ) {
    addColumn(i18n("UID"));
    setColumnAlignment(0, AlignRight);
    addColumn(i18n("User Login"));
    addColumn(i18n("Full Name"));
    addColumn(i18n("Home Directory"));
    addColumn(i18n("Login Shell"));
  }

  if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ) {
    addColumn(i18n("Domain SID"));
    addColumn(i18n("RID"));
    addColumn(i18n("Samba Login Script"));
    addColumn(i18n("Samba Profile Path"));
    addColumn(i18n("Samba Home Drive"));
    addColumn(i18n("Samba Home Path"));
  }
}

KU::KUser *KUserView::getCurrentUser() {
  KUserViewItem *userItem = (KUserViewItem *)currentItem();
  if (!userItem) return 0;

  return userItem->user();
}

#include "kuservw.moc"
