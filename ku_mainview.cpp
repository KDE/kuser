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


#include <stdio.h>

#include <QHeaderView>

#include <kinputdialog.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>

#include "ku_misc.h"
#include "ku_edituser.h"
#include "ku_adduser.h"
#include "ku_deluser.h"
#include "ku_pwdlg.h"
#include "ku_editgroup.h"
#include "ku_global.h"

#include "ku_mainview.h"

KU_MainView::KU_MainView(QWidget *parent) : KTabWidget(parent)
{
  init();
}

void KU_MainView::init() {

  users = 0;
  groups = 0;
  usermodel = 0;
  groupmodel = 0;

  userview = new QTreeView( 0 );
  userview->setSelectionMode( QAbstractItemView::ExtendedSelection );
  userview->setItemsExpandable( false );
  userview->setRootIsDecorated( false );
  userview->setSortingEnabled( true );
  userview->setUniformRowHeights( true );
  addTab( userview, i18n("Users") );

  groupview = new QTreeView( 0 );
  groupview->setSelectionMode( QAbstractItemView::ExtendedSelection );
  groupview->setItemsExpandable( false );
  groupview->setRootIsDecorated( false );
  groupview->setSortingEnabled( true );
  groupview->setUniformRowHeights( true );
  addTab( groupview, i18n("Groups") );

  connect(userview, SIGNAL(activated(const QModelIndex&)), this, SLOT(userSelected()));
  connect(groupview, SIGNAL(activated(const QModelIndex&)), this, SLOT(groupSelected()));

  connect(this, SIGNAL(currentChanged(QWidget *)), this, SLOT(slotTabChanged()));
}

KU_MainView::~KU_MainView()
{
}

void KU_MainView::slotTabChanged()
{
  if (currentWidget() == userview)
  {
     emit userSelected(true);
     emit groupSelected(false);
  }
  else
  {
     emit userSelected(false);
     emit groupSelected(true);
  }
}

void KU_MainView::clearUsers()
{
//  lbusers->clear();
}

void KU_MainView::clearGroups()
{
//  lbgroups->clear();
}

void KU_MainView::reloadUsers()
{
  users = kug->getUsers();
  if ( usermodel == 0 ) {
    usermodel = new KU_UserModel;
    userproxymodel.setSourceModel( usermodel );
    userview->setModel( &userproxymodel );
  }
  userproxymodel.setFirstUser( mShowSys ? 0 : kug->kcfg()->firstUID() );
  usermodel->init();
  userview->sortByColumn( 0, Qt::AscendingOrder );
}

void KU_MainView::reloadGroups()
{
  groups = kug->getGroups();
  if ( groupmodel == 0 ) {
    groupmodel = new KU_GroupModel;
    groupproxymodel.setSourceModel( groupmodel );
    groupview->setModel( &groupproxymodel );
  }
  groupproxymodel.setFirstGroup( mShowSys ? 0 : kug->kcfg()->firstGID() );
  groupmodel->init();
  groupview->sortByColumn( 0, Qt::AscendingOrder );
}

bool KU_MainView::queryClose()
{
  return true;
}

void KU_MainView::setpwd()
{
  QModelIndexList selectedindexes = userview->selectionModel()->selectedIndexes();
  int count = selectedindexes.count() / usermodel->columnCount();
  
  if ( count == 0 ) return;
  if ( count > 1 ) {
    if ( KMessageBox::questionYesNo( 0,
      i18n("You have selected %1 users. Do you really want to change the password for all the selected users?",
		  count ), QString::null, KGuiItem(i18n("Change")), KGuiItem(i18n("Do Not Change")) ) == KMessageBox::No ) return;
  }
  KU_PwDlg d( this );
  if ( d.exec() != QDialog::Accepted ) return;

  KU_User user;
  int index;

  foreach( QModelIndex selectedindex, selectedindexes ) {
    if ( selectedindex.row() != 0 ) continue;
    index = userproxymodel.mapToSource(selectedindex).row();
    user = users->at( index );
    users->createPassword( user, d.getPassword() );
    user.setLastChange( now() );
    user.setDisabled( false );
    users->mod( index, user );
  }
  updateUsers();
}

void KU_MainView::userSelected()
{
  useredit();
}

void KU_MainView::groupSelected()
{
  grpedit();
}

void KU_MainView::useradd()
{
  KU_User user;

  setCurrentIndex(0);

  uid_t uid, rid = 0;
  bool samba = users->getCaps() & KU_Users::Cap_Samba;

  if ((uid = users->first_free()) == KU_Users::NO_FREE) {
    KMessageBox::sorry( 0, i18n("You have run out of uid space.") );
    return;
  }
/*
  if ( samba && (rid = users->first_free_sam()) == 0) {
    KMessageBox::sorry( 0, i18n("You have run out of user RID space.") );
    return;
  }
*/
  if ( samba ) rid = SID::uid2rid( uid );
  bool ok;
  QString name = KInputDialog::getText( QString::null,
    i18n("Please type the name of the new user:"),
    QString::null, &ok );

  if ( !ok ) return;

  if ( users->lookup( name ) != -1 ) {
    KMessageBox::sorry( 0, i18n("User with name %1 already exists.", name ) );
    return;
  }

  user.setCaps( samba ? KU_User::Cap_POSIX | KU_User::Cap_Samba : KU_User::Cap_POSIX );
  user.setUID( uid );
  user.setName( name );

  if ( samba ) {
    SID sid;
    sid.setDOM( users->getDOMSID() );
    sid.setRID( rid );
    user.setSID( sid );
    user.setProfilePath( kug->kcfg()->samprofilepath().replace( "%U",name ) );
    user.setHomePath( kug->kcfg()->samhomepath().replace( "%U", name ) );
    user.setHomeDrive( kug->kcfg()->samhomedrive() );
    user.setLoginScript( kug->kcfg()->samloginscript() );
    user.setDomain( kug->kcfg()->samdomain() );
  }

  user.setShell( kug->kcfg()->shell() );
  user.setHomeDir( kug->kcfg()->homepath().replace( "%U", name ) );
  if ( users->getCaps() & KU_Users::Cap_Shadow || samba ) {
    user.setLastChange( now() );
  }

  user.setMin( kug->kcfg()->smin() );
  user.setMax( kug->kcfg()->smax() );
  user.setWarn( kug->kcfg()->swarn() );
  user.setInactive( kug->kcfg()->sinact() );
  user.setExpire( kug->kcfg()->sneverexpire() ? (uint) -1 :
    (kug->kcfg()->sexpire()).toTime_t() );

  bool privgroup = kug->kcfg()->userPrivateGroup();

  if ( !privgroup ) user.setGID( kug->kcfg()->defaultgroup() );

  KU_AddUser au( user, privgroup, this );

  au.setCreateHomeDir( kug->kcfg()->createHomeDir() );
  au.setCopySkel( kug->kcfg()->copySkel() );

  if ( au.exec() == QDialog::Rejected ) {
    return;
  }
  user = au.getNewUser();
  kDebug() << " surname: " << user.getSurname() << endl;
  if ( privgroup ) {
    KU_Group group;
    int index;

    index = groups->lookup(user.getName());
    //if no group exists with the user's name, create one
    if ( index == -1 ) {
      gid_t gid;
      if ( groups->lookup( user.getUID() ) == -1 ) {
        gid = user.getUID();
      } else {
        gid = groups->first_free();
      }
      kDebug() << "private group GID: " << gid << endl;
      uid_t rid = 0;
//      if ( samba ) rid = kug->getGroups().first_free_sam();
      if ( samba ) rid = SID::gid2rid( gid );
      if ( gid == KU_Groups::NO_FREE || ( samba && rid == 0 ) ) {
        groups->cancelMods();
        return;
      }
      group.setGID( gid );
      if ( samba && ( user.getCaps() & KU_User::Cap_Samba ) ) {
        SID sid;
        sid.setDOM( groups->getDOMSID() );
        sid.setRID( rid );
        group.setSID( sid );
        group.setDisplayName( user.getName() );
        group.setCaps( KU_Group::Cap_Samba );
      }
      group.setName( user.getName() );
      groups->add( group );
    } else {
      group = groups->at(index);
    }
    user.setGID( group.getGID() );
    user.setPGSID( group.getSID() );
  }
  users->add( user );
  if ( !updateUsers() ) {
    groups->cancelMods();
    return;
  }
  updateGroups();
}

void KU_MainView::useredit()
{
  QList<int> selected;
  QModelIndexList selectedindexes = userview->selectionModel()->selectedIndexes();

  foreach( QModelIndex selectedindex, selectedindexes ) {
    if ( selectedindex.column() == 0 ) 
      selected.append( userproxymodel.mapToSource(selectedindex).row() );
  }
  if ( selected.isEmpty() ) return;

  KU_EditUser editUser( selected, this );
  if ( editUser.exec() == QDialog::Rejected ) return;

  KU_User user;
  foreach(int i, selected) {
    editUser.mergeUser( users->at(i), user );
    users->mod( i, user );
  }
  updateUsers();
  updateGroups();
}

void KU_MainView::userdel()
{
  QModelIndex currentindex = userview->selectionModel()->currentIndex();
  if ( !currentindex.isValid() ) return;

  int index = userproxymodel.mapToSource(currentindex).row();
  kDebug() << "selected index: " << index << endl;

  KU_User user = users->at(index);
  QString username = user.getName();
  gid_t gid = user.getGID();
  KU_DelUser dlg(&user, this);

  if ( dlg.exec() == QDialog::Rejected )
     return;

  user.setDeleteHome( dlg.getDeleteHomeDir() );
  user.setDeleteMailBox( dlg.getDeleteMailBox() );


  users->del( index );
  if ( !updateUsers() ) return;

  for ( int i = 0; i < groups->count(); i++ ) {
    KU_Group group = groups->at(i);
    kDebug() << "group: " << group.getName() << endl;
    if ( group.lookup_user( username ) ) {
      kDebug() << "group: " << group.getName() << " found user: " << username << endl;
      group.removeUser( username );
      groups->mod( i, group );
    }
  }

  if ( kug->kcfg()->userPrivateGroup() ) {

    int i = groups->lookup( gid );
    if ( i != -1 &&
      KMessageBox::questionYesNo( 0, i18n("You are using private groups.\n"
        "Do you want to delete the user's private group '%1'?",
         groups->at(i).getName()), QString::null,
        KStdGuiItem::del(), KGuiItem(i18n("Do Not Delete"))) == KMessageBox::Yes) {
      kDebug() << "del private group" << endl;
      groups->del( i );
    }
  }
  kDebug() << "update groups" << endl;
  updateGroups();

}

void KU_MainView::grpadd()
{
  setCurrentIndex(1);

  gid_t gid;

  if ( (gid = groups->first_free()) == KU_Groups::NO_FREE )
  {
    KMessageBox::sorry( 0, i18n("You have run out of gid space.") );
    return;
  }
/*
  if ( samba && (rid = kug->getGroups().first_free_sam()) == 0 )
  {
    KMessageBox::sorry( 0, i18n("You have run out of group RID space.") );
    return;
  }
*/
  KU_Group group;
  group.setGID(gid);
  if ( groups->getCaps() & KU_Groups::Cap_Samba ) {
    uid_t rid = SID::gid2rid( gid );
    SID sid;
    sid.setRID( rid );
    sid.setDOM( groups->getDOMSID() );
    group.setSID( sid );
  }
  KU_EditGroup egdlg( group, true );

  if ( egdlg.exec() == QDialog::Rejected ) {
    return;
  }
  groups->add(egdlg.getGroup());
  updateGroups();
}

void KU_MainView::grpedit()
{
  QModelIndex currentindex = groupview->selectionModel()->currentIndex();
  if ( !currentindex.isValid() ) return;

  int index = groupproxymodel.mapToSource(currentindex).row();
  kDebug() << "selected index: " << index << endl;

  KU_Group group = groups->at(index);

  kDebug() << "The SID for group " << group.getName() << " is: '" << group.getSID().getSID() << "'" << endl;
  if ( ( groups->getCaps() & KU_Groups::Cap_Samba ) &&
       ( group.getCaps() & KU_Group::Cap_Samba ) &&
         group.getSID().isEmpty() ) {
    SID sid;
    sid.setDOM( groups->getDOMSID() );
//    sid.setRID( kug->getGroups().first_free_sam() );
    sid.setRID( SID::gid2rid( group.getGID() ) );
    group.setSID( sid );
    kDebug() << "The new SID for group " << group.getName() << " is: " << sid.getSID() << endl;
  }
  KU_EditGroup egdlg( group, false );

  if ( egdlg.exec() == QDialog::Accepted ) {
    groups->mod( index, egdlg.getGroup() );
    updateGroups();
  }
}

void KU_MainView::grpdel()
{
  QList<int> selected;
  QModelIndexList selectedindexes = groupview->selectionModel()->selectedIndexes();

  foreach( QModelIndex selectedindex, selectedindexes ) {
    if ( selectedindex.column() == 0 ) 
      selected.append( groupproxymodel.mapToSource(selectedindex).row() );
  }
  if ( selected.isEmpty() ) return;

  KU_Group group;
  int index;

  for ( index = 0; index < selected.count(); index++ ) {
    group = groups->at( selected.at( index ) );

    KU_Users::const_iterator it = users->constBegin();
    while ( it != users->constEnd() ) {
      if ( it->getGID() == group.getGID() ) {
        KMessageBox::error( 0, i18n( "The group '%1' is the primary group of one or more users (such as '%2'); it cannot be deleted.", group.getName(), it->getName() ) );
        return;
      }
      ++it;
    }
  }

  if ( selected.count() == 1 ) {
      if (KMessageBox::warningContinueCancel( 0,
        i18n("Do you really want to delete the group '%1'?", group.getName()),
        QString::null, KStdGuiItem::del()) != KMessageBox::Continue) return;
  } else {
      if (KMessageBox::warningContinueCancel( 0,
        i18n("Do you really want to delete the %1 selected groups?", selected.count()),
        QString::null, KStdGuiItem::del()) != KMessageBox::Continue) return;
  }

  for ( index = 0; index < selected.count(); index++ ) {
    groups->del( selected.at( index ) );
  }
  updateGroups();
}

bool KU_MainView::updateUsers()
{
  bool ret;
  kDebug() << "updateUsers() " << endl;
  ret = users->dbcommit();

  if ( !ret ) {
    kug->displayUsersError();
  }

  usermodel->commitMod();
  usermodel->commitDel();
  usermodel->commitAdd();
  users->cancelMods();

  return ret;
}

bool KU_MainView::updateGroups()
{
  bool ret;
  kDebug() << "updateGroups() " << endl;
  ret = groups->dbcommit();

  if ( !ret ) {
    kug->displayGroupsError();
  }

  groupmodel->commitMod();
  groupmodel->commitDel();
  groupmodel->commitAdd();
  groups->cancelMods();

  return ret;
}

#include "ku_mainview.moc"
