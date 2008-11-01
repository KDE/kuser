/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
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

#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

#include <klocale.h>
#include <kpushbutton.h>
#include <kicon.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include "ku_global.h"
#include "ku_editgroup.h"

KU_EditGroup::KU_EditGroup(const KU_Group &group, bool add,
   QWidget* parent)
  : KDialog(parent)
{
    setCaption( i18n("Group Properties") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );

  mAdd = add;
  mOldName = group.getName();
  SID sid = group.getSID();
  ro = KU_Global::groups()->getCaps() & KU_Groups::Cap_ReadOnly;
  mSamba = KU_Global::groups()->getCaps() & KU_Groups::Cap_Samba;

  RID rid;
  rid.rid = 512; rid.name = i18n("Domain Admins"); rid.desc = i18n("Admins"); mRids.append( rid );
  rid.rid = 513; rid.name = i18n("Domain Users"); rid.desc = i18n("Users");  mRids.append( rid );
  rid.rid = 514; rid.name = i18n("Domain Guests"); rid.desc = i18n("Guests"); mRids.append( rid );

  QFrame *page = new QFrame( this );
  QGridLayout *layout = new QGridLayout( page );
  layout->setSpacing( spacingHint() );
  layout->setMargin( marginHint() );

  QLabel *lb;
  setMainWidget( page );

  lb = new QLabel( page );
  lb->setText(i18n("Group number:"));
  legid = new KLineEdit(page);
  // ensure it fits at least 20 characters
  legid->setText( "XXXXXXXXXXXXXXXXXXX" );
  kDebug() << "gid: " << group.getGID();
  legid->setText( QString::number(group.getGID()) );
  legid->setValidator( new QIntValidator(this) );
  legid->setEnabled( mAdd );
  legid->setReadOnly( ro );
  lb->setBuddy( legid );
  layout->addWidget( lb, 0, 0 );
  layout->addWidget( legid, 0, 1, 1, 2 );

  if ( mSamba ) {
    lb = new QLabel( page );
    lb->setText(i18n("Group rid:"));
    lerid = new KComboBox( page );
    lerid->setEditable( !ro );
    QList<RID>::Iterator it;
    for ( it = mRids.begin(); it != mRids.end(); ++it ) {
      lerid->addItem( QString::number( (*it).rid ) + " - " + (*it).name );
    }

    lerid->lineEdit()->setText( QString::number( sid.getRID() ) );
    lerid->setValidator (new QIntValidator(this) );
    lerid->setEnabled( mAdd );
    connect( lerid, SIGNAL(activated(int)), SLOT(ridSelected(int)) );
    lb->setBuddy( lerid );
    layout->addWidget( lb, 1, 0 );
    layout->addWidget( lerid, 1, 1, 1, 2 );
  }

  lb = new QLabel( page );
  lb->setText(i18n("Group name:"));

  legrpname = new KLineEdit( page );
  // ensure it fits at least 20 characters
  legrpname->setText( "XXXXXXXXXXXXXXXXXXX" );
  legrpname->setText( group.getName() );
  legrpname->setReadOnly( ro );
  legrpname->setFocus();
  lb->setBuddy( legrpname );
  layout->addWidget( lb, 2, 0 );
  layout->addWidget( legrpname, 2, 1, 1, 2 );

  if ( mSamba ) {
    lb = new QLabel( page );
    lb->setText(i18n("Description:"));
    ledesc = new KLineEdit(page);
    ledesc->setText( group.getDesc() );
    ledesc->setReadOnly( ro );
    lb->setBuddy( ledesc );
    layout->addWidget( lb, 3, 0 );
    layout->addWidget( ledesc, 3, 1, 1, 2 );

    lb = new QLabel( page );
    lb->setText(i18n("Display name:"));
    ledispname = new KLineEdit(page);
    ledispname->setText( group.getDisplayName() );
    ledispname->setReadOnly( ro );
    lb->setBuddy( ledispname );
    layout->addWidget( lb, 4, 0 );
    layout->addWidget( ledispname, 4, 1, 1, 2 );

    lb = new QLabel( page );
    lb->setText(i18n("Type:"));
    letype = new KComboBox( page );
    letype->addItem( i18n("Domain") );
    letype->addItem( i18n("Local") );
    letype->addItem( i18n("Builtin") );
    switch ( group.getType() ) {
      case 2:
        letype->setCurrentIndex( 0 );
        break;
      case 4:
        letype->setCurrentIndex( 1 );
        break;
      case 5:
        letype->setCurrentIndex( 2 );
        break;
    }
    lb->setBuddy( letype );
    layout->addWidget( lb, 5, 0 );
    layout->addWidget( letype, 5, 1, 1, 2 );

    lb = new QLabel( page );
    lb->setText(i18n("Domain SID:"));
    ledomsid = new KLineEdit(page);
    ledomsid->setText( sid.getDOM() );
    ledomsid->setReadOnly( ro );
    lb->setBuddy( ledomsid );
    layout->addWidget( lb, 6, 0 );
    layout->addWidget( ledomsid, 6, 1, 1, 2 );

    cbsamba = new QCheckBox( i18n("Disable Samba group information"), page );
    layout->addWidget( cbsamba, 7, 0, 1, 3 );
    connect( cbsamba, SIGNAL(toggled(bool)), ledesc, SLOT(setDisabled(bool)) );
    connect( cbsamba, SIGNAL(toggled(bool)), ledispname, SLOT(setDisabled(bool)) );
    connect( cbsamba, SIGNAL(toggled(bool)), letype, SLOT(setDisabled(bool)) );
    connect( cbsamba, SIGNAL(toggled(bool)), ledomsid, SLOT(setDisabled(bool)) );
    if ( mAdd ) connect( cbsamba, SIGNAL(toggled(bool)), lerid, SLOT(setDisabled(bool)) );
    if ( !mAdd ) cbsamba->setChecked( !( group.getCaps() & KU_Group::Cap_Samba ) );
  }

  QStringList sl;

  m_list_in = new QTreeWidget(page);
  m_list_in->setColumnCount( 1 );
  sl.append(i18n("Users in Group"));
  m_list_in->setHeaderLabels(sl);
  m_list_in->setSelectionMode( QAbstractItemView::ExtendedSelection );
  m_list_in->setSortingEnabled( true );
  layout->addWidget( m_list_in, 8, 0 );

  QWidget *vbox = new QWidget;
  QPushButton *btadd = new QPushButton(KIcon("list-add"), i18n("Add"), vbox);
  QPushButton *btdel = new QPushButton(KIcon("list-remove"), i18n("Remove"), vbox);

  QVBoxLayout *vlayout = new QVBoxLayout;
  vlayout->addWidget(btadd);
  vlayout->addWidget(btdel);
  vbox->setLayout(vlayout);

  layout->addWidget( vbox, 8, 1 );

  sl.clear();
  m_list_notin = new QTreeWidget( page );
  m_list_notin->setColumnCount( 1 );
  sl.append(i18n("Users NOT in Group"));
  m_list_notin->setHeaderLabels(sl);
  m_list_notin->setSelectionMode( QAbstractItemView::ExtendedSelection );
  m_list_notin->setSortingEnabled( true );
  layout->addWidget( m_list_notin, 8, 2 );

  for ( int i = 0; i<KU_Global::users()->count(); i++ ) {
    KU_User user;
    user = KU_Global::users()->at(i);
    QString userName = user.getName();
    sl.clear();
    sl.append( userName );
    if ( group.lookup_user(userName) || user.getGID() == group.getGID() ) {
      QTreeWidgetItem *item = new QTreeWidgetItem(m_list_in, sl);
      if ( user.getGID() == group.getGID() )
        item->setFlags( Qt::ItemIsEnabled );
      else
        item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
    } else {
      QTreeWidgetItem *item = new QTreeWidgetItem(m_list_notin, sl);
      item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
    }
  }

  connect(btadd, SIGNAL(clicked()), SLOT(addClicked()));
  connect(btdel, SIGNAL(clicked()), SLOT(delClicked()));

  if ( ro ) {
    btadd->setEnabled( false );
    btdel->setEnabled( false );
  }
}

KU_EditGroup::~KU_EditGroup()
{
}

void KU_EditGroup::ridSelected( int index )
{
  lerid->lineEdit()->setText( QString::number( mRids[ index ].rid ) );
  legrpname->setText( mRids[ index ].name );
  ledesc->setText( mRids[ index ].desc );
  ledispname->setText( mRids[ index ].name );
}

void KU_EditGroup::addClicked()
{
  int i = 0;
  while ( i < m_list_notin->topLevelItemCount() ) {
    QTreeWidgetItem *item = m_list_notin->topLevelItem( i );
    if ( m_list_notin->isItemSelected( item ) ) {
      item = m_list_notin->takeTopLevelItem( i );
      m_list_in->insertTopLevelItem( m_list_in->topLevelItemCount(), item );
    } else {
      i++;
    }
  }
}

void KU_EditGroup::delClicked()
{
  int i = 0;
  while ( i < m_list_in->topLevelItemCount() ) {
    QTreeWidgetItem *item = m_list_in->topLevelItem( i );
    if ( m_list_in->isItemSelected( item ) ) {
      item = m_list_in->takeTopLevelItem( i );
      m_list_notin->insertTopLevelItem( m_list_notin->topLevelItemCount(), item );
    } else {
      i++;
    }
  }
}

void KU_EditGroup::accept()
{
  if ( ro ) {
    reject();
    return;
  }

  SID sid;
  gid_t gid = legid->text().toInt();

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
    KU_Global::groups()->lookup( legrpname->text() ) != -1 ) {

    KMessageBox::sorry( 0,
      i18n("Group with name %1 already exists.", legrpname->text()) );
    return;
  }

  if ( mAdd ) {
    if ( mSamba && !cbsamba->isChecked() && KU_Global::groups()->lookup_sam( sid ) != -1 ) {
      KMessageBox::sorry( 0,
        i18n("Group with SID %1 already exists.", sid.getSID() ) );
      return;
    }
    if ( KU_Global::groups()->lookup(gid) != -1 ) {
      KMessageBox::sorry( 0,
        i18n("Group with gid %1 already exists.", gid) );
      return;
    }
  }

  mGroup.setName( legrpname->text() );
  mGroup.setGID( gid );
  if ( mSamba && !cbsamba->isChecked() ) {
    mGroup.setCaps ( KU_Group::Cap_Samba );
    mGroup.setSID( sid );
    switch ( letype->currentIndex() ) {
      case 0:
        mGroup.setType( 2 );
        break;
      case 1:
        mGroup.setType( 4 );
        break;
      case 2:
        mGroup.setType( 5 );
        break;
    }
    mGroup.setDesc( ledesc->text() );
    mGroup.setDisplayName( ledispname->text() );
  } else {
    mGroup.setCaps( 0 );
    mGroup.setSID( SID( QString::null ) );	//krazy:exclude=nullstrassign for old broken gcc
    mGroup.setDesc( QString::null );		//krazy:exclude=nullstrassign for old broken gcc
    mGroup.setDisplayName( QString::null );	//krazy:exclude=nullstrassign for old broken gcc
    mGroup.setType( 0 );
  }

  for( int i = 0; i < m_list_in->topLevelItemCount(); i++ ) {
    QTreeWidgetItem *item = m_list_in->topLevelItem( i );
    mGroup.addUser( item->text( 0 ) );
  }
  done( Accepted );
}

#include "ku_editgroup.moc"
