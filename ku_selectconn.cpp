/*
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
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
#include <QRegExp>
#include <QVBoxLayout>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kinputdialog.h>

#include "ku_global.h"
#include "ku_selectconn.h"
#include "ku_configdlg.h"

KU_SelectConn::KU_SelectConn(const QString &selected, QWidget *parent) :
  KDialog( parent)
{
  QStringList conns;
  setButtons(Ok | Apply | Cancel | User1 | User2 | User3);
  setCaption(i18n("Connection Selection"));
  setButtonText( User3, i18n("&New...") );
  setButtonText( User2, i18n("&Edit") );
  setButtonText( User1, i18n("&Delete") );

  QFrame *page = new QFrame();
  setMainWidget( page );
  QVBoxLayout *topLayout = new QVBoxLayout( page );
  topLayout->setSpacing( KDialog::spacingHint() );
  QLabel *label = new QLabel( i18n("Defined connections:"), page );
  mCombo = new KComboBox( page );
  mSelected = selected;
  kDebug() << "selected item: " << mSelected;

  conns = KGlobal::config()->groupList();
  QStringList::iterator it = conns.begin();
  int i = 0, sel = 0;
  while ( it != conns.end() ) {
    if ( (*it).startsWith( QLatin1String( "connection-" ) ) ) {
      (*it).remove( QRegExp(QLatin1String( "^connection-" )) );
      if ( (*it) == mSelected ) sel = i;
      i++;
      it++;
    } else
      it = conns.erase( it );
  }
  mCombo->insertItems( 0, conns );
  //TODO i18n ?
  if ( mCombo->count() == 0 ) mCombo->addItem( QLatin1String( "default" ) );
  mCombo->setCurrentIndex( sel );
  mSelected = connSelected();
  topLayout->addWidget( label );
  topLayout->addWidget( mCombo );

  connect( this, SIGNAL(user1Clicked()), SLOT(slotUser1()) );
  connect( this, SIGNAL(user2Clicked()), SLOT(slotUser2()) );
  connect( this, SIGNAL(user3Clicked()), SLOT(slotUser3()) );
  connect( this, SIGNAL(applyClicked()), SLOT(slotApply()));
}

QString KU_SelectConn::connSelected()
{
  return mCombo->currentText();
}

void KU_SelectConn::slotUser3()
{
  newconn = KInputDialog::getText( QString::null,	//krazy:exclude=nullstrassign for old broken gcc
    i18n("Please type the name of the new connection:") );
  if ( newconn.isEmpty() ) return;
  if ( KGlobal::config()->groupList().contains( QLatin1String( "connection-" ) + newconn ) ) {
    KMessageBox::sorry( 0, i18n("A connection with this name already exists.") );
    return;
  }

  KSharedConfig::Ptr config( KGlobal::config() );
  KU_PrefsBase kcfg( config, newconn );

  KU_ConfigDlg cfgdlg( &kcfg, this );
  connect(&cfgdlg, SIGNAL(settingsChanged(QString)), this, SLOT(slotNewApplySettings()));
  cfgdlg.exec();

  if ( newconn.isEmpty() )
    emit( applyClicked() );
}

void KU_SelectConn::slotNewApplySettings()
{
  if ( !newconn.isEmpty() ) {
    mCombo->addItem( newconn );
    mCombo->setCurrentIndex( mCombo->count()-1 );
    mSelected = newconn;
  }
}

void KU_SelectConn::slotUser2()
{
  kDebug() << "slotUser2: " << connSelected();

  KSharedConfig::Ptr config( KGlobal::config() );
  KU_PrefsBase kcfg( config, connSelected() );
  kcfg.readConfig();

  KU_ConfigDlg cfgdlg( &kcfg, this );
  connect( &cfgdlg, SIGNAL(settingsChanged(QString)), this, SLOT(slotApplySettings()) );
  cfgdlg.exec();
}

void KU_SelectConn::slotUser1()
{
  QString conn = connSelected();
  if ( KMessageBox::warningContinueCancel( 0, i18n("Do you really want to delete the connection '%1'?",
     conn ),i18n("Delete Connection"),KStandardGuiItem::del() ) == KMessageBox::Cancel ) return;

  KGlobal::config()->deleteGroup( QLatin1String( "connection-" ) + conn );
  KGlobal::config()->sync();
  mCombo->removeItem( mCombo->currentIndex() );
  if ( mCombo->count() == 0 ) {
      //TODO i18n ?
    mCombo->addItem( QLatin1String( "default" ) );
    mCombo->setCurrentIndex( 0 );
  }
  kDebug() << "slotUser1: " << conn << " " << mSelected;
  if ( mSelected == conn )
    emit( applyClicked() );
}

void KU_SelectConn::slotApply()
{
  kDebug() << "slotApply()";
  if ( connSelected() != mSelected ) {
    mSelected = connSelected();
    emit( applyClicked() );
  }
}

void KU_SelectConn::slotApplySettings()
{
  kDebug() << "slotApplySettings()";
  if ( connSelected() == mSelected )
    emit( applyClicked() );
}

#include "ku_selectconn.moc"
