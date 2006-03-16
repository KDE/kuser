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

#include <stdio.h>
#include <stdlib.h>

#include <qdatetime.h>
#include <qwhatsthis.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qfile.h>

#include <kseparator.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include <qvalidator.h>

#include "propdlg.h"
#include "pwddlg.h"
#include "kglobal_.h"
#include "misc.h"

void propdlg::addRow(QWidget *parent, QGridLayout *layout, int row,
  QWidget *widget, const QString &label, const QString &what,
  bool two_column, bool nochange)
{
   QLabel *lab = new QLabel(widget, label, parent);
   lab->setMinimumSize(lab->sizeHint());
   widget->setMinimumSize(widget->sizeHint());
   layout->addWidget(lab, row, 0);
   if (!what.isEmpty())
   {
      QWhatsThis::add(lab, what);
      QWhatsThis::add(widget, what);
   }
   if (two_column)
      layout->addMultiCellWidget(widget, row, row, 1, 2);
   else
      layout->addWidget(widget, row, 1);

   if ( !nochange || ro ) return;
   QCheckBox *nc = new QCheckBox( i18n("Do not change"), parent );
   layout->addWidget( nc, row, 3 );
   nc->hide();
   mNoChanges[ widget ] = nc;
}

KIntSpinBox *propdlg::addDaysGroup(QWidget *parent, QGridLayout *layout, int row,
  const QString &title, bool never)
{
    KIntSpinBox *days;

    QLabel *label = new QLabel( title, parent );
    layout->addMultiCellWidget( label, row, row, 0, 1, AlignRight );

    days = new KIntSpinBox( parent );
    label->setBuddy( days );
    days->setSuffix( i18n(" days") );
    days->setMaxValue( 99999 );
    if (never)
    {
      days->setMinValue( -1 );
      days->setSpecialValueText(i18n("Never"));
    }
    else
    {
      days->setMinValue( 0 );
    }
    layout->addWidget( days, row, 2 );

    connect(days, SIGNAL(valueChanged(int)), this, SLOT(changed()));

    QCheckBox *nc = new QCheckBox( i18n("Do not change"), parent );
    layout->addWidget( nc, row, 3 );
    nc->hide();
    mNoChanges[ days ] = nc;

    return days;
}

void propdlg::initDlg()
{
  ro = kug->getUsers().getCaps() & KU::KUsers::Cap_ReadOnly;

  QString whatstr;

  // Tab 1: User Info
  {
    QFrame *frame = addPage(i18n("User Info"));
    QGridLayout *layout = new QGridLayout(frame, 20, 4, marginHint(), spacingHint());
    int row = 0;

    frontpage = frame;
    frontlayout = layout;

    lbuser = new QLabel(frame);
//    whatstr = i18n("WHAT IS THIS: User login");
    addRow(frame, layout, row++, lbuser, i18n("User login:"), whatstr, false, false);

    leid = new KLineEdit(frame);
//    whatstr = i18n("WHAT IS THIS: User Id");
    leid->setValidator(new QIntValidator(frame));
    addRow(frame, layout, row++, leid, i18n("&User ID:"), whatstr);
    connect(leid, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));

    if ( !ro ) {
      pbsetpwd = new QPushButton(i18n("Set &Password..."), frame);
      layout->addWidget(pbsetpwd, 0, 2);
      connect(pbsetpwd, SIGNAL(clicked()), this, SLOT(setpwd()));
    }


    lefname = new KLineEdit(frame);
//    whatstr = i18n("WHAT IS THIS: Full Name");
    addRow(frame, layout, row++, lefname, i18n("Full &name:"), whatstr);
    connect(lefname, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
    lefname->setFocus();

    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_InetOrg ) {
      lesurname = new KLineEdit(frame);
//    whatstr = i18n("WHAT IS THIS: Surname");
      addRow(frame, layout, row++, lesurname, i18n("Surname:"), whatstr);
      connect(lesurname, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));

      lemail = new KLineEdit(frame);
//    whatstr = i18n("WHAT IS THIS: Email");
      addRow(frame, layout, row++, lemail, i18n("Email address:"), whatstr);
      connect(lemail, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
    }

    leshell = new KComboBox(true, frame);
    leshell->clear();
    leshell->insertItem(i18n("<Empty>"));

    QStringList shells = readShells();
    shells.sort();
    leshell->insertStringList(shells);
    connect(leshell, SIGNAL(activated(const QString &)), this, SLOT(changed()));
    connect(leshell, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Login Shell");
    addRow(frame, layout, row++, leshell, i18n("&Login shell:"), whatstr);

    lehome = new KLineEdit(frame);
    connect(lehome, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Home Directory");
    addRow(frame, layout, row++, lehome, i18n("&Home folder:"), whatstr);

    // FreeBSD appears to use the comma separated fields in the GECOS entry
    // differently than Linux.
    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_BSD ) {
      leoffice = new KLineEdit(frame);
      connect(leoffice, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Office");
      addRow(frame, layout, row++, leoffice, i18n("&Office:"), whatstr);

      leophone = new KLineEdit(frame);
      connect(leophone, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Office Phone");
      addRow(frame, layout, row++, leophone, i18n("Offi&ce Phone:"), whatstr);

      lehphone = new KLineEdit(frame);
      connect(lehphone, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Home Phone");
      addRow(frame, layout, row++, lehphone, i18n("Ho&me Phone:"), whatstr);

      leclass = new KLineEdit(frame);
      connect(leclass, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Login class");
      addRow(frame, layout, row++, leclass, i18n("Login class:"), whatstr, true);
    } else {
      leoffice1 = new KLineEdit(frame);
      connect(leoffice1, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Office1");
      addRow(frame, layout, row++, leoffice1, i18n("&Office #1:"), whatstr);

      leoffice2 = new KLineEdit(frame);
      connect(leoffice2, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Office2");
      addRow(frame, layout, row++, leoffice2, i18n("O&ffice #2:"), whatstr);

      leaddress = new KLineEdit(frame);
      connect(leaddress, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Address");
      addRow(frame, layout, row++, leaddress, i18n("&Address:"), whatstr);
    }
    cbdisabled = new QCheckBox(frame);
    connect(cbdisabled, SIGNAL(stateChanged(int)), this, SLOT(changed()));
    addRow(frame, layout, row++, cbdisabled, i18n("Account &disabled"), whatstr);

    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Disable_POSIX ) {
      cbposix = new QCheckBox(frame);
      connect(cbposix, SIGNAL(stateChanged(int)), this, SLOT(changed()));
      connect(cbposix, SIGNAL(stateChanged(int)), this, SLOT(cbposixChanged()));
      addRow(frame, layout, row++, cbposix, i18n("Disable &POSIX account information"), whatstr);
    } else {
      cbposix = 0;
    }
    frontrow = row;
  }

  if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Shadow ||
       kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ||
       kug->getUsers().getCaps() & KU::KUsers::Cap_BSD ) {

  // Tab 2 : Password Management
    QFrame *frame = addPage(i18n("Password Management"));
    QGridLayout *layout = new QGridLayout(frame, 20, 4, marginHint(), spacingHint());
    int row = 0;

    QDateTime time;
    leslstchg = new QLabel(frame);
    addRow(frame, layout, row++, leslstchg, i18n("Last password change:"), QString::null, true);

    layout->addMultiCellWidget(new KSeparator(KSeparator::HLine, frame), row, row, 0, 3);
    row++;

    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Shadow ) {
      layout->addWidget( new QLabel( i18n("POSIX parameters:"), frame ), row++, 0 );
      lesmin = addDaysGroup(frame, layout, row++, i18n("Time before password may &not be changed after last password change:"), false);
      lesmax = addDaysGroup(frame, layout, row++, i18n("Time when password &expires after last password change:") );
      leswarn = addDaysGroup(frame, layout, row++, i18n("Time before password expires to &issue an expire warning:"));
      lesinact = addDaysGroup(frame, layout, row++, i18n("Time when account will be &disabled after expiration of password:"));
      layout->addMultiCellWidget(new KSeparator(KSeparator::HLine, frame), row, row, 0, 3);
      row++;
    }
    /*
    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ) {
      layout->addWidget( new QLabel( "SAMBA parameters:", frame ), row++, 0 );
      layout->addMultiCellWidget(new KSeparator(KSeparator::HLine, frame), row, row, 0, 3);
      row++;
    }
    */
    QLabel *label = new QLabel( i18n("&Account will expire on:"), frame );
    layout->addWidget( label, row, 0 );
    lesexpire = new KDateTimeWidget( frame );
    label->setBuddy( lesexpire );
    layout->addMultiCellWidget( lesexpire, row, row, 1, 2);

    cbexpire = new QCheckBox( i18n("Never"), frame );
    layout->addWidget( cbexpire, row++, 3 );

    connect( lesexpire, SIGNAL(valueChanged(const QDateTime&)), this, SLOT(changed()) );
    connect( cbexpire, SIGNAL(stateChanged(int)), this, SLOT(changed()) );
    connect( cbexpire, SIGNAL(toggled(bool)), lesexpire, SLOT(setDisabled(bool)) );
  }

  // Tab 3: Samba
  if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ) {
    QFrame *frame = addPage(i18n("Samba"));
    QGridLayout *layout = new QGridLayout(frame, 10, 4, marginHint(), spacingHint());
    int row = 0;

    lerid = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Rid");
    lerid->setValidator(new QIntValidator(frame));
    addRow(frame, layout, row++, lerid, i18n("RID:"), whatstr);
    connect(lerid, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));

    leliscript = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, leliscript, i18n("Login script:"), whatstr);
    connect(leliscript, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));

    leprofile = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, leprofile, i18n("Profile path:"), whatstr);
    connect(leprofile, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));

    lehomedrive = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, lehomedrive, i18n("Home drive:"), whatstr);
    connect(lehomedrive, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));

    lehomepath = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, lehomepath, i18n("Home path:"), whatstr);
    connect(lehomepath, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));

    leworkstations = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, leworkstations, i18n("User workstations:"), whatstr);
    connect(leworkstations, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));

    ledomain = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, ledomain, i18n("Domain name:"), whatstr);
    connect(ledomain, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));

    ledomsid = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, ledomsid, i18n("Domain SID:"), whatstr);
    connect(ledomsid, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));

    cbsamba = new QCheckBox(frame);
    connect(cbsamba, SIGNAL(stateChanged(int)), this, SLOT(changed()));
    connect(cbsamba, SIGNAL(stateChanged(int)), this, SLOT(cbsambaChanged()));
    addRow(frame, layout, row++, cbsamba, i18n("Disable &Samba account information"), whatstr);
  }

  // Tab 4: Groups
  {
    QFrame *frame = addPage(i18n("Groups"));
    QGridLayout *layout = new QGridLayout(frame, 2, 2, marginHint(), spacingHint());

    lstgrp = new KListView(frame);
    lstgrp->setFullWidth(true); // Single column, full widget width.
    lstgrp->addColumn( i18n("Groups") );
    if ( ro ) lstgrp->setSelectionMode( QListView::NoSelection );
//    QString whatstr = i18n("Select the groups that this user belongs to.");
    QWhatsThis::add(lstgrp, whatstr);
    layout->addMultiCellWidget(lstgrp, 0, 0, 0, 1);
    leprigr = new QLabel( i18n("Primary group: "), frame );
    layout->addWidget( leprigr, 1, 0 );
    if ( !ro ) {
      pbprigr = new QPushButton( i18n("Set as Primary"), frame );
      layout->addWidget( pbprigr, 1, 1 );
      connect( pbprigr, SIGNAL(clicked()), this, SLOT(setpgroup()) );
    }
    connect( lstgrp, SIGNAL(clicked(QListViewItem *)), this, SLOT(gchanged()) );
  }

}

propdlg::propdlg( const QPtrList<KU::KUser> &users,
  QWidget *parent, const char *name ) :
  KDialogBase(Tabbed, i18n("User Properties"), Ok | Cancel, Ok, parent, name, true)

{
  mUsers = users;
  if ( mUsers.getFirst() != mUsers.getLast() )
    setCaption( i18n("User Properties - %1 Selected Users").arg( mUsers.count() ) );
  initDlg();
  loadgroups( false );
  selectuser();
  ischanged = false;
  isgchanged = false;
}

propdlg::propdlg( KU::KUser *AUser, bool fixedprivgroup,
  QWidget *parent, const char *name ) :
  KDialogBase(Tabbed, i18n("User Properties"), Ok | Cancel, Ok, parent, name, true)

{
  mUsers.append( AUser );
  initDlg();
  loadgroups( fixedprivgroup );
  selectuser();
  ischanged = false;
  isgchanged = false;
}

propdlg::~propdlg()
{
}

void propdlg::cbposixChanged()
{
  bool posix = !( cbposix->state() == QButton::On );
  leid->setEnabled( posix  & ( mUsers.getFirst() == mUsers.getLast() ) );
  leshell->setEnabled( posix );
  lehome->setEnabled( posix );
  if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Shadow ) {
    lesmin->setEnabled( posix );
    lesmax->setEnabled( posix );
    leswarn->setEnabled( posix );
    lesinact->setEnabled( posix );
  }
}

void propdlg::cbsambaChanged()
{
  bool samba = !( cbsamba->state() == QButton::On );
  lerid->setEnabled( samba & ( mUsers.getFirst() == mUsers.getLast() ) );
  leliscript->setEnabled( samba );
  leprofile->setEnabled( samba );
  lehomedrive->setEnabled( samba );
  lehomepath->setEnabled( samba );
  leworkstations->setEnabled( samba );
  ledomain->setEnabled( samba );
  ledomsid->setEnabled( samba );
}

void propdlg::setLE( KLineEdit *le, const QString &val, bool first )
{
  if ( first ) {
    le->setText( val );
    if ( ro ) le->setReadOnly( true );
    return;
  }
  if ( val.isEmpty() && le->text().isEmpty() ) return;
  if ( le->text() != val ) {
    le->setText( "" );
    if ( !ro && mNoChanges.contains( le ) ) {
      mNoChanges[ le ]->show();
      mNoChanges[ le ]->setChecked( true );
    }
  }
}

void propdlg::setCB( QCheckBox *cb, bool val, bool first )
{
  if ( first ) {
    cb->setChecked( val );
    if ( ro ) cb->setEnabled( false );
    return;
  }
  if ( cb->isChecked() != val ) {
    cb->setTristate();
    cb->setNoChange();
  }
}

void propdlg::setSB( KIntSpinBox *sb, int val, bool first )
{
  if ( first ) {
    sb->setValue( val );
    if ( ro ) sb->setEnabled( false );
    return;
  }
  if ( sb->value() != val ) {
    sb->setValue( 0 );
    if ( !ro && mNoChanges.contains( sb ) ) {
      mNoChanges[ sb ]->show();
      mNoChanges[ sb ]->setChecked( true );
    }
  }
}

void propdlg::selectuser()
{
  KU::KUser *user;
  bool first = true, one = ( mUsers.getFirst() == mUsers.getLast() );

  ismoreshells = false;
  user = mUsers.first();
  olduid = user->getUID();
  oldrid = user->getSID().getRID();
  oldshell = user->getShell();
  lstchg = user->getLastChange();
  QDateTime datetime;
  datetime.setTime_t( lstchg );
  if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Shadow ||
       kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ||
       kug->getUsers().getCaps() & KU::KUsers::Cap_BSD ) {

    leslstchg->setText( KGlobal::locale()->formatDateTime( datetime, false ) );
  }

  if ( one ) {
    lbuser->setText( user->getName() );
    leid->setText( QString::number( user->getUID() ) );
    if ( ro ) leid->setReadOnly( true );
    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ) {
      lerid->setText( QString::number( user->getSID().getRID() ) );
      if ( ro ) lerid->setReadOnly( true );
    }
  } else {
    leid->setEnabled( false );
    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ) {
      lerid->setEnabled( false );
    }
  }
  if ( ro ) leshell->setEditable( false );

  while ( user ) {

    setLE( lefname, user->getFullName(), first );
    QString home;
    home = user->getHomeDir();
    if ( !one ) home.replace( user->getName(), "%U" );
    setLE( lehome, home, first );

    QString shell = user->getShell();
    if ( first ) {
      if ( !shell.isEmpty() ) {
        bool tested = false;
        for ( int i=0; i<leshell->count(); i++ )
          if ( leshell->text(i) == shell ) {
            tested = true;
            leshell->setCurrentItem(i);
            break;
          }
          if ( !tested ) {
            leshell->insertItem( shell );
            leshell->setCurrentItem( leshell->count()-1 );
          }
      } else
        leshell->setCurrentItem(0);
    } else {
      if ( leshell->currentText() != shell ) {
        if ( !ismoreshells ) {
          leshell->insertItem( i18n("Do Not Change"), 0 );
          ismoreshells = true;
        }
        leshell->setCurrentItem( 0 );
      }
    }

    setCB( cbdisabled, user->getDisabled(), first );
    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Disable_POSIX ) {
      setCB( cbposix, !(user->getCaps() & KU::KUser::Cap_POSIX), first );
    }

    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ) {
      setLE( leliscript, user->getLoginScript(), first );
      QString profile;
      profile = user->getProfilePath();
      if ( !one ) profile.replace( user->getName(), "%U" );
      setLE( leprofile, profile, first );
      setLE( lehomedrive, user->getHomeDrive(), first );
      home = user->getHomePath();
      if ( !one ) home.replace( user->getName(), "%U" );
      setLE( lehomepath, home, first );
      setLE( leworkstations, user->getWorkstations(), first );
      setLE( ledomain, user->getDomain(), first );
      setLE( ledomsid, user->getSID().getDOM(), first );
      setCB( cbsamba, !(user->getCaps() & KU::KUser::Cap_Samba), first );
    }

    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Shadow ||
         kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ||
         kug->getUsers().getCaps() & KU::KUsers::Cap_BSD ) {

      if ( user->getLastChange() != lstchg ) {
        leslstchg->setText( "" );
        lstchg = 0;
      }

      QDateTime expire;
      expire.setTime_t( user->getExpire() );
      kdDebug() << "expiration: " << user->getExpire() << endl;
      setCB( cbexpire, (int) expire.toTime_t() == -1, first );
      if ( (int) expire.toTime_t() == -1 ) expire.setTime_t( 0 );
      if ( first ) {
        lesexpire->setDateTime( expire );
      } else {
        if ( lesexpire->dateTime() != expire ) {
          expire.setTime_t( 0 );
          lesexpire->setDateTime( expire );
        }
      }
    }

    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Shadow ) {
      setSB( lesmin, user->getMin(), first );
      setSB( lesmax, user->getMax(), first );
      setSB( leswarn, user->getWarn(), first );
      setSB( lesinact, user->getInactive(), first );
    }

    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_InetOrg ) {
      setLE( lesurname, user->getSurname(), first );
      setLE( lemail, user->getEmail(), first );
    }
    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_BSD ) {
      setLE( leoffice, user->getOffice(), first );
      setLE( leophone, user->getWorkPhone(), first );
      setLE( lehphone, user->getHomePhone(), first );
      setLE( leclass, user->getClass(), first );
    } else {
      setLE( leoffice1, user->getOffice1(), first );
      setLE( leoffice2, user->getOffice2(), first );
      setLE( leaddress, user->getAddress(), first );
    }

    first = false;
    user = mUsers.next();
  }
}

void propdlg::loadgroups( bool fixedprivgroup )
{
  bool wasprivgr = false;

  primaryGroupWasOn = false;

  KU::KGroup *group = kug->getGroups().first();
  while ( group ) {
    QString groupName = group->getName();
    QCheckListItem *item = new QCheckListItem(lstgrp, groupName, QCheckListItem::CheckBox);
    KU::KUser *user = mUsers.first();
    while ( user ) {
      bool prigr =
        ( !fixedprivgroup && group->getGID() == user->getGID() ) ||
        ( fixedprivgroup && groupName == user->getName() );
      bool on = group->lookup_user( user->getName() ) || prigr;

      if ( prigr ) {
        item->setEnabled( false );
        if ( !wasprivgr )
          primaryGroup = groupName;
        else
          if ( primaryGroup != groupName ) primaryGroup = "";
//      primaryGroupWasOn = group->lookup_user(user->getName());
        wasprivgr = true;
      }

      if ( mUsers.getFirst() == user )
        item->setOn( on );
      else
        if ( item->isOn() != on ) {
          item->setTristate( true );
          item->setState( QCheckListItem::NoChange );
        }
      user = mUsers.next();
    }
    group = kug->getGroups().next();
  }

  if ( fixedprivgroup ) {
    KU::KUser *user = mUsers.first();
    kdDebug() << "privgroup: " << user->getName() << endl;
    if ( !wasprivgr ) {
      QCheckListItem *item = new QCheckListItem(lstgrp, user->getName(), QCheckListItem::CheckBox);
      item->setOn(true);
      item->setEnabled(false);
      primaryGroup = user->getName();
    }
  }
  leprigr->setText( i18n("Primary group: ") + primaryGroup );
}

void propdlg::setpgroup()
{
  isgchanged = true;
  QCheckListItem *item;
  item = (QCheckListItem *) lstgrp->selectedItem();
  if ( item == 0 || item->text() == primaryGroup )
     return;

  bool prevPrimaryGroupWasOn = primaryGroupWasOn;
  primaryGroup = ((QCheckListItem *) lstgrp->selectedItem())->text();

  item = (QCheckListItem *) lstgrp->firstChild();

  while(item)
  {
     QString groupName = item->text();
     if ( !item->isEnabled() )
     {
        item->setEnabled(true);
        item->setOn(prevPrimaryGroupWasOn);
        item->repaint();
     }
     if ( groupName == primaryGroup )
     {
        primaryGroupWasOn = item->isOn();
        item->setEnabled(false);
        item->setOn(true);
        item->repaint();
     }

     item = (QCheckListItem *) item->nextSibling();
  }
  leprigr->setText( i18n("Primary group: ") + primaryGroup );
}

void propdlg::changed()
{
  QWidget *widget = (QWidget*) sender();
  if ( mNoChanges.contains( widget ) ) mNoChanges[ widget ]->setChecked( false );
  ischanged = true;
  kdDebug() << "changed" << endl;
}

void propdlg::gchanged()
{
  isgchanged = true;
}

QString propdlg::mergeLE( KLineEdit *le, const QString &val, bool one )
{
  QCheckBox *cb = 0;
  if ( mNoChanges.contains( le ) ) cb = mNoChanges[ le ];
  return ( one || ( cb && !cb->isChecked() ) ) ? le->text() : val;
}

int propdlg::mergeSB( KIntSpinBox *sb, int val, bool one )
{
  QCheckBox *cb = 0;
  if ( mNoChanges.contains( sb ) ) cb = mNoChanges[ sb ];
  return ( one || ( cb && !cb->isChecked() ) ) ? sb->value() : val;
}

void propdlg::mergeUser( KU::KUser *user, KU::KUser *newuser )
{
  QDateTime epoch ;
  epoch.setTime_t(0);
  bool one = ( mUsers.getFirst() == mUsers.getLast() );
  bool posix, samba = false;

  newuser->copy( user );

  if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Disable_POSIX && cbposix->state() != QButton::NoChange ) {
    if ( cbposix->isChecked() )
      newuser->setCaps( newuser->getCaps() & ~KU::KUser::Cap_POSIX );
    else
      newuser->setCaps( newuser->getCaps() | KU::KUser::Cap_POSIX );
  }
  posix = newuser->getCaps() & KU::KUser::Cap_POSIX;
  kdDebug() << "posix: " << posix << endl;
  if ( one ) {
//    newuser->setName( leuser->text() );
    newuser->setUID( posix ? leid->text().toInt() : 0 );
  }
  if ( !newpass.isNull() ) {
    kug->getUsers().createPassword( newuser, newpass );
    newuser->setLastChange( lstchg );
  }
  newuser->setFullName( mergeLE( lefname, user->getFullName(), one ) );
  if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ) {
    if ( cbsamba->state() != QButton::NoChange ) {
      if ( cbsamba->isChecked() )
        newuser->setCaps( newuser->getCaps() & ~KU::KUser::Cap_Samba );
      else
        newuser->setCaps( newuser->getCaps() | KU::KUser::Cap_Samba );
    }
    samba = newuser->getCaps() & KU::KUser::Cap_Samba;
    kdDebug() << "user : " << newuser->getName() << " caps: " << newuser->getCaps() << " samba: " << samba << endl;

    SID sid;
    if ( samba ) {
      sid.setRID( one ? lerid->text().toUInt() : user->getSID().getRID() );
      sid.setDOM( mergeLE( ledomsid, user->getSID().getDOM(), one ) );
    }
    newuser->setSID( sid );
    newuser->setLoginScript( samba ? 
      mergeLE( leliscript, user->getLoginScript(), one ) : QString::null  );
    newuser->setProfilePath( samba ? 
      mergeLE( leprofile, user->getProfilePath(), one ).replace( "%U", newuser->getName() ) : QString::null );
    newuser->setHomeDrive( samba ? 
      mergeLE( lehomedrive, user->getHomeDrive(), one ) : QString::null );
    newuser->setHomePath( samba ? 
      mergeLE( lehomepath, user->getHomePath(), one ).replace( "%U", newuser->getName() ) : QString::null );
    newuser->setWorkstations( samba ? 
      mergeLE( leworkstations, user->getWorkstations(), one ) : QString::null );
    newuser->setDomain( samba ? 
      mergeLE( ledomain, user->getDomain(), one ) : QString::null );
  }

  if ( kug->getUsers().getCaps() & KU::KUsers::Cap_BSD ) {
    newuser->setOffice( mergeLE( leoffice, user->getOffice(), one ) );
    newuser->setWorkPhone( mergeLE( leophone, user->getWorkPhone(), one ) );
    newuser->setHomePhone( mergeLE( lehphone, user->getHomePhone(), one ) );
    newuser->setClass( mergeLE( leclass, user->getClass(), one ) );
  } else {
    newuser->setOffice1( mergeLE( leoffice1, user->getOffice1(), one ) );
    newuser->setOffice2( mergeLE( leoffice2, user->getOffice2(), one ) );
    newuser->setAddress( mergeLE( leaddress, user->getAddress(), one ) );
  }

  newuser->setHomeDir( posix ? 
    mergeLE( lehome, user->getHomeDir(), one ).replace( "%U", newuser->getName() ) : 
    QString::null );
  if ( posix ) {
    if ( leshell->currentItem() == 0 && ismoreshells ) {
      newuser->setShell( user->getShell() );
    } else if  (
      ( leshell->currentItem() == 0 && !ismoreshells ) ||
      ( leshell->currentItem() == 1 && ismoreshells ) ) {

      newuser->setShell( QString::null );
    } else {
  // TODO: Check shell.
      newuser->setShell( leshell->currentText() );
    }
  } else
    newuser->setShell( QString::null );

  newuser->setDisabled( (cbdisabled->state() == QButton::NoChange) ? user->getDisabled() : cbdisabled->isChecked() );

  if ( kug->getUsers().getCaps() & KU::KUsers::Cap_InetOrg ) {
    newuser->setSurname( mergeLE( lesurname, user->getSurname(), one ) );
    newuser->setEmail( mergeLE( lemail, user->getEmail(), one ) );
  }

  if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Shadow ) {
    newuser->setMin( posix ? mergeSB( lesmin, user->getMin(), one ) : 0 );
    newuser->setMax( posix ? mergeSB( lesmax, user->getMax(), one ) : 0 );
    newuser->setWarn( posix ? mergeSB( leswarn, user->getWarn(), one ) : 0 );
    newuser->setInactive( posix ? mergeSB( lesinact, user->getInactive(), one ) : 0 );
  }

  if ( ( (kug->getUsers().getCaps() & KU::KUsers::Cap_Shadow) && posix ) ||
       ( (kug->getUsers().getCaps() & KU::KUsers::Cap_Samba) && samba ) ||
       ( (kug->getUsers().getCaps() & KU::KUsers::Cap_BSD) && posix ) ) {

    switch ( cbexpire->state() ) {
      case QButton::NoChange:
        newuser->setExpire( user->getExpire() );
        break;
      case QButton::On:
        newuser->setExpire( -1 );
        break;
      case QButton::Off:
        newuser->setExpire( !one && lesexpire->dateTime().toTime_t() == 0 ?
          user->getExpire() : lesexpire->dateTime().toTime_t() );
        break;
    }
  } else {
    newuser->setExpire( -1 );
  }

  if ( !primaryGroup.isEmpty() ) {
    KU::KGroup *group = kug->getGroups().lookup( primaryGroup );
    if ( group ) {
      newuser->setGID( group->getGID() );
      if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ) {
        newuser->setPGSID( group->getSID() );
      }
    }
  }
}

bool propdlg::saveg()
{
  if ( !isgchanged ) return true;

  QCheckListItem *item = (QCheckListItem *) lstgrp->firstChild();
  KU::KGroup *group;

  while(item)
  {
    kdDebug() << "saveg: group name: " << item->text() << endl;
    group = kug->getGroups().lookup(item->text());
    if ( group && item->state() != QCheckListItem::NoChange ) {

      KU::KGroup newgroup( group );
      bool mod = false;
      bool on = item->isOn();
      KU::KUser *user = mUsers.first();

      while ( user ) {
        if ( on && (( !primaryGroup.isEmpty() && primaryGroup != group->getName() ) ||
                    ( primaryGroup.isEmpty() && user->getGID() != group->getGID() )) ) {
          if ( newgroup.addUser( user->getName() ) ) mod = true;
        } else {
          if ( newgroup.removeUser( user->getName() ) ) mod = true;
        }
        user = mUsers.next();
      }

      if ( mod ) kug->getGroups().mod( group, newgroup );
    }
    item = (QCheckListItem *) item->nextSibling();
  }
  return true;
}

bool propdlg::checkShell(const QString &shell)
{
   if (shell.isEmpty())
      return true;
   QStringList shells = readShells();
   return shells.contains(shell);
}

bool propdlg::check()
{
  bool one = ( mUsers.getFirst() == mUsers.getLast() );
  bool posix = !( kug->getUsers().getCaps() & KU::KUsers::Cap_Disable_POSIX ) || !( cbposix->isChecked() );

  if ( one && posix && leid->text().isEmpty() ) {
    KMessageBox::sorry( 0, i18n("You need to specify an UID.") );
    return false;
  }

  if ( one && posix && lehome->text().isEmpty() ) {
    KMessageBox::sorry( 0, i18n("You must specify a home directory.") );
    return false;
  }

  if ( kug->getUsers().getCaps() & KU::KUsers::Cap_InetOrg ) {
    if ( one && lesurname->text().isEmpty() ) {
      KMessageBox::sorry( 0, i18n("You must fill the surname field.") );
      return false;
    }
  }

  if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ) {
    if ( one && lerid->text().isEmpty() && !( cbsamba->isChecked() ) ) {
      KMessageBox::sorry( 0, i18n("You need to specify a samba RID.") );
      return false;
    }
  }

  return true;
}

void propdlg::setpwd()
{
  pwddlg pd( this );

  if ( pd.exec() == QDialog::Accepted ) {
    ischanged = true;
    newpass = pd.getPassword();
    lstchg = now();
    QDateTime datetime;
    datetime.setTime_t( lstchg );
    if ( kug->getUsers().getCaps() & KU::KUsers::Cap_Shadow ||
        kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ||
        kug->getUsers().getCaps() & KU::KUsers::Cap_BSD ) {

        leslstchg->setText( KGlobal::locale()->formatDateTime( datetime, false ) );
    }
    cbdisabled->setChecked( false );
  }
}

void propdlg::slotOk()
{
  if ( ro ) {
    reject();
    return;
  }

  bool one = ( mUsers.getFirst() == mUsers.getLast() );

  uid_t newuid = leid->text().toULong();

  if ( one && ( !( kug->getUsers().getCaps() & KU::KUsers::Cap_Disable_POSIX ) || !cbposix->isChecked() )
               && olduid != newuid )
  {
    if (kug->getUsers().lookup(newuid)) {
      KMessageBox::sorry( 0,
        i18n("User with UID %1 already exists").arg(newuid) );
      return;
    }
  }

  if ( one && ( kug->getUsers().getCaps() & KU::KUsers::Cap_Samba ) && !cbsamba->isChecked() ) {
    uint newrid = lerid->text().toInt();
    if ( oldrid != newrid ) {
      if (kug->getUsers().lookup_sam(newrid)) {
        KMessageBox::sorry( 0,
          i18n("User with RID %1 already exists").arg(newrid) );
        return;
      }
    }
  }

  QString newshell;
  if (leshell->currentItem() != 0)
    newshell = leshell->currentText();

  if (oldshell != newshell)
  {
    if (!checkShell(newshell)) {
      int result = KMessageBox::warningYesNoCancel( 0,
      		i18n("<p>The shell %1 is not yet listed in the file %2. "
      		     "In order to use this shell you must add it to "
      		     "this file first."
      		     "<p>Do you want to add it now?").arg(newshell).arg(QFile::decodeName(SHELL_FILE)),
      		i18n("Unlisted Shell"),
      		i18n("&Add Shell"),
      		i18n("Do &Not Add"));
      	if (result == KMessageBox::Cancel)
      	  return;

      	if (result == KMessageBox::Yes)
      	  addShell(newshell);
    }
  }

  if ( !ischanged && !isgchanged ) {
    reject();
  } else if ( check() ) {
    saveg();
    accept();
  }
}

#include "propdlg.moc"
