/*
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
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
  KDialog( parent, i18n("Connection Selection"), 
  Ok | Apply | Cancel | User1 | User2 | User3 )
{
  QStringList conns;

  setButtonText( User3, i18n("&New...") );
  setButtonText( User2, i18n("&Edit") );
  setButtonText( User1, i18n("&Delete") );

  QFrame *page = new QFrame();
  setMainWidget( page );
  QVBoxLayout *topLayout = new QVBoxLayout( page, 0, KDialog::spacingHint() );
  QLabel *label = new QLabel( i18n("Defined connections:"), page );
  mCombo = new KComboBox( page );
  mSelected = selected;
  kDebug() << "selected item: " << mSelected << endl;

  conns = KGlobal::sharedConfig()->groupList();
  QStringList::iterator it = conns.begin();
  int i = 0, sel = 0;
  while ( it != conns.end() ) {
    if ( (*it).startsWith( "connection-" ) ) {
      (*it).remove( QRegExp("^connection-") );
      if ( (*it) == mSelected ) sel = i;
      i++;
      it++;
    } else
      it = conns.remove( it );
  }
  mCombo->insertStringList( conns );
  if ( mCombo->count() == 0 ) mCombo->insertItem( "default" );
  mCombo->setCurrentItem( sel );
  mSelected = connSelected();
  topLayout->addWidget( label );
  topLayout->addWidget( mCombo );
  
  connect( this, SIGNAL(user1Clicked()), SLOT(slotUser1()) );
  connect( this, SIGNAL(user2Clicked()), SLOT(slotUser2()) );
  connect( this, SIGNAL(user3Clicked()), SLOT(slotUser3()) );
}

QString KU_SelectConn::connSelected()
{
  return mCombo->currentText();
}

void KU_SelectConn::slotUser3()
{
  newconn = KInputDialog::getText( QString::null,
    i18n("Please type the name of the new connection:") );
  if ( newconn.isEmpty() ) return;
  if ( kapp->sharedConfig()->groupList().contains( "connection-" + newconn ) ) {
    KMessageBox::sorry( 0, i18n("A connection with this name already exists.") );
    return;
  }

  KSharedConfig::Ptr config( KGlobal::sharedConfig() );
  KU_PrefsBase kcfg( config, newconn );

  KU_ConfigDlg cfgdlg( &kcfg, this );
  connect(&cfgdlg, SIGNAL(settingsChanged(const QString&)), this, SLOT(slotNewApplySettings()));
  cfgdlg.exec();

  if ( newconn.isEmpty() )
    emit( applyClicked() );
}

void KU_SelectConn::slotNewApplySettings()
{
  if ( !newconn.isEmpty() ) {
    mCombo->insertItem( newconn );
    mCombo->setCurrentItem( mCombo->count()-1 );
    mSelected = newconn;
  }
}

void KU_SelectConn::slotUser2()
{
  kDebug() << "slotUser2: " << connSelected() << endl;

  KSharedConfig::Ptr config( KGlobal::sharedConfig() );
  KU_PrefsBase kcfg( config, newconn );
  kcfg.readConfig();

  KU_ConfigDlg cfgdlg( &kcfg, this );
  connect( &cfgdlg, SIGNAL(settingsChanged(const QString&)), this, SLOT(slotApplySettings()) );
  cfgdlg.exec();
}

void KU_SelectConn::slotUser1()
{
  QString conn = connSelected();
  if ( KMessageBox::warningContinueCancel( 0, i18n("Do you really want to delete the connection '%1'?").
    arg( conn ),i18n("Delete Connection"),KStdGuiItem::del() ) == KMessageBox::Cancel ) return;

  KGlobal::sharedConfig()->deleteGroup( "connection-" + conn );
  KGlobal::sharedConfig()->sync();
  mCombo->removeItem( mCombo->currentItem() );
  if ( mCombo->count() == 0 ) {
    mCombo->insertItem( "default" );
    mCombo->setCurrentItem( 0 );
  }
  kDebug() << "slotUser1: " << conn << " " << mSelected << endl;
  if ( mSelected == conn )
    emit( applyClicked() );
}

void KU_SelectConn::slotApply()
{
  kDebug() << "slotApply()" << endl;
  if ( connSelected() != mSelected ) {
    mSelected = connSelected();
    emit( applyClicked() );
  }
}

void KU_SelectConn::slotApplySettings()
{
  kDebug() << "slotApplySettings()" << endl;
  if ( connSelected() == mSelected )
    emit( applyClicked() );
}

#include "ku_selectconn.moc"
