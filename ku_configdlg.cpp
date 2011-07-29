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

#include <kdebug.h>

#include <kcombobox.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <ktabwidget.h>
#include <kldap/ldapconfigwidget.h>
#include <kldap/ldapurl.h>
#include <klocale.h>

#include "ku_configdlg.h"
#include "ku_misc.h"

#include "ui_ku_generalsettings.h"
#include "ui_ku_filessettings.h"
#include "ui_ku_ldapsettings.h"
#include "ui_ku_ldapsamba.h"
#include "ui_ku_passwordpolicy.h"

KU_ConfigDlg::KU_ConfigDlg( KConfigSkeleton *config, QWidget *parent, const char *name ) :
    KConfigDialog( parent, QLatin1String( name ), config),sambaui(0)
{
  setFaceType(List);
  setButtons(Default|Ok|Apply|Cancel|Help);
  setDefaultButton(Ok);
  setModal(true);
  KTabWidget *page1 = new KTabWidget( this );
  {
    Ui::KU_GeneralSettings ui;
    QFrame *frame = new QFrame ( page1 );
    ui.setupUi( frame );
    ui.kcfg_shell->addItem( i18n("<Empty>" ) );
    ui.kcfg_shell->addItems( readShells() );
    page1->addTab( frame, i18n("Connection") );
  }
  {
    Ui::KU_PasswordPolicy ui;
    QFrame *frame = new QFrame ( page1 );
    ui.setupUi( frame );
    page1->addTab( frame, i18n("Password Policy") );
    ui.kcfg_smax->setSuffix(ki18np(" day", " days"));
    ui.kcfg_sinact->setSuffix(ki18np(" day", " days"));
    ui.kcfg_swarn->setSuffix(ki18np(" day", " days"));
    ui.kcfg_smin->setSuffix(ki18np(" day", " days"));
  }
  addPage( page1, i18n("General"), QLatin1String( "kuser" ), i18n("General Settings") );

  {
    QFrame *page2 = new QFrame( this );
    fileui = new Ui::KU_FilesSettings();
    fileui->setupUi( page2 );
    addPage( page2, i18n("Files"), QLatin1String( "document-properties" ), i18n("File Source Settings") );
  }

  KTabWidget *page3 = new KTabWidget( this );

  ldconf =
    new KLDAP::LdapConfigWidget(
       KLDAP::LdapConfigWidget::W_USER |
       KLDAP::LdapConfigWidget::W_PASS |
       KLDAP::LdapConfigWidget::W_BINDDN |
       KLDAP::LdapConfigWidget::W_REALM |
       KLDAP::LdapConfigWidget::W_HOST |
       KLDAP::LdapConfigWidget::W_PORT |
       KLDAP::LdapConfigWidget::W_VER |
       KLDAP::LdapConfigWidget::W_DN |
       KLDAP::LdapConfigWidget::W_SECBOX |
       KLDAP::LdapConfigWidget::W_AUTHBOX |
       KLDAP::LdapConfigWidget::W_TIMELIMIT |
       KLDAP::LdapConfigWidget::W_SIZELIMIT |
       KLDAP::LdapConfigWidget::W_PAGESIZE,
        0 );

  page3->addTab( ldconf, i18n("Connection") );

  {
    QFrame *page3b = new QFrame( 0 );
    ldapui = new Ui::KU_LdapSettings();
    ldapui->setupUi( page3b );
    page3->addTab( page3b, i18n("Settings") );
  }
  {
    QFrame *page3c = new QFrame( 0 );
    sambaui = new Ui::KU_LdapSamba();
    sambaui->setupUi( page3c );
    connect( sambaui->domQuery, SIGNAL(clicked()), SLOT(slotQueryClicked()) );
    page3->addTab( page3c, i18n("Samba") );
  }
  addPage( page3, i18n("LDAP"), QLatin1String( "network-server-database" ), i18n("LDAP Source Settings") );
  setHelp(QString(),QLatin1String( "kuser" ));
}

KU_ConfigDlg::~KU_ConfigDlg()
{
  delete sambaui;
  delete ldapui;
  delete fileui;
}

void KU_ConfigDlg::slotQueryClicked()
{

  KLDAP::LdapUrl _url = ldconf->url();

  mResult.clear();
  mDomain.name.clear();
  mDomain.sid.clear();
  mDomain.ridbase = 1000;

  QStringList attrs;
  QString filter = QLatin1String( "(objectClass=sambaDomain)" );
  QString dom = sambaui->kcfg_samdomain->text();
  if ( !dom.isEmpty() ) filter = QLatin1String( "(&(sambaDomainName=" ) + dom + QLatin1Char( ')' ) + filter + QLatin1Char( ')' );
  attrs.append(QLatin1String( "sambaDomainName" ));
  attrs.append(QLatin1String( "sambaSID" ));
  attrs.append(QLatin1String( "sambaAlgorithmicRidBase" ));
  _url.setAttributes( attrs );
  _url.setScope( KLDAP::LdapUrl::One );
  _url.setExtension( QLatin1String( "x-dir" ), QLatin1String( "base" ) );
  _url.setFilter( filter );

  kDebug() << "sendQuery url: " << _url.prettyUrl();
  mLdif.startParsing();
  KIO::Job *job = KIO::get( _url, KIO::Reload, KIO::HideProgressInfo );
//  job->addMetaData("no-auth-prompt","true");
  connect( job, SIGNAL(data(KIO::Job*,QByteArray)),
    this, SLOT(loadData(KIO::Job*,QByteArray)) );
  connect( job, SIGNAL(result(KJob*)),
    this, SLOT(loadResult(KJob*)) );

  mProg = new QProgressDialog( 0 );
  mProg->setLabel( new QLabel(_url.prettyUrl()) );
  mProg->setValue( 0 );
  mProg->setMaximum( 1 );
  mProg->setAutoClose( false );
  mProg->setAutoReset( false );
  mProg->exec();
  if ( mProg->wasCanceled() ) {
    kDebug() << "query cancelled!";
    job->kill( KJob::Quietly );
  } else {
    if ( !mErrorMsg.isEmpty() )
      KMessageBox::error( this, mErrorMsg );
    else {
      if ( !mResult.isEmpty() ) {
        mDomain = mResult.first();
        if ( !mDomain.name.isEmpty() && !mDomain.sid.isEmpty() ) {
          sambaui->kcfg_samdomain->setText( mDomain.name );
          sambaui->kcfg_samdomsid->setText( mDomain.sid );
          sambaui->kcfg_samridbase->setValue( mDomain.ridbase );
        }
      }
    }
  }
  delete mProg;
  kDebug() << "domQueryx";

}

void KU_ConfigDlg::loadData( KIO::Job*, const QByteArray& d )
{
  KLDAP::Ldif::ParseValue ret;

  if ( d.size() ) {
    mLdif.setLdif( d );
  } else {
    mLdif.endLdif();
  }
  do {
    ret = mLdif.nextItem();
    switch ( ret ) {
      case KLDAP::Ldif::Item:
        if ( mLdif.attr() == QLatin1String( "sambaDomainName" ) )
          mDomain.name = QString::fromUtf8( mLdif.value(), mLdif.value().size() );
        else if ( mLdif.attr() == QLatin1String( "sambaSID" ) )
          mDomain.sid = QString::fromUtf8( mLdif.value(), mLdif.value().size() );
        else if ( mLdif.attr() == QLatin1String( "sambaAlgorithmicRidBase" ) )
          mDomain.ridbase = QString::fromUtf8( mLdif.value(), mLdif.value().size() ).toUInt();
        break;
      case KLDAP::Ldif::EndEntry:
        mProg->setValue( 1 );
        if ( !mDomain.name.isEmpty() && !mDomain.sid.isEmpty() )
          mResult.push_back( mDomain );
        mDomain.sid.clear();
        mDomain.name.clear();
        mDomain.ridbase = 1000;
      default:
        break;
    }
  } while ( ret != KLDAP::Ldif::MoreData );
}

void KU_ConfigDlg::loadResult( KJob* job)
{
  int error = job->error();
  if ( error && error != KIO::ERR_USER_CANCELED )
    mErrorMsg = job->errorString();
  else
    mErrorMsg = QLatin1String( "" );

  mProg->hide();
}

#include "ku_configdlg.moc"
