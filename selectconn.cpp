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

#include <qpixmap.h>
#include <qlayout.h>

#include <qlabel.h>
#include <qgrid.h>
#include <qregexp.h>

#include <kdebug.h>
#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kinputdialog.h>

#include "kglobal_.h"
#include "selectconn.h"
#include "editDefaults.h"

SelectConn::SelectConn(const QString &selected, QWidget* parent, const char * name) :
  KDialogBase( Plain, WStyle_DialogBorder, parent, name, true,
  i18n("Connection Selection"), Ok | Apply | Cancel | User1 | User2 | User3 )
{
  QStringList conns;

  setButtonText( User3, i18n("&New...") );
  setButtonText( User2, i18n("&Edit") );
  setButtonText( User1, i18n("&Delete") );

  QFrame *page = plainPage();
  QVBoxLayout *topLayout = new QVBoxLayout( page, 0, KDialog::spacingHint() );
  QLabel *label = new QLabel( i18n("Defined connections:"), page );
  mCombo = new KComboBox( page );
  mSelected = selected;
  kdDebug() << "selected item: " << mSelected << endl;

  conns = kapp->sharedConfig()->groupList();
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
}

QString SelectConn::connSelected()
{
  return mCombo->currentText();
}

void SelectConn::slotUser3()
{
  newconn = KInputDialog::getText( QString::null,
    i18n("Please type the name of the new connection:") );
  if ( newconn.isEmpty() ) return;
  if ( kapp->sharedConfig()->groupList().contains( "connection-" + newconn ) ) {
    KMessageBox::sorry( 0, i18n("A connection with this name already exists.") );
    return;
  }

  KUserPrefsBase kcfg( kapp->sharedConfig(), newconn );

  editDefaults eddlg( &kcfg, this );
  connect(&eddlg, SIGNAL(settingsChanged()), this, SLOT(slotNewApplySettings()));
  eddlg.exec();

  if ( newconn.isEmpty() )
    emit( applyClicked() );
}

void SelectConn::slotNewApplySettings()
{
  if ( !newconn.isEmpty() ) {
    mCombo->insertItem( newconn );
    mCombo->setCurrentItem( mCombo->count()-1 );
    mSelected = newconn;
  }
}

void SelectConn::slotUser2()
{
  kdDebug() << "slotUser2: " << connSelected() << endl;
  KUserPrefsBase kcfg( kapp->sharedConfig(), connSelected() );
  kcfg.readConfig();

  editDefaults eddlg( &kcfg, this );
  connect( &eddlg, SIGNAL(settingsChanged()), this, SLOT(slotApplySettings()) );

  eddlg.exec();
}

void SelectConn::slotUser1()
{
  QString conn = connSelected();
  if ( KMessageBox::warningContinueCancel( 0, i18n("Do you really want to delete the connection '%1'?").
    arg( conn ),i18n("Delete Connection"),KStdGuiItem::del() ) == KMessageBox::Cancel ) return;

  kapp->sharedConfig()->deleteGroup( "connection-" + conn, true );
  kapp->sharedConfig()->sync();
  mCombo->removeItem( mCombo->currentItem() );
  if ( mCombo->count() == 0 ) {
    mCombo->insertItem( "default" );
    mCombo->setCurrentItem( 0 );
  }
  kdDebug() << "slotUser1: " << conn << " " << mSelected << endl;
  if ( mSelected == conn )
    emit( applyClicked() );
}

void SelectConn::slotApply()
{
  kdDebug() << "slotApply()" << endl;
  if ( connSelected() != mSelected ) {
    mSelected = connSelected();
    emit( applyClicked() );
  }
}

void SelectConn::slotApplySettings()
{
  kdDebug() << "slotApplySettings()" << endl;
  if ( connSelected() == mSelected )
    emit( applyClicked() );
}

#include "selectconn.moc"
