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

#include <QByteArray>
#include <QLabel>
#include <QCryptographicHash>

#include <kdebug.h>
#include <klocale.h>
#include <kio/kntlm.h>
#include <kldap/ldapdefs.h>
#include <kldap/ldapdn.h>
#include <kldap/ldapconnection.h>
#include <kldap/ldapoperation.h>

#include "ku_userldap.h"
#include "ku_misc.h"

KU_UserLDAP::KU_UserLDAP(KU_PrefsBase *cfg) : KU_Users( cfg )
{
  schemaversion = 0;

  if ( mCfg->ldapssl() )
    mUrl.setProtocol(QLatin1String( "ldaps" ));
  else
    mUrl.setProtocol(QLatin1String( "ldap" ));

  mUrl.setHost( mCfg->ldaphost() );
  mUrl.setPort( mCfg->ldapport() );
  mUrl.setDn( KLDAP::LdapDN( mCfg->ldapuserbase() + QLatin1Char( ',' ) + mCfg->ldapdn() ) );
  if ( !mCfg->ldapanon() ) {
    mUrl.setUser( mCfg->ldapuser() );
    mUrl.setPass( mCfg->ldappassword() );
    QString binddn = mCfg->ldapbinddn();
    if ( !binddn.isEmpty() )
      mUrl.setExtension( QLatin1String( "bindname" ),binddn );
  }
  mUrl.setFilter( mCfg->ldapuserfilter() );

  if ( mCfg->ldaptls() ) mUrl.setExtension( QLatin1String( "x-tls" ), QLatin1String( "" ) );
  if ( mCfg->ldapsasl() ) {
    mUrl.setExtension( QLatin1String( "x-sasl" ), QLatin1String( "" ) );
    mUrl.setExtension( QLatin1String( "x-mech" ), mCfg->ldapsaslmech() );
  }

  mUrl.setScope(KLDAP::LdapUrl::One);
  mUrl.setExtension(QLatin1String( "x-dir" ),QLatin1String( "base" ));

  if ( mCfg->ldaptimelimit() )
    mUrl.setExtension(QLatin1String( "x-timelimit" ),QString::number(mCfg->ldaptimelimit()));
  if ( mCfg->ldapsizelimit() )
    mUrl.setExtension(QLatin1String( "x-sizelimit" ),QString::number(mCfg->ldapsizelimit()));
  if ( mCfg->ldappagesize() )
    mUrl.setExtension(QLatin1String( "x-pagesize" ),QString::number(mCfg->ldappagesize()));

  caps = Cap_Passwd | Cap_Disable_POSIX;
  if ( mCfg->ldapshadow() ) caps |= Cap_Shadow;
  if ( mCfg->ldapstructural() == KU_PrefsBase::EnumLdapstructural::inetOrgPerson )
    caps |= Cap_InetOrg;

  if ( mCfg->ldapsam() ) {
    caps |= Cap_Samba;
    domsid = mCfg->samdomsid();
  }
}

KU_UserLDAP::~KU_UserLDAP()
{
}

void KU_UserLDAP::result( KLDAP::LdapSearch *search )
{
  kDebug() << "LDAP result: " << search->error() << " " << search->errorString();
  mProg->hide();

  if ( search->error() ) {
    mErrorString = KLDAP::LdapConnection::errorString(search->error());
    mErrorDetails = search->errorString();
    mOk = false;
  } else {
    mOk = true;
  }
}

void KU_UserLDAP::data( KLDAP::LdapSearch *, const KLDAP::LdapObject& data )
{
  KU_User user;
  QStringList objectclasses;

  KLDAP::LdapAttrMap attrs = data.attributes();
  for ( KLDAP::LdapAttrMap::ConstIterator it = attrs.constBegin(); it != attrs.constEnd(); ++it ) {
    QString name = it.key().toLower();
    if ( name == QLatin1String( "objectclass" ) ) {
      for ( KLDAP::LdapAttrValue::ConstIterator it2 = (*it).constBegin(); it2 != (*it).constEnd(); ++it2 ) {
        if ( (*it2).toLower() == "posixaccount" )
            user.setCaps( user.getCaps() | KU_User::Cap_POSIX );
        else if ( (*it2).toLower() ==  "sambasamaccount" )
            user.setCaps( user.getCaps() | KU_User::Cap_Samba );
	else if ( (*it2).toLower() != "inetorgperson" &&
	          (*it2).toLower() != "shadowaccount" &&
                  (*it2).toLower() != "account" )
            objectclasses.append( QLatin1String( (*it2) ) );
      }
      continue;
    }

    KLDAP::LdapAttrValue values = (*it);
    if ( values.isEmpty() ) continue;
    QString val = QString::fromUtf8( values.first(), values.first().size() );
    if ( name == QLatin1String( "uidnumber" ) )
      user.setUID( val.toLong() );
    else if ( name == QLatin1String( "gidnumber" ) )
      user.setGID( val.toLong() );
    else if ( name == QLatin1String( "uid" ) || name == QLatin1String( "userid" ) )
      user.setName( val );
    else if ( name == QLatin1String( "sn" ) )
      user.setSurname( val );
    else if ( name == QLatin1String( "mail" ) )
      user.setEmail( val );
    else if ( name == QLatin1String( "homedirectory" ) )
      user.setHomeDir( val );
    else if ( name == QLatin1String( "loginshell" ) )
      user.setShell( val );
    else if ( name == QLatin1String( "postaladdress" ) )
      user.setAddress( val );
    else if ( name == QLatin1String( "telephonenumber" ) ) {
      user.setOffice1( val );
      if ( values.size() > 1 )
        user.setOffice2( QString::fromUtf8( values[1], values[1].size() ) );
    } else if ( name == QLatin1String( "gecos" ) ) {
      QString name, f1, f2, f3;
      parseGecos( values.first(), name, f1, f2, f3 );
      if ( user.getFullName().isEmpty() ) user.setFullName( val );
      if ( user.getOffice1().isEmpty() ) user.setOffice1( f1 );
      if ( user.getOffice2().isEmpty() ) user.setOffice2( f1 );
      if ( user.getAddress().isEmpty() ) user.setAddress( f1 );
    } else if ( name == QLatin1String( "cn" ) ) {
      if ( user.getFullName().isEmpty() || mCfg->ldapcnfullname() )
        user.setFullName( val );
      if ( user.getName().isEmpty() )
        user.setName( val );
    } else if ( name == QLatin1String( "displayname" ) ) {
      user.setFullName( val );
    } else if ( name == QLatin1String( "userpassword" ) ) {
      if ( !val.isEmpty() ) user.setDisabled( false );
      user.setPwd( val );
    } else if ( name == QLatin1String( "shadowlastchange" ) ) {
      if ( user.getLastChange() == 0 ) //sambapwdlastset is more precise
        user.setLastChange( daysToTime( val.toLong() ) );
    } else if ( name == QLatin1String( "shadowmin" ) )
      user.setMin( val.toInt() );
    else if ( name == QLatin1String( "shadowmax" ) )
      user.setMax( val.toLong() );
    else if ( name == QLatin1String( "shadowwarning" ) )
      user.setWarn( val.toLong() );
    else if ( name == QLatin1String( "shadowinactive" ) )
      user.setInactive( val.toLong() );
    else if ( name == QLatin1String( "shadowexpire" ) )
      user.setExpire( val.toLong() );
    else if ( name == QLatin1String( "shadowflag" ) )
      user.setFlag( val.toLong() );
    else if ( name == QLatin1String( "sambaacctflags" ) ) {
      if ( !val.contains( QLatin1Char( 'D' ) ) ) user.setDisabled( false );
    } else if ( name == QLatin1String( "sambasid" ) )
      user.setSID( val );
    else if ( name == QLatin1String( "sambaprimarygroupsid" ) )
      user.setPGSID( val );
    else if ( name == QLatin1String( "sambalmpassword" ) )
      user.setLMPwd( val );
    else if ( name == QLatin1String( "sambantpassword" ) )
      user.setNTPwd( val );
    else if ( name == QLatin1String( "sambahomepath" ) )
      user.setHomePath( val );
    else if ( name == QLatin1String( "sambahomedrive" ) )
      user.setHomeDrive( val );
    else if ( name == QLatin1String( "sambalogonscript" ) )
      user.setLoginScript( val );
    else if ( name == QLatin1String( "sambaprofilepath" ) )
      user.setProfilePath( val );
    else if ( name == QLatin1String( "sambauserworkstations" ) )
      user.setWorkstations( val );
    else if ( name == QLatin1String( "sambadomainname" ) )
      user.setDomain( val );
    else if ( name == QLatin1String( "sambapwdlastset" ) )
      user.setLastChange( val.toLong() );
//these new attributes introduced around samba 3.0.6
    else if ( name == QLatin1String( "sambapasswordhistory" ) || name == QLatin1String( "sambalogonhours" ) )
      schemaversion = 1;

  }

  kDebug() << "new user: " << user.getName();
  if ( !objectclasses.isEmpty() ) {
    mObjectClasses.insert( count(), objectclasses );
    kDebug() << "user: " << user.getName() << " other objectclasses: " << objectclasses.join(QLatin1String( "," ));
  }
  append( user );

  if ( ( count() & 7 ) == 7 ) {
    mProg->setValue( mProg->value() + mAdv );
    if ( mProg->value() == 0 ) mAdv = 1;
    if ( mProg->value() == mProg->maximum()-1 ) mAdv = -1;
  }
}

bool KU_UserLDAP::reload()
{
  kDebug() << "KU_UserLDAP::reload()";
  mErrorString = mErrorDetails = QString();
  mObjectClasses.clear();
  mProg = new QProgressDialog( 0 );
  mProg->setLabel( new QLabel( i18n("Loading Users From LDAP") ) );
  mProg->setAutoClose( false );
  mProg->setAutoReset( false );
  mProg->setMaximum( 100 );
  mAdv = 1;
  mOk = true;
  mProg->show();
  qApp->processEvents();
  KLDAP::LdapSearch search;

  connect( &search,
    SIGNAL(data(KLDAP::LdapSearch*,KLDAP::LdapObject)),
    this, SLOT (data(KLDAP::LdapSearch*,KLDAP::LdapObject)) );
  connect( &search,
    SIGNAL(result(KLDAP::LdapSearch*)),
    this, SLOT (result(KLDAP::LdapSearch*)) );

  if (search.search( mUrl )) {
    mProg->exec();
    if ( mProg->wasCanceled() ) search.abandon();
  } else {
    kDebug() << "search failed";
    mOk = false;
    mErrorString = KLDAP::LdapConnection::errorString(search.error());
    mErrorDetails = search.errorString();
  }
  delete mProg;
  return( mOk );
}

QString KU_UserLDAP::getRDN(const KU_User &user) const
{
  switch ( mCfg->ldapuserrdn() ) {
    case KU_PrefsBase::EnumLdapuserrdn::uid:
      return QLatin1String( "uid=" ) + user.getName();
    case KU_PrefsBase::EnumLdapuserrdn::uidNumber:
      return QLatin1String( "uidNumber=" ) + QString::number( user.getUID() );
    case KU_PrefsBase::EnumLdapuserrdn::cn: {
      QString cn = mCfg->ldapcnfullname() ? user.getFullName() : user.getName();
      if ( cn.isEmpty() ) cn = user.getName();
      return QLatin1String( "cn=" ) + cn;
    }
  }
  return QLatin1String( "" );
}

void KU_UserLDAP::createPassword( KU_User &user, const QString &password )
{
  switch ( mCfg->ldappasswordhash() ) {
    case KU_PrefsBase::EnumLdappasswordhash::Clear:
      user.setPwd( password );
      break;
    case KU_PrefsBase::EnumLdappasswordhash::CRYPT:
      user.setPwd( QLatin1String( "{CRYPT}" ) + encryptPass( password, false ) );
      break;
    case KU_PrefsBase::EnumLdappasswordhash::MD5: {
      QCryptographicHash md5(QCryptographicHash::Md5);
      md5.addData( password.toUtf8() );
      user.setPwd( QLatin1String( "{MD5}" ) + QLatin1String( md5.result().toBase64() ) );
      break;
    }
    case KU_PrefsBase::EnumLdappasswordhash::SMD5: {
      QCryptographicHash md5(QCryptographicHash::Md5);
      QByteArray salt = genSalt( 8 );
      QByteArray pwd = password.toUtf8() + salt;

      md5.addData( pwd );
      user.setPwd( QLatin1String( "{SMD5}" ) + QLatin1String( (md5.result() + salt).toBase64() ) );
      break;
    }
    case KU_PrefsBase::EnumLdappasswordhash::SHA: {
      QCryptographicHash sha1(QCryptographicHash::Sha1);

      sha1.addData( password.toUtf8() );
      user.setPwd( QLatin1String( "{SHA}" ) + QLatin1String( sha1.result().toBase64() ) );
      break;
    }
    case KU_PrefsBase::EnumLdappasswordhash::SSHA: {
      QCryptographicHash sha1(QCryptographicHash::Sha1);

      QByteArray salt = genSalt( 8 );
      QByteArray pwd = password.toUtf8() + salt;

      sha1.addData( pwd );
      user.setPwd( QLatin1String( "{SSHA}" ) + QLatin1String( (sha1.result() + salt).toBase64() ));
      break;
    }
  }

  if ( caps & Cap_Samba ) {
    quint8 hex[33];

    QByteArray ntlmhash;
    ntlmhash = KNTLM::ntlmHash( password );
    unsigned char *hash = (unsigned char*) ntlmhash.data();

    snprintf( (char*) &hex, 33,
      "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
            hash[0], hash[1], hash[2], hash[3], hash[4], hash[5],
            hash[6], hash[7], hash[8], hash[9], hash[10], hash[11],
            hash[12], hash[13], hash[14], hash[15]);

    user.setNTPwd( QString::fromLatin1( (const char*) &hex, 32 ) );

    if ( mCfg->lanmanhash() ) {

      QByteArray lmhash;
      lmhash = KNTLM::lmHash( password );
      unsigned char *hash = (unsigned char*) lmhash.data();
      snprintf( (char*) &hex, 33,
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
            hash[0], hash[1], hash[2], hash[3], hash[4], hash[5],
            hash[6], hash[7], hash[8], hash[9], hash[10], hash[11],
            hash[12], hash[13], hash[14], hash[15]);

      user.setLMPwd( QString::fromLatin1( (const char*) &hex, 32 ) );
    } else {
      user.setLMPwd( QLatin1String( "" ) );
    }
  }
}

void KU_UserLDAP::createModStruct( const KU_User &user, int oldindex, KLDAP::LdapOperation::ModOps &ops)
{
  QString gecos, cn, pwd, samflags;
  QList<QByteArray> vals;

  bool mod = ( oldindex != -1 );

  pwd = user.getPwd();
  if ( user.getDisabled() ) pwd = QLatin1String( "" );

  cn = mCfg->ldapcnfullname() ? user.getFullName() : user.getName();
  if ( cn.isEmpty() ) cn = user.getName();

  gecos = QString::fromLatin1("%1,%2,%3,%4")
    .arg(user.getFullName())
    .arg(user.getOffice1())
    .arg(user.getOffice2())
    .arg(user.getAddress());

  samflags = QLatin1String( "[U" );
  samflags += user.getDisabled() ? QLatin1Char( 'D' ) : QLatin1Char( ' ' );
  samflags += QLatin1String( "         ]" );

  vals.append( caps & Cap_InetOrg ? "inetOrgPerson" : "account" );
  if ( user.getCaps() & KU_User::Cap_POSIX ) {
    vals.append( "posixAccount" );
  }
  if ( ( caps & Cap_Shadow ) && ( user.getCaps() & KU_User::Cap_POSIX ) ) {
    vals.append( "shadowAccount" );
  }
  if ( ( caps & Cap_Samba ) && ( user.getCaps() & KU_User::Cap_Samba ) ) {
    vals.append( "sambaSamAccount" );
  }

  if ( mod && mObjectClasses.contains( oldindex ) ) {
    QStringList ocs = mObjectClasses[ oldindex ];
    kDebug() << user.getName() << " has additional objectclasses: " << ocs.join(QLatin1String( "," ));
    QStringList::iterator it;
    for ( it = ocs.begin(); it != ocs.end(); ++it ) {
      vals.append( (*it).toUtf8() );
    }
  }
  ku_add2ops( ops, QLatin1String( "objectClass" ), vals );
  vals.clear();

  ku_add2ops( ops, QLatin1String( "cn" ), cn.toUtf8() );
  ku_add2ops( ops, caps & Cap_InetOrg ? QLatin1String( "uid" ) : QLatin1String( "userid" ), user.getName().toUtf8() );

  if ( ( user.getCaps() & KU_User::Cap_POSIX ) || ( caps & Cap_InetOrg ) ) {
    ku_add2ops( ops, QLatin1String( "userpassword" ), pwd.toUtf8(), true );
  }

  if ( user.getCaps() & KU_User::Cap_POSIX ) {
    ku_add2ops( ops, QLatin1String( "uidnumber" ), QString::number(user.getUID()).toUtf8() );
    ku_add2ops( ops, QLatin1String( "gidnumber" ), QString::number(user.getGID()).toUtf8() );
    ku_add2ops( ops, QLatin1String( "gecos" ), !mCfg->ldapgecos() ? QByteArray() : QByteArray( gecos.toLatin1() ) );
    ku_add2ops( ops, QLatin1String( "homedirectory" ), user.getHomeDir().toUtf8() );
    ku_add2ops( ops, QLatin1String( "loginshell" ), user.getShell().toUtf8() );
  } else if (mod) {
    ku_add2ops( ops, QLatin1String( "uidnumber" ) );
    ku_add2ops( ops, QLatin1String( "gidnumber" ) );
    ku_add2ops( ops, QLatin1String( "gecos" ) );
    ku_add2ops( ops, QLatin1String( "homedirectory" ) );
    ku_add2ops( ops, QLatin1String( "loginshell" ) );
  }

  if ( caps & Cap_InetOrg ) {
    ku_add2ops( ops, QLatin1String( "sn" ), user.getSurname().toUtf8() );
    ku_add2ops( ops, QLatin1String( "mail" ), user.getEmail().toUtf8() );
    ku_add2ops( ops, QLatin1String( "displayName" ), user.getFullName().toUtf8() );
    ku_add2ops( ops, QLatin1String( "postaladdress" ), user.getAddress().toUtf8() );
    vals.append( user.getOffice1().toUtf8() );
    vals.append( user.getOffice2().toUtf8() );
    ku_add2ops( ops, QLatin1String( "telephoneNumber" ), vals );
    vals.clear();
  }

  if ( caps & Cap_Samba ) {
    if ( user.getCaps() & KU_User::Cap_Samba ) {
      ku_add2ops( ops, QLatin1String( "sambadomainname" ), user.getDomain().toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambauserworkstations" ), user.getWorkstations().toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambahomepath" ), user.getHomePath().toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambahomedrive" ), user.getHomeDrive().toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambalogonscript" ), user.getLoginScript().toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambaprofilepath" ), user.getProfilePath().toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambalmpassword" ), user.getLMPwd().toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambantpassword" ), user.getNTPwd().toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambasid" ), user.getSID().getSID().toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambaacctflags" ), samflags.toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambaprimarygroupsid" ), user.getPGSID().getSID().toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambapwdlastset" ), QString::number( user.getLastChange() ).toUtf8() );
      if ( user.getExpire() != -1 )
        vals.append( QString::number( user.getExpire() ).toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambakickofftime" ), vals );
      vals.clear();
    } else if (mod) {
      ku_add2ops( ops, QLatin1String( "sambadomainname" ) );
      ku_add2ops( ops, QLatin1String( "sambauserworkstations" ) );
      ku_add2ops( ops, QLatin1String( "sambahomepath" ) );
      ku_add2ops( ops, QLatin1String( "sambahomedrive" ) );
      ku_add2ops( ops, QLatin1String( "sambalogonscript" ) );
      ku_add2ops( ops, QLatin1String( "sambaprofilepath" ) );
      ku_add2ops( ops, QLatin1String( "sambalmpassword" ) );
      ku_add2ops( ops, QLatin1String( "sambantpassword" ) );
      ku_add2ops( ops, QLatin1String( "sambasid" ) );
      ku_add2ops( ops, QLatin1String( "sambaacctflags" ) );
      ku_add2ops( ops, QLatin1String( "sambaprimarygroupsid" ) );
      ku_add2ops( ops, QLatin1String( "sambapwdlastset" ) );
      ku_add2ops( ops, QLatin1String( "sambakickofftime" ) );
      if ( schemaversion > 0 ) {
        ku_add2ops( ops, QLatin1String( "sambapasswordhistory" ) );
        ku_add2ops( ops, QLatin1String( "sambalogonhours" ) );
      }
    }
  }

  if ( caps & Cap_Shadow ) {
    if ( user.getCaps() & KU_User::Cap_POSIX ) {
      ku_add2ops( ops, QLatin1String( "shadowlastchange" ),  QString::number( timeToDays( user.getLastChange() ) ).toUtf8() );
      ku_add2ops( ops, QLatin1String( "shadowmin" ), QString::number( user.getMin() ).toUtf8() );
      ku_add2ops( ops, QLatin1String( "shadowmax" ), QString::number( user.getMax() ).toUtf8() );
      ku_add2ops( ops, QLatin1String( "shadowwarning" ), QString::number( user.getWarn() ).toUtf8() );
      ku_add2ops( ops, QLatin1String( "shadowinactive" ), QString::number( user.getInactive() ).toUtf8() );
      ku_add2ops( ops, QLatin1String( "shadowexpire" ), QString::number( timeToDays( user.getExpire() ) ).toUtf8() );
      ku_add2ops( ops, QLatin1String( "shadowflag" ), QString::number( user.getFlag() ).toUtf8() );
    } else if (mod) {
      ku_add2ops( ops, QLatin1String( "shadowlastchange" ) );
      ku_add2ops( ops, QLatin1String( "shadowmin" ) );
      ku_add2ops( ops, QLatin1String( "shadowmax" ) );
      ku_add2ops( ops, QLatin1String( "shadowwarning" ) );
      ku_add2ops( ops, QLatin1String( "shadowinactive" ) );
      ku_add2ops( ops, QLatin1String( "shadowexpire" ) );
      ku_add2ops( ops, QLatin1String( "shadowflag" ) );
    }
  }
}

bool KU_UserLDAP::dbcommit()
{
  mAddSucc.clear();
  mDelSucc.clear();
  mModSucc.clear();
  mErrorString = mErrorDetails = QString();

  KLDAP::LdapConnection conn( mUrl );

  if ( conn.connect() != KLDAP_SUCCESS ) {
    mErrorString = conn.connectionError();
    return false;
  }

  KLDAP::LdapOperation op( conn );

  if ( op.bind_s() != KLDAP_SUCCESS ) {
    mErrorString = KLDAP::LdapConnection::errorString(conn.ldapErrorCode());
    mErrorDetails = conn.ldapErrorString();
    return false;
  }

  KLDAP::LdapOperation::ModOps ops;

  mProg = new QProgressDialog( 0 );
  mProg->setLabel( new QLabel(i18n("LDAP Operation") ) );
  mProg->setAutoClose( false );
  mProg->setAutoReset( false );
  mProg->setMaximum( mAdd.count() + mDel.count() + mMod.count() );

  //modify
  for ( KU_Users::ModList::Iterator it = mMod.begin(); it != mMod.end(); ++it ) {
    QString oldrdn = getRDN( at( it.key() ) );
    QString newrdn = getRDN( it.value() );

    if ( oldrdn != newrdn ) {
      int ret = op.rename_s( KLDAP::LdapDN( oldrdn + QLatin1Char( ',' ) + mUrl.dn().toString() ),
        newrdn,
                             QLatin1String( mUrl.dn().toString().toUtf8() ),
        true );

      if ( ret != KLDAP_SUCCESS ) {
        mErrorString = KLDAP::LdapConnection::errorString(conn.ldapErrorCode());
        mErrorDetails = conn.ldapErrorString();
        delete mProg;
        return false;
      }
    }

    ops.clear();
    createModStruct( it.value(), it.key(), ops );
    int ret = op.modify_s( KLDAP::LdapDN( getRDN( it.value() ) + QLatin1Char( ',' ) + mUrl.dn().toString() ), ops );
    if ( ret != KLDAP_SUCCESS ) {
      mErrorString = KLDAP::LdapConnection::errorString(conn.ldapErrorCode());
      mErrorDetails = conn.ldapErrorString();
      delete mProg;
      return false;
    } else {
      mModSucc.insert( it.key(), it.value() );
    }
  }

  //add
  for ( KU_Users::AddList::Iterator it = mAdd.begin(); it != mAdd.end(); ++it ) {
    ops.clear();
    createModStruct( (*it), -1, ops );
    kDebug() << "add name: " << (*it).getName();
    int ret = op.add_s( KLDAP::LdapDN( getRDN( (*it) ) + QLatin1Char( ',' ) + mUrl.dn().toString() ), ops );
    if ( ret != KLDAP_SUCCESS ) {
      mErrorString = KLDAP::LdapConnection::errorString(conn.ldapErrorCode());
      mErrorDetails = conn.ldapErrorString();
      delete mProg;
      return false;
    } else {
      mAddSucc.append( (*it) );
    }
  }

  //del
  for ( KU_Users::DelList::Iterator it = mDel.begin(); it != mDel.end(); ++it ) {
    kDebug() << "delete name: " << at((*it)).getName();
    int ret = op.del_s( KLDAP::LdapDN( getRDN( at((*it)) ) + QLatin1Char( ',' ) + mUrl.dn().toString() ) );
    if ( ret != KLDAP_SUCCESS ) {
      mErrorString = KLDAP::LdapConnection::errorString(conn.ldapErrorCode());
      mErrorDetails = conn.ldapErrorString();
      delete mProg;
      return false;
    } else {
      mDelSucc.append( (*it) );
    }
  }

  delete mProg;
  return true;
}

#include "ku_userldap.moc"
