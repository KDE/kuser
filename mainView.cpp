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

#include "mainView.h"

#include <stdio.h>

#include <qtooltip.h>
#include <qsplitter.h>
#include <qfile.h>

#include <kinputdialog.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "misc.h"
#include "kglobal_.h"
#include "propdlg.h"
#include "addUser.h"
#include "delUser.h"
#include "pwddlg.h"
#include "editGroup.h"
#include "editDefaults.h"

mainView::mainView(QWidget *parent) : QTabWidget(parent) 
{
  init();
}

void mainView::init() {

  lbusers = new KUserView( this, "lbusers" );
  addTab( lbusers, i18n("Users") );

  lbgroups = new KGroupView( this, "lbgroups" );
  addTab( lbgroups, i18n("Groups"));

  connect(lbusers, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(userSelected()));
  connect(lbusers, SIGNAL(returnPressed(QListViewItem *)), this, SLOT(userSelected()));

  connect(lbgroups, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(groupSelected()));
  connect(lbgroups, SIGNAL(returnPressed(QListViewItem *)), this, SLOT(groupSelected()));

  connect(this, SIGNAL(currentChanged(QWidget *)), this, SLOT(slotTabChanged()));
}

mainView::~mainView() 
{
}

void mainView::slotTabChanged()
{
  if (currentPage() == lbusers)
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

void mainView::reloadUsers() 
{
  KUser *ku;

  lbusers->clear();
  lbusers->init();
  uint uid = kug->kcfg()->firstUID();
  
  ku = kug->getUsers().first();
  while ( ku ) {
    if ( ku->getUID() >= uid || mShowSys ) lbusers->insertItem( ku );
    ku = kug->getUsers().next();
  }
  if (lbusers->firstChild())
    lbusers->setSelected(lbusers->firstChild(), true);
}

void mainView::reloadGroups() 
{
  KGroup *kg;

  lbgroups->clear();
  lbgroups->init();
  uint gid = kug->kcfg()->firstGID();

  kg = kug->getGroups().first();
  while ( kg ) {
    if ( kg->getGID() >= gid || mShowSys ) lbgroups->insertItem(kg);
    kg = kug->getGroups().next();
  }
}

void mainView::useredit() 
{
  userSelected();
}

void mainView::userdel() 
{
  KUser *user = lbusers->getCurrentUser();
  if (!user)
    return;

  QString username = user->getName();
  gid_t gid = user->getGID();
  delUser dlg(user, this);

  if ( dlg.exec() == QDialog::Rejected )
     return;

  user->setDeleteHome( dlg.getDeleteHomeDir() );
  user->setDeleteMailBox( dlg.getDeleteMailBox() );
  
  kug->getUsers().del( user );
  if ( !updateUsers() ) return;
    
  KGroup *group = 0;
  group = kug->getGroups().first();
  while ( group ) {
    kdDebug() << "group: " << group->getName() << endl;
    if ( group->lookup_user( username ) ) {
      kdDebug() << "group: " << group->getName() << " found user: " << username << endl;
      KGroup newgroup( group );
      newgroup.removeUser( username );
      kug->getGroups().mod( group, newgroup );
    }
    group = kug->getGroups().next();
  }
  
  if ( kug->kcfg()->userPrivateGroup() ) {

    group = kug->getGroups().lookup( gid );

    if ( group &&
      KMessageBox::warningContinueCancel( 0, i18n("You are using private groups.\n"
        "Do you want to delete the user's private group '%1'?")
        .arg(group->getName()), QString::null,
        i18n("&Delete")) == KMessageBox::Continue) {

      kug->getGroups().del( group );
    }
  }
  updateGroups();
}

void mainView::useradd() 
{
  KUser *tk;

  showPage(lbusers);

  int uid;
  uint rid;
  bool samba = kug->getUsers().getCaps() & KUsers::Cap_Samba;

  if ((uid = kug->getUsers().first_free()) == -1) {
    KMessageBox::sorry( 0, i18n("You have run out of uid space.") );
    return;
  }
  if ( samba && (rid = kug->getUsers().first_free_sam()) == 0) {
    KMessageBox::sorry( 0, i18n("You have run out of user RID space.") );
    return;
  }

  bool ok;
  QString name = KInputDialog::getText( QString::null, 
    i18n("Please type the name of the new user."), 
    QString::null, &ok );
    
  if ( !ok ) return;
  
  if ( kug->getUsers().lookup( name ) ) {
    KMessageBox::sorry( 0, i18n("User with name %1 already exists.").arg( name ) );
    return;
  }
    
  tk = new KUser();
  tk->setUID( uid );
  tk->setName( name );
  
  if ( samba ) {
    SID sid;
    sid.setDOM( kug->getUsers().getDOMSID() );
    sid.setRID( rid );
    tk->setSID( sid );
    tk->setProfilePath( kug->kcfg()->samprofilepath().replace( "%U",name ) );
    tk->setHomePath( kug->kcfg()->samhomepath().replace( "%U", name ) );
    tk->setHomeDrive( kug->kcfg()->samhomedrive() );
    tk->setLoginScript( kug->kcfg()->samloginscript() );
  }
  
  tk->setShell( kug->kcfg()->shell() );
  tk->setHomeDir( kug->kcfg()->homepath().replace( "%U", name ) );
  if ( kug->getUsers().getCaps() & KUsers::Cap_Shadow || samba ) {
    tk->setLastChange( now() );
  }
/*  
  if (config->hasKey("s_min"))
    tk->setMin(config->readNumEntry("s_min"));

  if (config->hasKey("s_max"))
    tk->setMax(config->readNumEntry("s_max"));

  if (config->hasKey("s_warn"))
    tk->setWarn(config->readNumEntry("s_warn"));

  if (config->hasKey("s_inact"))
    tk->setInactive(config->readNumEntry("s_inact"));

  if (config->hasKey("s_expire"))
    tk->setExpire(config->readNumEntry("s_expire"));

  if (config->hasKey("s_flag"))
    tk->setFlag(config->readNumEntry("s_flag"));
*/
  
  bool privgroup = kug->kcfg()->userPrivateGroup();

  addUser au( tk, privgroup, this );
  
  au.setCreateHomeDir( kug->kcfg()->createHomeDir() );
  au.setCopySkel( kug->kcfg()->copySkel() );
  
  if ( au.exec() == QDialog::Rejected ) {
    delete tk;
    return;
  }
  if ( privgroup ) {
    KGroup *tg;

    if ((tg = kug->getGroups().lookup(tk->getName())) == 0) {
      int gid = kug->getGroups().first_free();
      uint rid = 0;
      if ( samba ) rid = kug->getGroups().first_free_sam();
      if ( gid < 0 || ( samba && rid == 0 ) ) {
        kug->getGroups().cancelMods();
        delete tk;
        return;
      }
      tg = new KGroup();
      tg->setGID(kug->getGroups().first_free());
      if ( samba ) {
        SID sid;
        sid.setDOM( kug->getGroups().getDOMSID() );
        sid.setRID( rid );
        tg->setSID( sid );
        tg->setDisplayName( tk->getName() );
      }
      tg->setName( tk->getName() );
      kug->getGroups().add( tg );
    }
    tk->setGID( tg->getGID() );
    tk->setPGSID( tg->getSID() );
  }
  kug->getUsers().add( tk );
  if ( !updateUsers() ) {
    kug->getGroups().cancelMods();
    return;
  }
  updateGroups();
}

bool mainView::queryClose()
{
  return true;
}

void mainView::slotApplySettings()
{
  kdDebug() << "settings changed!" << endl;
  lbusers->clear();
  lbgroups->clear();
  kug->init();
  reloadUsers();
  reloadGroups();
}

void mainView::setpwd() 
{
  KUser *user = lbusers->getCurrentUser();
  if ( !user ) return;
  KUser newuser( user );
  pwddlg d( this );
  if ( d.exec() == QDialog::Accepted ) {
    kug->getUsers().createPassword( &newuser, d.getPassword() );
    newuser.setLastChange( now() );
    kug->getUsers().mod( user, newuser );
    updateUsers();
  }
}

void mainView::properties() 
{
  editDefaults *eddlg = new editDefaults( kug->kcfg(), this );
  connect(eddlg, SIGNAL(settingsChanged()), this, SLOT(slotApplySettings()));

  eddlg->show();
}

void mainView::groupSelected() 
{
  bool samba = kug->getGroups().getCaps() & KGroups::Cap_Samba;
  KGroup *tmpKG = lbgroups->getCurrentGroup();
  if ( !tmpKG ) return;
  KGroup newGroup( tmpKG );

  if ( samba && newGroup.getSID().isEmpty() ) {
    SID sid;
    sid.setDOM( kug->getGroups().getDOMSID() );
    sid.setRID( kug->getGroups().first_free_sam() );
    newGroup.setSID( sid );
  }
  editGroup egdlg( &newGroup, samba, false );

  if ( egdlg.exec() == QDialog::Accepted ) {
    kug->getGroups().mod( tmpKG, newGroup );
    updateGroups();
  }
}

void mainView::userSelected() 
{
  QListViewItem *item;
  QPtrList<KUser> ulist;
  
  item = lbusers->firstChild();
  while ( item ) {
    if ( item->isSelected() ) {
      ulist.append( ((KUserViewItem*) item)->user() );
    }
    item = item->nextSibling();
  }
  if ( ulist.isEmpty() ) return;
  
  propdlg editUser( ulist, this );
  if ( editUser.exec() == QDialog::Rejected ) return;
    
  KUser *user, newuser;
  user = ulist.first();
  while ( user ) {
    editUser.mergeUser( user, &newuser );
    kug->getUsers().mod( user, newuser );
    user = ulist.next();
  }
  updateUsers();
  updateGroups();
}

void mainView::grpadd() 
{
  showPage(lbgroups);

  int gid;
  uint rid;
  bool samba;
  
  samba = kug->getGroups().getCaps() & KGroups::Cap_Samba;

  if ( (gid = kug->getGroups().first_free()) == -1 )
  {
    KMessageBox::sorry( 0, i18n("You have run out of gid space.") );
    return;
  }
  if ( samba && (rid = kug->getGroups().first_free_sam()) == 0 )
  {
    KMessageBox::sorry( 0, i18n("You have run out of group RID space.") );
    return;  
  }

  KGroup *tk = new KGroup();
  tk->setGID(gid);
  if ( samba ) {
    SID sid;
    sid.setRID( rid );
    sid.setDOM( kug->getGroups().getDOMSID() );
    tk->setSID( sid );
  }
  editGroup egdlg( tk, samba, true );
  
  if ( egdlg.exec() == QDialog::Rejected ) {
    delete tk;
    return;
  }
  kug->getGroups().add(tk);
  updateGroups();
}

void mainView::grpedit() 
{
  groupSelected();
}

void mainView::grpdel() 
{
  QListViewItem *item;
  KGroup *group;
  int selected = 0;
  
  item = lbgroups->firstChild();
  while ( item ) {
    if ( item->isSelected() ) {
    
      selected++;
      group = ((KGroupViewItem*) item)->group();
      
      KUser *user = kug->getUsers().first();
      while ( user ) {
        if ( user->getGID() == group->getGID() ) {
          KMessageBox::error( 0, i18n( "The group '%1' is the primary group of one or more users (such as '%2'); it cannot be deleted." ).arg( group->getName() ).arg( user->getName() ) );
          return;
        }
        user = kug->getUsers().next();
      }
    }
    item = item->nextSibling();
  }

  switch ( selected ) {
    case 0: return;
    case 1: 
      if (KMessageBox::warningContinueCancel( 0, 
        i18n("Do you really want to delete the group '%1'?").arg(group->getName()),
        QString::null, i18n("&Delete")) != KMessageBox::Continue) return;
      break;
    default:
      if (KMessageBox::warningContinueCancel( 0, 
        i18n("Do you really want to delete the %1 selected groups?").arg(selected),
        QString::null, i18n("&Delete")) != KMessageBox::Continue) return;
  }
  
  item = lbgroups->firstChild();
  while ( item ) {
    if ( item->isSelected() ) {
      group = ((KGroupViewItem*) item)->group();
      kug->getGroups().del( group );
    }
    item = item->nextSibling();
  }
  updateGroups();
}

bool mainView::updateGroups()
{
  bool ret;
  kdDebug() << "updateGroups() " << endl;
  ret = kug->getGroups().dbcommit();
  
  KGroup *group;
  KGroups::DelIt dit( kug->getGroups().mDelSucc );
  KGroups::AddIt ait( kug->getGroups().mAddSucc );
    
  while ( (group = dit.current()) != 0 ) {
    ++dit;
    lbgroups->removeItem( group );
  }
  while ( (group = ait.current()) != 0 ) {
    ++ait;
    lbgroups->insertItem( group );
  }
  kug->getGroups().commit();
  return ret;
}

bool mainView::updateUsers()
{
  bool ret;
  kdDebug() << "updateUsers() " << endl;
  ret = kug->getUsers().dbcommit();
  
  KUser *user;
  KUsers::DelIt dit( kug->getUsers().mDelSucc );
  KUsers::AddIt ait( kug->getUsers().mAddSucc );
    
  while ( (user = dit.current()) != 0 ) {
    ++dit;
    lbusers->removeItem( user );
  }
  while ( (user = ait.current()) != 0 ) {
    ++ait;
    lbusers->insertItem( user );
  }
  kug->getUsers().commit();
  return ret;
}
  
void mainView::slotApplyConnection()
{
  kdDebug() << "slotApplyConnection()" << endl;
  QString conn = sc->connSelected();
  kug->kcfg()->setConnection( conn );
  kug->initCfg( conn );
  slotApplySettings();
}

void mainView::selectconn()
{
  sc = new SelectConn( kug->kcfg()->connection(), this, "selectconn" );
  connect( sc, SIGNAL(applyClicked()), SLOT(slotApplyConnection()) );
  connect( sc, SIGNAL(okClicked()), SLOT(slotApplyConnection()) );
  sc->show();
}

#include "mainView.moc"
