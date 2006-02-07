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

#include <kdebug.h>

#include <kcombobox.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <ktabwidget.h>
#include <kabc/ldapconfigwidget.h>
#include <kabc/ldapurl.h>
#include <klocale.h>

#include "ku_configdlg.h"
#include "ku_generalsettings.h"
#include "ku_filessettings.h"
#include "ku_ldapsettings.h"
#include "ku_ldapsamba.h"
#include "ku_passwordpolicy.h"
#include "ku_misc.h"

KU_ConfigDlg::KU_ConfigDlg( KConfigSkeleton *config, QWidget *parent, const char *name ) :
  KConfigDialog( parent, name, config, IconList,
  Default|Ok|Apply|Cancel|Help, Ok, true )
{
  KTabWidget *page1 = new KTabWidget( this );
  page1->setMargin( KDialog::marginHint() );

  {
    QDialog *page1a = new QDialog ( 0 );
    Ui::KU_GeneralSettings *ui = new Ui::KU_GeneralSettings();
    ui->setupUi( page1a );
    ui->kcfg_shell->insertItem( i18n("<Empty>" ) );
    ui->kcfg_shell->insertStringList( readShells() );
    page1->addTab( page1a, i18n("Connection") );
  }
  {
    QDialog *page1b = new QDialog( 0 );
    Ui::KU_PasswordPolicy *ui = new Ui::KU_PasswordPolicy();
    ui->setupUi( page1b );
    page1->addTab( page1b, i18n("Password Policy") );
  }
  addPage( page1, i18n("General"), "", i18n("General Settings") );

  {
    QDialog *page2 = new QDialog( this );
    Ui::KU_FilesSettings *ui = new Ui::KU_FilesSettings();
    ui->setupUi( page2 );
    addPage( page2, i18n("Files"), "", i18n("File Source Settings") );
  }

  KTabWidget *page3 = new KTabWidget( this );
  page3->setMargin( KDialog::marginHint() );

  ldconf =
    new KABC::LdapConfigWidget(
       KABC::LdapConfigWidget::W_USER |
       KABC::LdapConfigWidget::W_PASS |
       KABC::LdapConfigWidget::W_BINDDN |
       KABC::LdapConfigWidget::W_REALM |
       KABC::LdapConfigWidget::W_HOST |
       KABC::LdapConfigWidget::W_PORT |
       KABC::LdapConfigWidget::W_VER |
       KABC::LdapConfigWidget::W_DN |
       KABC::LdapConfigWidget::W_SECBOX |
       KABC::LdapConfigWidget::W_AUTHBOX |
       KABC::LdapConfigWidget::W_TIMELIMIT | 
       KABC::LdapConfigWidget::W_SIZELIMIT | 
       KABC::LdapConfigWidget::W_PAGESIZE,
        0 );

  page3->addTab( ldconf, i18n("Connection") );

  {
    QDialog *page3b = new QDialog( 0 );
    Ui::KU_LdapSettings *ui = new Ui::KU_LdapSettings();
    ui->setupUi( page3b );
    page3->addTab( page3b, i18n("Settings") );
  }
  {
    QDialog *page3c = new QDialog( 0 );
    sambaui = new Ui::KU_LdapSamba();
    sambaui->setupUi( page3c );
    connect( sambaui->domQuery, SIGNAL(clicked()), SLOT(slotQueryClicked()) );
    page3->addTab( page3c, i18n("Samba") );
  }
  addPage( page3, i18n("LDAP"), "", i18n("LDAP Source Settings") );
}

void KU_ConfigDlg::slotQueryClicked()
{

  KABC::LDAPUrl _url = ldconf->url();

  mResult.clear();
  mDomain.name = "";
  mDomain.sid = "";
  mDomain.ridbase = 1000;

  QStringList attrs;
  QString filter = "(objectClass=sambaDomain)";
  QString dom = sambaui->kcfg_samdomain->text();
  if ( !dom.isEmpty() ) filter = "(&(sambaDomainName=" + dom + ")"+filter+")";
  attrs.append("sambaDomainName");
  attrs.append("sambaSID");
  attrs.append("sambaAlgorithmicRidBase");
  _url.setAttributes( attrs );
  _url.setScope( KABC::LDAPUrl::One );
  _url.setExtension( "x-dir", "base" );
  _url.setFilter( filter );

  kDebug() << "sendQuery url: " << _url.prettyURL() << endl;
  mLdif.startParsing();
  KIO::Job *job = KIO::get( _url, true, false );
//  job->addMetaData("no-auth-prompt","true");
  connect( job, SIGNAL( data( KIO::Job*, const QByteArray& ) ),
    this, SLOT( loadData( KIO::Job*, const QByteArray& ) ) );
  connect( job, SIGNAL( result( KIO::Job* ) ),
    this, SLOT( loadResult( KIO::Job* ) ) );

  mProg = new QProgressDialog( 0 );
  mProg->setLabel( new QLabel(_url.prettyURL()) );
  mProg->setValue( 0 );
  mProg->setMaximum( 1 );
  mProg->setAutoClose( false );
  mProg->setAutoReset( false );
  mProg->exec();
  if ( mProg->wasCanceled() ) {
    kDebug() << "query cancelled!" << endl;
    job->kill( true );
  } else {
    if ( !mErrorMsg.isEmpty() ) 
      KMessageBox::error( this, mErrorMsg );
    else {
      mDomain = mResult.first();
      if ( !mDomain.name.isEmpty() && !mDomain.sid.isEmpty() ) {
        sambaui->kcfg_samdomain->setText( mDomain.name );
        sambaui->kcfg_samdomsid->setText( mDomain.sid );
        sambaui->kcfg_samridbase->setValue( mDomain.ridbase );
      }
    }
  }
  delete mProg;
  kDebug() << "domQueryx" << endl;

}

void KU_ConfigDlg::loadData( KIO::Job*, const QByteArray& d )
{
  KABC::LDIF::ParseVal ret;

  if ( d.size() ) {
    mLdif.setLDIF( d );
  } else {
    mLdif.endLDIF();
  }
  do {
    ret = mLdif.nextItem();
    switch ( ret ) {
      case KABC::LDIF::Item:
        if ( mLdif.attr() == "sambaDomainName" ) 
          mDomain.name = QString::fromUtf8( mLdif.val(), mLdif.val().size() );
        else if ( mLdif.attr() == "sambaSID" ) 
          mDomain.sid = QString::fromUtf8( mLdif.val(), mLdif.val().size() );
        else if ( mLdif.attr() == "sambaAlgorithmicRidBase" )
          mDomain.ridbase = QString::fromUtf8( mLdif.val(), mLdif.val().size() ).toUInt();
        break;
      case KABC::LDIF::EndEntry:
        mProg->setValue( 1 );
        if ( !mDomain.name.isEmpty() && !mDomain.sid.isEmpty() )
          mResult.push_back( mDomain );
        mDomain.sid = "";
        mDomain.name = "";
        mDomain.ridbase = 1000;
        break;
      
    }
  } while ( ret != KABC::LDIF::MoreData );
}

void KU_ConfigDlg::loadResult( KIO::Job* job)
{
  int error = job->error();
  if ( error && error != KIO::ERR_USER_CANCELED )
    mErrorMsg = job->errorString();
  else
    mErrorMsg = "";

  mProg->hide();
}

#include "ku_configdlg.moc"
