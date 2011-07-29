/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi GyĂśrgy <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 or at your option version 3 as published by
 *  the Free Software Foundation.
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

#include "globals.h"

#include <stdio.h>
#include <stdlib.h>

#include <QDateTime>
#include <QGridLayout>

#include <kseparator.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>

#include "ku_edituser.h"
#include "ku_pwdlg.h"
#include "ku_global.h"
#include "ku_misc.h"

void KU_EditUser::addRow(QWidget *parent, QGridLayout *layout, int row,
  QWidget *widget, const QString &label, const QString &what,
  bool two_column, bool nochange)
{
   QLabel *lab = new QLabel( label, parent );
   lab->setBuddy( widget );
   lab->setMinimumSize(lab->sizeHint());
   widget->setMinimumSize(widget->sizeHint());
   layout->addWidget(lab, row, 0);
   if (!what.isEmpty())
   {
      lab->setWhatsThis( what);
      widget->setWhatsThis( what);
   }
   if (two_column)
      layout->addWidget( widget, row, 1, 1, 2 );
   else
      layout->addWidget(widget, row, 1);

   if ( !nochange || ro ) return;
   QCheckBox *nc = new QCheckBox( i18n("Do not change"), parent );
   layout->addWidget( nc, row, 3 );
   nc->hide();
   mNoChanges[ widget ] = nc;
}

KIntSpinBox *KU_EditUser::addDaysGroup(QWidget *parent, QGridLayout *layout, int row,
  const QString &title, bool never)
{
    KIntSpinBox *days;

    QLabel *label = new QLabel( title, parent );
    layout->addWidget( label, row, 0, 1, 2, Qt::AlignRight );

    days = new KIntSpinBox( parent );
    label->setBuddy( days );
    days->setSuffix( i18n(" days") );
    days->setMaximum( 99999 );
    if (never)
    {
      days->setMinimum( -1 );
      days->setSpecialValueText(i18n("Never"));
    }
    else
    {
      days->setMinimum( 0 );
    }
    layout->addWidget( days, row, 2 );

    connect(days, SIGNAL(valueChanged(int)), this, SLOT(changed()));

    QCheckBox *nc = new QCheckBox( i18n("Do not change"), parent );
    layout->addWidget( nc, row, 3 );
    nc->hide();
    mNoChanges[ days ] = nc;

    return days;
}

void KU_EditUser::initDlg()
{
  ro = KU_Global::users()->getCaps() & KU_Users::Cap_ReadOnly;

  QString whatstr;

  // Tab 1: User Info
  {
    QFrame *frame = new QFrame();
    addPage(frame, i18n("User Info"));
    QGridLayout *layout = new QGridLayout( frame );
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
    connect(leid, SIGNAL(textChanged(QString)), this, SLOT(changed()));

    if ( !ro ) {
      pbsetpwd = new QPushButton(i18n("Set &Password..."), frame);
      layout->addWidget(pbsetpwd, 0, 2);
      connect(pbsetpwd, SIGNAL(clicked()), this, SLOT(setpwd()));
    }


    lefname = new KLineEdit(frame);
//    whatstr = i18n("WHAT IS THIS: Full Name");
    addRow(frame, layout, row++, lefname, i18n("Full &name:"), whatstr);
    connect(lefname, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    lefname->setFocus();

    if ( KU_Global::users()->getCaps() & KU_Users::Cap_InetOrg ) {
      lesurname = new KLineEdit(frame);
//    whatstr = i18n("WHAT IS THIS: Surname");
      addRow(frame, layout, row++, lesurname, i18n("Surname:"), whatstr);
      connect(lesurname, SIGNAL(textChanged(QString)), this, SLOT(changed()));

      lemail = new KLineEdit(frame);
//    whatstr = i18n("WHAT IS THIS: Email");
      addRow(frame, layout, row++, lemail, i18n("Email address:"), whatstr);
      connect(lemail, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    }

    leshell = new KComboBox(true, frame);
    leshell->clear();
    leshell->insertItem( 0, i18n("<Empty>") );

    QStringList shells = readShells();
    shells.sort();
    leshell->insertItems( 1, shells );
    connect(leshell, SIGNAL(activated(QString)), this, SLOT(changed()));
    connect(leshell, SIGNAL(editTextChanged(QString)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Login Shell");
    addRow(frame, layout, row++, leshell, i18n("&Login shell:"), whatstr);

    lehome = new KLineEdit(frame);
    connect(lehome, SIGNAL(textChanged(QString)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Home Directory");
    addRow(frame, layout, row++, lehome, i18n("&Home folder:"), whatstr);

    // FreeBSD appears to use the comma separated fields in the GECOS entry
    // differently than Linux.
    if ( KU_Global::users()->getCaps() & KU_Users::Cap_BSD ) {
      leoffice = new KLineEdit(frame);
      connect(leoffice, SIGNAL(textChanged(QString)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Office");
      addRow(frame, layout, row++, leoffice, i18n("&Office:"), whatstr);

      leophone = new KLineEdit(frame);
      connect(leophone, SIGNAL(textChanged(QString)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Office Phone");
      addRow(frame, layout, row++, leophone, i18n("Offi&ce Phone:"), whatstr);

      lehphone = new KLineEdit(frame);
      connect(lehphone, SIGNAL(textChanged(QString)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Home Phone");
      addRow(frame, layout, row++, lehphone, i18n("Ho&me Phone:"), whatstr);

      leclass = new KLineEdit(frame);
      connect(leclass, SIGNAL(textChanged(QString)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Login class");
      addRow(frame, layout, row++, leclass, i18n("Login class:"), whatstr, true);
    } else {
      leoffice1 = new KLineEdit(frame);
      connect(leoffice1, SIGNAL(textChanged(QString)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Office1");
      addRow(frame, layout, row++, leoffice1, i18n("&Office #1:"), whatstr);

      leoffice2 = new KLineEdit(frame);
      connect(leoffice2, SIGNAL(textChanged(QString)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Office2");
      addRow(frame, layout, row++, leoffice2, i18n("O&ffice #2:"), whatstr);

      leaddress = new KLineEdit(frame);
      connect(leaddress, SIGNAL(textChanged(QString)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Address");
      addRow(frame, layout, row++, leaddress, i18n("&Address:"), whatstr);
    }
    cbdisabled = new QCheckBox(frame);
    connect(cbdisabled, SIGNAL(stateChanged(int)), this, SLOT(changed()));
    addRow(frame, layout, row++, cbdisabled, i18n("Account &disabled"), whatstr);

    if ( KU_Global::users()->getCaps() & KU_Users::Cap_Disable_POSIX ) {
      cbposix = new QCheckBox(frame);
      connect(cbposix, SIGNAL(stateChanged(int)), this, SLOT(changed()));
      connect(cbposix, SIGNAL(stateChanged(int)), this, SLOT(cbposixChanged()));
      addRow(frame, layout, row++, cbposix, i18n("Disable &POSIX account information"), whatstr);
    } else {
      cbposix = 0;
    }
    frontrow = row;
  }

  if ( KU_Global::users()->getCaps() & KU_Users::Cap_Shadow ||
       KU_Global::users()->getCaps() & KU_Users::Cap_Samba ||
       KU_Global::users()->getCaps() & KU_Users::Cap_BSD ) {

  // Tab 2 : Password Management
    QFrame *frame = new QFrame();
    addPage(frame, i18n("Password Management"));
    QGridLayout *layout = new QGridLayout( frame );
    int row = 0;

    QDateTime time;
    leslstchg = new QLabel(frame);
    addRow(frame, layout, row++, leslstchg, i18n("Last password change:"), QString(), true);

    layout->addWidget(new KSeparator(Qt::Horizontal, frame), row, 0, 1, 4);
    row++;

    if ( KU_Global::users()->getCaps() & KU_Users::Cap_Shadow ) {
      layout->addWidget( new QLabel( i18n("POSIX parameters:"), frame ), row++, 0 );
      lesmin = addDaysGroup(frame, layout, row++, i18n("Time before password may &not be changed after last password change:"), false);
      lesmax = addDaysGroup(frame, layout, row++, i18n("Time when password &expires after last password change:") );
      leswarn = addDaysGroup(frame, layout, row++, i18n("Time before password expires to &issue an expire warning:"));
      lesinact = addDaysGroup(frame, layout, row++, i18n("Time when account will be &disabled after expiration of password:"));
      layout->addWidget(new KSeparator(Qt::Horizontal, frame), row, 0, 1, 4);
      row++;
    }
    /*
    if ( KU_Global::users()->getCaps() & KU_Users::Cap_Samba ) {
      layout->addWidget( new QLabel( i18n("SAMBA parameters:"), frame ), row++, 0 );
      layout->addMultiCellWidget(new KSeparator(Qt::Horizontal, frame), row, row, 0, 3);
      row++;
    }
    */
    QLabel *label = new QLabel( i18n("&Account will expire on:"), frame );
    layout->addWidget( label, row, 0 );
    lesexpire = new KDateTimeWidget( frame );
    label->setBuddy( lesexpire );
    layout->addWidget( lesexpire, row, 1, 1, 2 );

    cbexpire = new QCheckBox( i18n("Never"), frame );
    layout->addWidget( cbexpire, row++, 3 );

    connect( lesexpire, SIGNAL(valueChanged(QDateTime)), this, SLOT(changed()) );
    connect( cbexpire, SIGNAL(stateChanged(int)), this, SLOT(changed()) );
    connect( cbexpire, SIGNAL(toggled(bool)), lesexpire, SLOT(setDisabled(bool)) );
  }

  // Tab 3: Samba
  if ( KU_Global::users()->getCaps() & KU_Users::Cap_Samba ) {
    QFrame *frame = new QFrame();
    addPage(frame, i18n("Samba"));
    QGridLayout *layout = new QGridLayout( frame );
    int row = 0;

    lerid = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Rid");
    lerid->setValidator(new QIntValidator(frame));
    addRow(frame, layout, row++, lerid, i18n("RID:"), whatstr);
    connect(lerid, SIGNAL(textChanged(QString)), this, SLOT(changed()));

    leliscript = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, leliscript, i18n("Login script:"), whatstr);
    connect(leliscript, SIGNAL(textChanged(QString)), this, SLOT(changed()));

    leprofile = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, leprofile, i18n("Profile path:"), whatstr);
    connect(leprofile, SIGNAL(textChanged(QString)), this, SLOT(changed()));

    lehomedrive = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, lehomedrive, i18n("Home drive:"), whatstr);
    connect(lehomedrive, SIGNAL(textChanged(QString)), this, SLOT(changed()));

    lehomepath = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, lehomepath, i18n("Home path:"), whatstr);
    connect(lehomepath, SIGNAL(textChanged(QString)), this, SLOT(changed()));

    leworkstations = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, leworkstations, i18n("User workstations:"), whatstr);
    connect(leworkstations, SIGNAL(textChanged(QString)), this, SLOT(changed()));

    ledomain = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, ledomain, i18n("Domain name:"), whatstr);
    connect(ledomain, SIGNAL(textChanged(QString)), this, SLOT(changed()));

    ledomsid = new KLineEdit(frame);
//  whatstr = i18n("WHAT IS THIS: Login script");
    addRow(frame, layout, row++, ledomsid, i18n("Domain SID:"), whatstr);
    connect(ledomsid, SIGNAL(textChanged(QString)), this, SLOT(changed()));

    cbsamba = new QCheckBox(frame);
    connect(cbsamba, SIGNAL(stateChanged(int)), this, SLOT(changed()));
    connect(cbsamba, SIGNAL(stateChanged(int)), this, SLOT(cbsambaChanged()));
    addRow(frame, layout, row++, cbsamba, i18n("Disable &Samba account information"), whatstr);
  }

  // Tab 4: Groups
  {
    QFrame *frame = new QFrame();
    addPage(frame, i18n("Groups"));
    QGridLayout *layout = new QGridLayout( frame );

    lstgrp = new QListWidget(frame);
//    lstgrp->setFullWidth(true); // Single column, full widget width.
//    lstgrp->addColumn( i18n("Groups") );
    if ( ro ) lstgrp->setSelectionMode( QListWidget::NoSelection );
//    QString whatstr = i18n("Select the groups that this user belongs to.");
    lstgrp->setWhatsThis( whatstr);
    layout->addWidget( lstgrp, 0, 0, 1, 2 );
    leprigr = new QLabel( i18n("Primary group: "), frame );
    layout->addWidget( leprigr, 1, 0 );
    if ( !ro ) {
      pbprigr = new QPushButton( i18n("Set as Primary"), frame );
      layout->addWidget( pbprigr, 1, 1 );
      connect( pbprigr, SIGNAL(clicked()), this, SLOT(setpgroup()) );
    }
    connect( lstgrp, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(gchanged()) );
  }
//  connect( this, SIGNAL(okClicked()), SLOT(slotOk()) );
}

KU_EditUser::KU_EditUser( const QList<int> &selected,
  QWidget *parent ) :
  KPageDialog( parent)

{
  setCaption(i18n("User Properties"));
  setButtons( Ok | Cancel);
  setDefaultButton(Ok);
  setFaceType(KPageDialog::Tabbed);
  mSelected = selected;
  if ( mSelected.count() > 1 )
    setCaption( i18n("User Properties - %1 Selected Users", mSelected.count() ) );
  else {
    mUser = KU_Global::users()->at( selected[0] );
    mSelected.clear();
  }
  initDlg();
  loadgroups( false );
  selectuser();
  ischanged = false;
  isgchanged = false;
}

KU_EditUser::KU_EditUser( KU_User &user, bool fixedprivgroup,
  QWidget *parent ) :
  KPageDialog(parent)

{
  setCaption(i18n("User Properties"));
  setButtons(Ok | Cancel);
  setDefaultButton( Ok);
  setModal(true);
  setFaceType(KPageDialog::Tabbed);

  mUser = user;
  initDlg();
  loadgroups( fixedprivgroup );
  selectuser();
  ischanged = false;
  isgchanged = false;
}

KU_EditUser::~KU_EditUser()
{
}

void KU_EditUser::cbposixChanged()
{
  bool posix = !( cbposix->checkState() == Qt::Checked );
  leid->setEnabled( posix  && mSelected.isEmpty() );
  leshell->setEnabled( posix );
  lehome->setEnabled( posix );
  if ( KU_Global::users()->getCaps() & KU_Users::Cap_Shadow ) {
    lesmin->setEnabled( posix );
    lesmax->setEnabled( posix );
    leswarn->setEnabled( posix );
    lesinact->setEnabled( posix );
  }
}

void KU_EditUser::cbsambaChanged()
{
  bool samba = !( cbsamba->checkState() == Qt::Checked );
  lerid->setEnabled( samba && mSelected.isEmpty() );
  leliscript->setEnabled( samba );
  leprofile->setEnabled( samba );
  lehomedrive->setEnabled( samba );
  lehomepath->setEnabled( samba );
  leworkstations->setEnabled( samba );
  ledomain->setEnabled( samba );
  ledomsid->setEnabled( samba );
}

void KU_EditUser::setLE( KLineEdit *le, const QString &val, bool first )
{
  if ( first ) {
    le->setText( val );
    if ( ro ) le->setReadOnly( true );
    return;
  }
  if ( val.isEmpty() && le->text().isEmpty() ) return;
  if ( le->text() != val ) {
    le->setText( QLatin1String( "" ) );
    if ( !ro && mNoChanges.contains( le ) ) {
      mNoChanges[ le ]->show();
      mNoChanges[ le ]->setChecked( true );
    }
  }
}

void KU_EditUser::setCB( QCheckBox *cb, bool val, bool first )
{
  if ( first ) {
    cb->setChecked( val );
    if ( ro ) cb->setEnabled( false );
    return;
  }
  if ( cb->isChecked() != val ) {
    cb->setTristate();
    cb->setCheckState( Qt::PartiallyChecked );
  }
}

void KU_EditUser::setSB( KIntSpinBox *sb, int val, bool first )
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

void KU_EditUser::selectuser()
{
  KU_User user;
  bool one = mSelected.isEmpty();
  int index = 0;

  ismoreshells = false;
  user = one ? mUser : KU_Global::users()->at(mSelected[0]);
  olduid = user.getUID();
  oldrid = user.getSID().getRID();
  oldshell = user.getShell();
  lstchg = user.getLastChange();
  QDateTime datetime;
  datetime.setTime_t( lstchg );
  if ( KU_Global::users()->getCaps() & KU_Users::Cap_Shadow ||
       KU_Global::users()->getCaps() & KU_Users::Cap_Samba ||
       KU_Global::users()->getCaps() & KU_Users::Cap_BSD ) {

    leslstchg->setText( KGlobal::locale()->formatDateTime( datetime, KLocale::LongDate ) );
  }

  if ( one ) {
    lbuser->setText( user.getName() );
    leid->setText( QString::number( user.getUID() ) );
    if ( ro ) leid->setReadOnly( true );
    if ( KU_Global::users()->getCaps() & KU_Users::Cap_Samba ) {
      lerid->setText( QString::number( user.getSID().getRID() ) );
      if ( ro ) lerid->setReadOnly( true );
    }
  } else {
    leid->setEnabled( false );
    if ( KU_Global::users()->getCaps() & KU_Users::Cap_Samba ) {
      lerid->setEnabled( false );
    }
  }
  if ( ro ) leshell->setEditable( false );

  bool first;
  while ( true ) {
    first = (index == 0);
    setLE( lefname, user.getFullName(), first );
    QString home;
    home = user.getHomeDir();
    if ( !one ) home.replace( user.getName(), QLatin1String( "%U" ), Qt::CaseInsensitive );
    setLE( lehome, home, first );

    QString shell = user.getShell();
    if ( first ) {
      if ( !shell.isEmpty() ) {
        bool tested = false;
        for ( int i=0; i<leshell->count(); i++ )
          if ( leshell->itemText(i) == shell ) {
            tested = true;
            leshell->setCurrentIndex(i);
            break;
          }
          if ( !tested ) {
            leshell->insertItem( leshell->count(), shell );
            leshell->setCurrentIndex( leshell->count()-1 );
          }
      } else
        leshell->setCurrentIndex( 0 );
    } else {
      if ( leshell->currentText() != shell ) {
        if ( !ismoreshells ) {
          leshell->insertItem( 0, i18n("Do Not Change") );
          ismoreshells = true;
        }
        leshell->setCurrentIndex( 0 );
      }
    }

    setCB( cbdisabled, user.getDisabled(), first );
    if ( KU_Global::users()->getCaps() & KU_Users::Cap_Disable_POSIX ) {
      setCB( cbposix, !(user.getCaps() & KU_User::Cap_POSIX), first );
    }

    if ( KU_Global::users()->getCaps() & KU_Users::Cap_Samba ) {
      setLE( leliscript, user.getLoginScript(), first );
      QString profile;
      profile = user.getProfilePath();
      if ( !one ) profile.replace( user.getName(), QLatin1String( "%U" ), Qt::CaseInsensitive );
      setLE( leprofile, profile, first );
      setLE( lehomedrive, user.getHomeDrive(), first );
      home = user.getHomePath();
      if ( !one ) home.replace( user.getName(), QLatin1String( "%U" ), Qt::CaseInsensitive );
      setLE( lehomepath, home, first );
      setLE( leworkstations, user.getWorkstations(), first );
      setLE( ledomain, user.getDomain(), first );
      setLE( ledomsid, user.getSID().getDOM(), first );
      setCB( cbsamba, !(user.getCaps() & KU_User::Cap_Samba), first );
    }

    if ( KU_Global::users()->getCaps() & KU_Users::Cap_Shadow ||
         KU_Global::users()->getCaps() & KU_Users::Cap_Samba ||
         KU_Global::users()->getCaps() & KU_Users::Cap_BSD ) {

      if ( user.getLastChange() != lstchg ) {
        leslstchg->setText( QLatin1String( "" ) );
        lstchg = 0;
      }

      QDateTime expire;
      expire.setTime_t( user.getExpire() );
      kDebug() << "expiration: " << user.getExpire();
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

    if ( KU_Global::users()->getCaps() & KU_Users::Cap_Shadow ) {
      setSB( lesmin, user.getMin(), first );
      setSB( lesmax, user.getMax(), first );
      setSB( leswarn, user.getWarn(), first );
      setSB( lesinact, user.getInactive(), first );
    }

    if ( KU_Global::users()->getCaps() & KU_Users::Cap_InetOrg ) {
      setLE( lesurname, user.getSurname(), first );
      setLE( lemail, user.getEmail(), first );
    }
    if ( KU_Global::users()->getCaps() & KU_Users::Cap_BSD ) {
      setLE( leoffice, user.getOffice(), first );
      setLE( leophone, user.getWorkPhone(), first );
      setLE( lehphone, user.getHomePhone(), first );
      setLE( leclass, user.getClass(), first );
    } else {
      setLE( leoffice1, user.getOffice1(), first );
      setLE( leoffice2, user.getOffice2(), first );
      setLE( leaddress, user.getAddress(), first );
    }

    first = false;
    if ( index == mSelected.count() ) break;
    user = KU_Global::users()->at(mSelected[index++]);
  }
}

void KU_EditUser::loadgroups( bool fixedprivgroup )
{
  bool wasprivgr = false;

  primaryGroupWasOn = false;

  for ( QList<KU_Group>::Iterator it = KU_Global::groups()->begin();
        it != KU_Global::groups()->end(); ++it ) {
    QString groupName = (*it).getName();
    QListWidgetItem *item = new QListWidgetItem(groupName, lstgrp);
    item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable );
    int index = 0;
    KU_User user = mSelected.count() > 0 ?
      KU_Global::users()->at(mSelected[0]) : mUser;
    while ( true ) {
      bool prigr =
        ( !fixedprivgroup && (*it).getGID() == user.getGID() ) ||
        ( fixedprivgroup && groupName == user.getName() );
      bool on = (*it).lookup_user( user.getName() ) || prigr;

      if ( prigr ) {
        item->setFlags( item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable) );
        if ( !wasprivgr )
          primaryGroup = groupName;
        else
          if ( primaryGroup != groupName ) primaryGroup = QLatin1String( "" );
//      primaryGroupWasOn = group->lookup_user(user->getName());
        wasprivgr = true;
      }

      if ( index == 0 ) {
        item->setCheckState( on ? Qt::Checked : Qt::Unchecked );
      } else
        if ( item->checkState() != ( on ? Qt::Checked : Qt::Unchecked ) ) {
          item->setFlags( item->flags() | Qt::ItemIsTristate );
          item->setCheckState( Qt::PartiallyChecked );
        }
      if ( index == mSelected.count() ) break;
      kDebug() << "index: " << index << " count: " << mSelected.count();
      user = KU_Global::users()->at(mSelected[index++]);
    }
  }

  if ( fixedprivgroup ) {
    KU_User user = mSelected.isEmpty() ? mUser : KU_Global::users()->at(mSelected[0]);
    kDebug() << "privgroup: " << user.getName();
    if ( !wasprivgr ) {
      QListWidgetItem *item = new QListWidgetItem(user.getName(),lstgrp);
      item->setCheckState( Qt::Checked );
      item->setFlags( item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable) );
      primaryGroup = user.getName();
    }
  }
  leprigr->setText( i18n("Primary group: ") + primaryGroup );
}

void KU_EditUser::setpgroup()
{
  isgchanged = true;
  QList<QListWidgetItem*> items = lstgrp->selectedItems();
  if ( items.isEmpty() ) return;
  QListWidgetItem *item = items[0];

  if ( item->text() == primaryGroup ) return;

  bool prevPrimaryGroupWasOn = primaryGroupWasOn;
  primaryGroup = item->text();

  for ( int row = 0; row < lstgrp->count(); row++ ) {

     item = lstgrp->item( row );
     QString groupName = item->text();
     if ( !(item->flags() & Qt::ItemIsEnabled) )
     {
        item->setFlags( item->flags() | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
        item->setCheckState(prevPrimaryGroupWasOn ? Qt::Checked : Qt::Unchecked );
     }
     if ( groupName == primaryGroup )
     {
        primaryGroupWasOn = ( item->checkState() == Qt::Checked );
        item->setFlags( item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable) );
        item->setCheckState( Qt::Checked );
     }
  }
  leprigr->setText( i18n("Primary group: ") + primaryGroup );
}

void KU_EditUser::changed()
{
  QWidget *widget = (QWidget*) sender();
  if ( mNoChanges.contains( widget ) ) mNoChanges[ widget ]->setChecked( false );
  ischanged = true;
  kDebug() << "changed";
}

void KU_EditUser::gchanged()
{
  isgchanged = true;
}

QString KU_EditUser::mergeLE( KLineEdit *le, const QString &val, bool one )
{
  QCheckBox *cb = 0;
  if ( mNoChanges.contains( le ) ) cb = mNoChanges[ le ];
  return ( one || ( cb && !cb->isChecked() ) ) ? le->text() : val;
}

int KU_EditUser::mergeSB( KIntSpinBox *sb, int val, bool one )
{
  QCheckBox *cb = 0;
  if ( mNoChanges.contains( sb ) ) cb = mNoChanges[ sb ];
  return ( one || ( cb && !cb->isChecked() ) ) ? sb->value() : val;
}

void KU_EditUser::mergeUser( const KU_User &user, KU_User &newuser )
{
  QDateTime epoch ;
  epoch.setTime_t(0);
  bool one = mSelected.isEmpty();
  bool posix, samba = false;

  newuser = user;

  if ( KU_Global::users()->getCaps() & KU_Users::Cap_Disable_POSIX && cbposix->checkState() != Qt::PartiallyChecked ) {
    if ( cbposix->isChecked() )
      newuser.setCaps( newuser.getCaps() & ~KU_User::Cap_POSIX );
    else
      newuser.setCaps( newuser.getCaps() | KU_User::Cap_POSIX );
  }
  posix = newuser.getCaps() & KU_User::Cap_POSIX;
  kDebug() << "posix: " << posix;
  if ( one ) {
    newuser.setName( lbuser->text() );
    newuser.setUID( posix ? leid->text().toInt() : 0 );
  }
  if ( !newpass.isNull() ) {
    KU_Global::users()->createPassword( newuser, newpass );
    newuser.setLastChange( lstchg );
  }
  newuser.setFullName( mergeLE( lefname, user.getFullName(), one ) );
  if ( KU_Global::users()->getCaps() & KU_Users::Cap_Samba ) {
    if ( cbsamba->checkState() != Qt::PartiallyChecked ) {
      if ( cbsamba->isChecked() )
        newuser.setCaps( newuser.getCaps() & ~KU_User::Cap_Samba );
      else
        newuser.setCaps( newuser.getCaps() | KU_User::Cap_Samba );
    }
    samba = newuser.getCaps() & KU_User::Cap_Samba;
    kDebug() << "user : " << newuser.getName() << " caps: " << newuser.getCaps() << " samba: " << samba;

    SID sid;
    if ( samba ) {
      sid.setRID( one ? lerid->text().toUInt() : user.getSID().getRID() );
      sid.setDOM( mergeLE( ledomsid, user.getSID().getDOM(), one ) );
    }
    newuser.setSID( sid );
    newuser.setLoginScript( samba ?
      mergeLE( leliscript, user.getLoginScript(), one ) : QString() );
    newuser.setProfilePath( samba ?
      mergeLE( leprofile, user.getProfilePath(), one ).replace( QLatin1String( "%U" ), newuser.getName(), Qt::CaseInsensitive ) : QString() );
    newuser.setHomeDrive( samba ?
      mergeLE( lehomedrive, user.getHomeDrive(), one ) : QString() );
    newuser.setHomePath( samba ?
      mergeLE( lehomepath, user.getHomePath(), one ).replace( QLatin1String( "%U" ), newuser.getName(), Qt::CaseInsensitive ) : QString() );
    newuser.setWorkstations( samba ?
      mergeLE( leworkstations, user.getWorkstations(), one ) : QString() );
    newuser.setDomain( samba ?
      mergeLE( ledomain, user.getDomain(), one ) : QString() );
  }

  if ( KU_Global::users()->getCaps() & KU_Users::Cap_BSD ) {
    newuser.setOffice( mergeLE( leoffice, user.getOffice(), one ) );
    newuser.setWorkPhone( mergeLE( leophone, user.getWorkPhone(), one ) );
    newuser.setHomePhone( mergeLE( lehphone, user.getHomePhone(), one ) );
    newuser.setClass( mergeLE( leclass, user.getClass(), one ) );
  } else {
    newuser.setOffice1( mergeLE( leoffice1, user.getOffice1(), one ) );
    newuser.setOffice2( mergeLE( leoffice2, user.getOffice2(), one ) );
    newuser.setAddress( mergeLE( leaddress, user.getAddress(), one ) );
  }

  newuser.setHomeDir( posix ?
    mergeLE( lehome, user.getHomeDir(), one ).replace( QLatin1String( "%U" ), newuser.getName(), Qt::CaseInsensitive ) : QString() );
  if ( posix ) {
    if ( leshell->currentIndex() == 0 && ismoreshells ) {
      newuser.setShell( user.getShell() );
    } else if  (
      ( leshell->currentIndex() == 0 && !ismoreshells ) ||
      ( leshell->currentIndex() == 1 && ismoreshells ) ) {

      newuser.setShell( QString::null );	//krazy:exclude=nullstrassign for old broken gcc
    } else {
  // TODO: Check shell.
      newuser.setShell( leshell->currentText() );
    }
  } else
    newuser.setShell( QString::null );	//krazy:exclude=nullstrassign for old broken gcc

  newuser.setDisabled( (cbdisabled->checkState() == Qt::PartiallyChecked) ? user.getDisabled() : cbdisabled->isChecked() );

  if ( KU_Global::users()->getCaps() & KU_Users::Cap_InetOrg ) {
    newuser.setSurname( mergeLE( lesurname, user.getSurname(), one ) );
    newuser.setEmail( mergeLE( lemail, user.getEmail(), one ) );
    kDebug() << "surname: " << newuser.getSurname() << " mail: " << newuser.getEmail();
  }

  if ( KU_Global::users()->getCaps() & KU_Users::Cap_Shadow ) {
    newuser.setMin( posix ? mergeSB( lesmin, user.getMin(), one ) : 0 );
    newuser.setMax( posix ? mergeSB( lesmax, user.getMax(), one ) : 0 );
    newuser.setWarn( posix ? mergeSB( leswarn, user.getWarn(), one ) : 0 );
    newuser.setInactive( posix ? mergeSB( lesinact, user.getInactive(), one ) : 0 );
  }

  if ( ( (KU_Global::users()->getCaps() & KU_Users::Cap_Shadow) && posix ) ||
       ( (KU_Global::users()->getCaps() & KU_Users::Cap_Samba) && samba ) ||
       ( (KU_Global::users()->getCaps() & KU_Users::Cap_BSD) && posix ) ) {

    switch ( cbexpire->checkState() ) {
      case Qt::PartiallyChecked:
        newuser.setExpire( user.getExpire() );
        break;
      case Qt::Checked:
        newuser.setExpire( -1 );
        break;
      case Qt::Unchecked:
        newuser.setExpire( !one && lesexpire->dateTime().toTime_t() == 0 ?
          user.getExpire() : lesexpire->dateTime().toTime_t() );
        break;
    }
  } else {
    newuser.setExpire( -1 );
  }

  if ( !primaryGroup.isEmpty() ) {
    int index = KU_Global::groups()->lookup( primaryGroup );
    if ( index != -1 ) {
      KU_Group group = KU_Global::groups()->at( index );
      newuser.setGID( group.getGID() );
      if ( KU_Global::users()->getCaps() & KU_Users::Cap_Samba ) {
        newuser.setPGSID( group.getSID() );
      }
    }
  }
}

bool KU_EditUser::saveg()
{
  if ( !isgchanged ) return true;

  for ( int row = 0; row < lstgrp->count(); row++ ) {

    QListWidgetItem *item = lstgrp->item( row );

    kDebug() << "saveg: group name: " << item->text();
    int grpindex = KU_Global::groups()->lookup(item->text());
    if ( grpindex != -1 && item->checkState() != Qt::PartiallyChecked ) {

      KU_Group group = KU_Global::groups()->at( grpindex );
      bool mod = false;
      int index = 0;
      KU_User user = mSelected.isEmpty() ?
        mUser : KU_Global::users()->at(mSelected[0]);

      while ( true ) {
        if ( item->checkState() == Qt::Checked &&
	           (( !primaryGroup.isEmpty() && primaryGroup != group.getName() ) ||
                    ( primaryGroup.isEmpty() && user.getGID() != group.getGID() )) ) {
          if ( group.addUser( user.getName() ) ) mod = true;
        } else {
          if ( group.removeUser( user.getName() ) ) mod = true;
        }
        if ( index == mSelected.count() ) break;
        user = KU_Global::users()->at(mSelected[index++]);

      }

      if ( mod ) KU_Global::groups()->mod( grpindex, group );
    }
  }

  return true;
}

bool KU_EditUser::checkShell(const QString &shell)
{
   if (shell.isEmpty())
      return true;
   QStringList shells = readShells();
   return shells.contains(shell);
}

bool KU_EditUser::check()
{
  kDebug() << "KU_EditUser::check()";

  bool one = mSelected.isEmpty();
  bool posix = !( KU_Global::users()->getCaps() & KU_Users::Cap_Disable_POSIX ) || !( cbposix->isChecked() );

  if ( one && posix && leid->text().isEmpty() ) {
    KMessageBox::sorry( 0, i18n("You need to specify an UID.") );
    return false;
  }

  if ( one && posix && lehome->text().isEmpty() ) {
    KMessageBox::sorry( 0, i18n("You must specify a home directory.") );
    return false;
  }

  if ( KU_Global::users()->getCaps() & KU_Users::Cap_InetOrg ) {
    if ( one && lesurname->text().isEmpty() ) {
      KMessageBox::sorry( 0, i18n("You must fill the surname field.") );
      return false;
    }
  }

  if ( KU_Global::users()->getCaps() & KU_Users::Cap_Samba ) {
    if ( one && lerid->text().isEmpty() && !( cbsamba->isChecked() ) ) {
      KMessageBox::sorry( 0, i18n("You need to specify a samba RID.") );
      return false;
    }
  }

  return true;
}

void KU_EditUser::setpwd()
{
  KU_PwDlg pd( this );

  if ( pd.exec() == QDialog::Accepted ) {
    ischanged = true;
    newpass = pd.getPassword();
    lstchg = now();
    QDateTime datetime;
    datetime.setTime_t( lstchg );
    if ( KU_Global::users()->getCaps() & KU_Users::Cap_Shadow ||
        KU_Global::users()->getCaps() & KU_Users::Cap_Samba ||
        KU_Global::users()->getCaps() & KU_Users::Cap_BSD ) {

        leslstchg->setText( KGlobal::locale()->formatDateTime( datetime, KLocale::LongDate ) );
    }
    cbdisabled->setChecked( false );
  }
}

void KU_EditUser::accept()
{
  kDebug() << "KU_EditUser::slotOk() ischanged: " << ischanged;
  if ( ro ) {
    reject();
    return;
  }

  bool one = mSelected.isEmpty();

  uid_t newuid = leid->text().toULong();

  if ( one && ( !( KU_Global::users()->getCaps() & KU_Users::Cap_Disable_POSIX ) || !cbposix->isChecked() )
               && olduid != newuid )
  {
    if ( KU_Global::users()->lookup(newuid) != -1 ) {
      KMessageBox::sorry( 0,
        i18n("User with UID %1 already exists", newuid) );
      return;
    }
  }

  if ( one && ( KU_Global::users()->getCaps() & KU_Users::Cap_Samba ) && !cbsamba->isChecked() ) {
    uint newrid = lerid->text().toInt();
    if ( oldrid != newrid ) {
      if ( KU_Global::users()->lookup_sam(newrid) != -1 ) {
        KMessageBox::sorry( 0,
          i18n("User with RID %1 already exists", newrid) );
        return;
      }
    }
  }

  QString newshell;
  if (leshell->currentIndex() != 0)
    newshell = leshell->currentText();

  if (oldshell != newshell)
  {
    if (!checkShell(newshell)) {
      int result = KMessageBox::warningYesNoCancel( 0,
      		i18n("<p>The shell %1 is not yet listed in the file %2. "
      		     "In order to use this shell you must add it to "
      		     "this file first.<br />"
      		     "Do you want to add it now?</p>", newshell, QFile::decodeName(SHELL_FILE)),
      		i18n("Unlisted Shell"),
      		KGuiItem(i18n("&Add Shell")),
      		KGuiItem(i18n("Do &Not Add")));
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
    done( Accepted );
  }
}

#include "ku_edituser.moc"
