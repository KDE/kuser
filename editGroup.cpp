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


#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qvalidator.h>
#include <qwhatsthis.h>

#include <kapplication.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include "kglobal_.h"
#include "editGroup.h"

editGroup::editGroup(KU::KGroup *akg, bool samba, bool add,
   QWidget* parent, const char* name)
  : KDialogBase(parent, name, true, i18n("Group Properties"), Ok | Cancel, Ok, true)
{
  kg = akg;
  mAdd = add;
  mSamba = samba;
  mOldName = kg->getName();
  SID sid = kg->getSID();
  ro = kug->getGroups().getCaps() & KU::KGroups::Cap_ReadOnly;

  RID rid;
  rid.rid = 512; rid.name = i18n("Domain Admins"); rid.desc = i18n("Admins"); mRids.append( rid );
  rid.rid = 513; rid.name = i18n("Domain Users"); rid.desc = i18n("Users");  mRids.append( rid );
  rid.rid = 514; rid.name = i18n("Domain Guests"); rid.desc = i18n("Guests"); mRids.append( rid );

  QFrame *page = makeMainWidget();
  QGridLayout *layout = new QGridLayout( page, 10, 3, marginHint(), spacingHint() );
  QLabel *lb;

  lb = new QLabel( page );
  lb->setText(i18n("Group number:"));
  legid = new KLineEdit(page);
  // ensure it fits at least 20 characters
  legid->setText( "XXXXXXXXXXXXXXXXXXX" );
  legid->setText( QString::number(kg->getGID()) );
  legid->setValidator( new QIntValidator(this) );
  legid->setEnabled( mAdd );
  legid->setReadOnly( ro );
  lb->setBuddy( legid );
  layout->addWidget( lb, 0, 0 );
  layout->addMultiCellWidget( legid, 0, 0, 1, 2 );

  if ( mSamba ) {
    lb = new QLabel( page );
    lb->setText(i18n("Group rid:"));
    lerid = new KComboBox( page );
    lerid->setEditable( !ro );
    QValueList<RID>::Iterator it;
    for ( it = mRids.begin(); it != mRids.end(); ++it ) {
      lerid->insertItem( QString::number( (*it).rid ) + " - " + (*it).name );
    }

    lerid->setCurrentText( QString::number( sid.getRID() ) );
    lerid->setValidator (new QIntValidator(this) );
    lerid->setEnabled( mAdd );
    connect( lerid, SIGNAL(activated(int)), SLOT(ridSelected(int)) );
    lb->setBuddy( lerid );
    layout->addWidget( lb, 1, 0 );
    layout->addMultiCellWidget( lerid, 1, 1, 1, 2 );
  }

  lb = new QLabel( page );
  lb->setText(i18n("Group name:"));

  legrpname = new KLineEdit( page );
  // ensure it fits at least 20 characters
  legrpname->setText( "XXXXXXXXXXXXXXXXXXX" );
  legrpname->setText( kg->getName() );
  legrpname->setReadOnly( ro );
  legrpname->setFocus();
  lb->setBuddy( legrpname );
  layout->addWidget( lb, 2, 0 );
  layout->addMultiCellWidget( legrpname, 2, 2, 1, 2 );

  if ( mSamba ) {
    lb = new QLabel( page );
    lb->setText(i18n("Description:"));
    ledesc = new KLineEdit(page);
    ledesc->setText( kg->getDesc() );
    ledesc->setReadOnly( ro );
    lb->setBuddy( ledesc );
    layout->addWidget( lb, 3, 0 );
    layout->addMultiCellWidget( ledesc, 3, 3, 1, 2 );

    lb = new QLabel( page );
    lb->setText(i18n("Display name:"));
    ledispname = new KLineEdit(page);
    ledispname->setText( kg->getDisplayName() );
    ledispname->setReadOnly( ro );
    lb->setBuddy( ledispname );
    layout->addWidget( lb, 4, 0 );
    layout->addMultiCellWidget( ledispname, 4, 4, 1, 2 );

    lb = new QLabel( page );
    lb->setText(i18n("Type:"));
    letype = new KComboBox( page );
    letype->insertItem( i18n("Domain") );
    letype->insertItem( i18n("Local") );
    letype->insertItem( i18n("Builtin") );
    switch ( kg->getType() ) {
      case 2:
        letype->setCurrentItem( 0 );
        break;
      case 4:
        letype->setCurrentItem( 1 );
        break;
      case 5:
        letype->setCurrentItem( 2 );
        break;
    }
    lb->setBuddy( letype );
    layout->addWidget( lb, 5, 0 );
    layout->addMultiCellWidget( letype, 5, 5, 1, 2 );

    lb = new QLabel( page );
    lb->setText(i18n("Domain SID:"));
    ledomsid = new KLineEdit(page);
    ledomsid->setText( sid.getDOM() );
    ledomsid->setReadOnly( ro );
    lb->setBuddy( ledomsid );
    layout->addWidget( lb, 6, 0 );
    layout->addMultiCellWidget( ledomsid, 6, 6, 1, 2 );

    cbsamba = new QCheckBox( i18n("Disable Samba group information"), page );
    layout->addMultiCellWidget( cbsamba, 7, 7, 0, 2 );
    connect( cbsamba, SIGNAL(toggled(bool)), ledesc, SLOT(setDisabled(bool)) );
    connect( cbsamba, SIGNAL(toggled(bool)), ledispname, SLOT(setDisabled(bool)) );
    connect( cbsamba, SIGNAL(toggled(bool)), letype, SLOT(setDisabled(bool)) );
    connect( cbsamba, SIGNAL(toggled(bool)), ledomsid, SLOT(setDisabled(bool)) );
    if ( mAdd ) connect( cbsamba, SIGNAL(toggled(bool)), lerid, SLOT(setDisabled(bool)) );
    if ( !mAdd ) cbsamba->setChecked( !( kg->getCaps() & KU::KGroup::Cap_Samba ) );
  }

  m_list_in = new KListView(page);
  m_list_in->setFullWidth(true); // Single column, full widget width.
  m_list_in->addColumn(i18n("Users in Group"));
  m_list_in->setSelectionMode( QListView::Extended );
  layout->addWidget( m_list_in, 8, 0 );

  QVBox *vbox = new QVBox(page);
  QPushButton *btadd = new QPushButton(i18n("Add <-"), vbox);
  QPushButton *btdel = new QPushButton(i18n("Remove ->"), vbox);
  layout->addWidget( vbox, 8, 1 );

  m_list_notin = new KListView(page);
  m_list_notin->setFullWidth(true); // Single column, full widget width.
  m_list_notin->addColumn(i18n("Users NOT in Group"));
  m_list_notin->setSelectionMode(QListView::Extended);
  layout->addWidget( m_list_notin, 8, 2 );
//  QString whatstr = i18n("Select the users that should be in this kg->");
//  QWhatsThis::add(m_list, whatstr);
//  connect(this,SIGNAL(okClicked(void)),
          //this,SLOT(okClicked()));


  for (unsigned int i = 0; i<kug->getUsers().count(); i++) {
    KU::KUser *user;
    user = kug->getUsers()[i];
    QString userName = user->getName();
    if ( kg->lookup_user(userName) || user->getGID() == kg->getGID() ) {
      KListViewItem *item = new KListViewItem(m_list_in, userName);
      if ( user->getGID() == kg->getGID() ) item->setSelectable( false );
    } else {
      new KListViewItem(m_list_notin, userName);
    }
  }

  connect(btadd, SIGNAL(clicked()), SLOT(addClicked()));
  connect(btdel, SIGNAL(clicked()), SLOT(delClicked()));

  if ( ro ) {
    btadd->setEnabled( false );
    btdel->setEnabled( false );
  }
}

editGroup::~editGroup() 
{
}

void editGroup::ridSelected( int index )
{
  lerid->setCurrentText( QString::number( mRids[ index ].rid ) );
  legrpname->setText( mRids[ index ].name );
  ledesc->setText( mRids[ index ].desc );
  ledispname->setText( mRids[ index ].name );
}

void editGroup::addClicked()
{
  QListViewItem *item, *next;
  QString name;

  item = m_list_notin->firstChild();
  while ( item ) {
    next = item->nextSibling();
    if ( item->isSelected() ) {
      name = item->text( 0 );
      delete item;
      item = new KListViewItem( m_list_in, name );
    }
    item = next;
  }
}

void editGroup::delClicked()
{
  QListViewItem *item, *next;
  QString name;

  item = m_list_in->firstChild();
  while ( item ) {
    next = item->nextSibling();
    if ( item->isSelected() ) {
      name = item->text( 0 );
      delete item;
      item = new KListViewItem( m_list_notin, name );
    }
    item = next;
  }
}

void editGroup::slotOk()
{
  if ( ro ) {
    reject();
    return;
  }

  SID sid;
  kg->clear();
  QString s;
  s = legid->text();

  if ( mSamba && !cbsamba->isChecked() ) {
    sid.setDOM( ledomsid->text() );
    sid.setRID( lerid->currentText() );
  }

  if ( legrpname->text().isEmpty() ) {
    KMessageBox::sorry( 0,
      i18n("You need to type a group name.") );
    return;
  }

  if ( legrpname->text() != mOldName && 
    kug->getGroups().lookup( legrpname->text() ) ) {

    KMessageBox::sorry( 0,
      i18n("Group with name %1 already exists.").arg(legrpname->text()) );
    return;
  }

  if ( mAdd ) {
    if ( mSamba && !cbsamba->isChecked() && kug->getGroups().lookup_sam( sid ) ) {
      KMessageBox::sorry( 0,
        i18n("Group with SID %1 already exists.").arg( sid.getSID() ) );
      return;
    }
    if (kug->getGroups().lookup(s.toInt())) {
      KMessageBox::sorry( 0,
        i18n("Group with gid %1 already exists.").arg(s.toInt()) );
      return;
    }
  }

  kg->setName(legrpname->text());
  kg->setGID(s.toInt());
  if ( mSamba && !cbsamba->isChecked() ) {
    kg->setCaps ( KU::KGroup::Cap_Samba );
    kg->setSID( sid );
    switch ( letype->currentItem() ) {
      case 0:
        kg->setType( 2 );
        break;
      case 1:
        kg->setType( 4 );
        break;
      case 2:
        kg->setType( 5 );
        break;
    }
    kg->setDesc( ledesc->text() );
    kg->setDisplayName( ledispname->text() );
  } else {
    kg->setCaps( 0 );
    kg->setSID( QString::null );
    kg->setDesc( QString::null );
    kg->setDisplayName( QString::null );
    kg->setType( 0 );
  }

  QListViewItem *item;
  item = m_list_in->firstChild();
  while ( item ) {
    kg->addUser( item->text( 0 ) );
    item = item->nextSibling();
  }
  accept();
}

#include "editGroup.moc"
