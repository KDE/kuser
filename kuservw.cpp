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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include "kglobal_.h"
#include "misc.h"

#include "kuservw.h"


KUserViewItem::KUserViewItem(KListView *parent, KUser *aku)
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
     case 0: return QString::fromLatin1("%1 ").arg(mUser->getUID(),6);
     case 1: return mUser->getName();
     case 2: return mUser->getFullName();
     case 3: return mUser->getHomeDir();
     case 4: return mUser->getShell();
     case 5: return QString::number( mUser->getSID().getRID() );
     case 6: return mUser->getLoginScript();
     case 7: return mUser->getProfilePath();
     case 8: return mUser->getHomeDrive();
     case 9: return mUser->getHomePath();
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

void KUserView::insertItem(KUser *aku) {
  KUserViewItem *userItem = new KUserViewItem(this, aku);
  KListView::insertItem(userItem);
}

void KUserView::removeItem(KUser *aku) {
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
    addColumn(i18n("Home directory"));
    addColumn(i18n("Login shell"));
  }
  
  if ( kug->getUsers().getCaps() & KUsers::Cap_Samba ) {
    addColumn(i18n("RID"));
    addColumn(i18n("Samba login script"));
    addColumn(i18n("Samba profile path"));
    addColumn(i18n("Samba home drive"));
    addColumn(i18n("Samba home path"));
  }
}

KUser *KUserView::getCurrentUser() {
  KUserViewItem *userItem = (KUserViewItem *)currentItem();
  if (!userItem) return 0;
  
  return userItem->user();
}

#include "kuservw.moc"
