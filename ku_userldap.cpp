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
    mUrl.setProtocol("ldaps");
  else
    mUrl.setProtocol("ldap");

  mUrl.setHost( mCfg->ldaphost() );
  mUrl.setPort( mCfg->ldapport() );
  mUrl.setDn( KLDAP::LdapDN( mCfg->ldapuserbase() + ',' + mCfg->ldapdn() ) );
  if ( !mCfg->ldapanon() ) {
    mUrl.setUser( mCfg->ldapuser() );
    mUrl.setPass( mCfg->ldappassword() );
    QString binddn = mCfg->ldapbinddn();
    if ( !binddn.isEmpty() ) 
      mUrl.setExtension( "bindname",binddn );
  }
  mUrl.setFilter( mCfg->ldapuserfilter() );

  if ( mCfg->ldaptls() ) mUrl.setExtension( "x-tls", "" );
  if ( mCfg->ldapsasl() ) {
    mUrl.setExtension( "x-sasl", "" );
    mUrl.setExtension( "x-mech", mCfg->ldapsaslmech() );
  }

  mUrl.setScope(KLDAP::LdapUrl::One);
  mUrl.setExtension("x-dir","base");

  if ( mCfg->ldaptimelimit() )
    mUrl.setExtension("x-timelimit",QString::number(mCfg->ldaptimelimit()));
  if ( mCfg->ldapsizelimit() )
    mUrl.setExtension("x-sizelimit",QString::number(mCfg->ldapsizelimit()));
  if ( mCfg->ldappagesize() )
    mUrl.setExtension("x-pagesize",QString::number(mCfg->ldappagesize()));

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
    if ( name == "objectclass" ) {
      for ( KLDAP::LdapAttrValue::ConstIterator it2 = (*it).constBegin(); it2 != (*it).constEnd(); ++it2 ) {
        if ( (*it2).toLower() == "posixaccount" )
            user.setCaps( user.getCaps() | KU_User::Cap_POSIX );
        else if ( (*it2).toLower() == "sambasamaccount" )
            user.setCaps( user.getCaps() | KU_User::Cap_Samba );
	else if ( (*it2).toLower() != "inetorgperson" &&
	          (*it2).toLower() != "shadowaccount" &&
                  (*it2).toLower() != "account" )
            objectclasses.append( (*it2) );
      }
      continue;
    }
                      
    KLDAP::LdapAttrValue values = (*it);
    if ( values.isEmpty() ) continue;
    QString val = QString::fromUtf8( values.first(), values.first().size() );
    if ( name == "uidnumber" )
      user.setUID( val.toLong() );
    else if ( name == "gidnumber" )
      user.setGID( val.toLong() );
    else if ( name == "uid" || name == "userid" )
      user.setName( val );
    else if ( name == "sn" )
      user.setSurname( val );
    else if ( name == "mail" )
      user.setEmail( val );
    else if ( name == "homedirectory" )
      user.setHomeDir( val );
    else if ( name == "loginshell" )
      user.setShell( val );
    else if ( name == "postaladdress" )
      user.setAddress( val );
    else if ( name == "telephonenumber" ) {
      user.setOffice1( val );
      if ( values.size() > 1 )
        user.setOffice2( QString::fromUtf8( values[1], values[1].size() ) );
    } else if ( name == "gecos" ) {
      QString name, f1, f2, f3;
      parseGecos( values.first(), name, f1, f2, f3 );
      if ( user.getFullName().isEmpty() ) user.setFullName( val );
      if ( user.getOffice1().isEmpty() ) user.setOffice1( f1 );
      if ( user.getOffice2().isEmpty() ) user.setOffice2( f1 );
      if ( user.getAddress().isEmpty() ) user.setAddress( f1 );
    } else if ( name == "cn" ) {
      if ( user.getFullName().isEmpty() || mCfg->ldapcnfullname() )
        user.setFullName( val );
      if ( user.getName().isEmpty() )
        user.setName( val );
    } else if ( name == "displayname" ) {
      user.setFullName( val );
    } else if ( name == "userpassword" ) {
      if ( !val.isEmpty() ) user.setDisabled( false );
      user.setPwd( val );
    } else if ( name == "shadowlastchange" ) {
      if ( user.getLastChange() == 0 ) //sambapwdlastset is more precise
        user.setLastChange( daysToTime( val.toLong() ) );
    } else if ( name == "shadowmin" )
      user.setMin( val.toInt() );
    else if ( name == "shadowmax" )
      user.setMax( val.toLong() );
    else if ( name == "shadowwarning" )
      user.setWarn( val.toLong() );
    else if ( name == "shadowinactive" )
      user.setInactive( val.toLong() );
    else if ( name == "shadowexpire" )
      user.setExpire( val.toLong() );
    else if ( name == "shadowflag" )
      user.setFlag( val.toLong() );
    else if ( name == "sambaacctflags" ) {
      if ( !val.contains( 'D' ) ) user.setDisabled( false );
    } else if ( name == "sambasid" )
      user.setSID( val );
    else if ( name == "sambaprimarygroupsid" )
      user.setPGSID( val );
    else if ( name == "sambalmpassword" )
      user.setLMPwd( val );
    else if ( name == "sambantpassword" )
      user.setNTPwd( val );
    else if ( name == "sambahomepath" )
      user.setHomePath( val );
    else if ( name == "sambahomedrive" )
      user.setHomeDrive( val );
    else if ( name == "sambalogonscript" )
      user.setLoginScript( val );
    else if ( name == "sambaprofilepath" )
      user.setProfilePath( val );
    else if ( name == "sambauserworkstations" )
      user.setWorkstations( val );
    else if ( name == "sambadomainname" )
      user.setDomain( val );
    else if ( name == "sambapwdlastset" )
      user.setLastChange( val.toLong() );
//these new attributes introduced around samba 3.0.6
    else if ( name == "sambapasswordhistory" || name == "sambalogonhours" )
      schemaversion = 1;

  }

  kDebug() << "new user: " << user.getName();
  if ( !objectclasses.isEmpty() ) {
    mObjectClasses.insert( count(), objectclasses );
    kDebug() << "user: " << user.getName() << " other objectclasses: " << objectclasses.join(",");
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
    SIGNAL( data( KLDAP::LdapSearch*, const KLDAP::LdapObject& ) ), 
    this, SLOT ( data ( KLDAP::LdapSearch*, const KLDAP::LdapObject&) ) );
  connect( &search, 
    SIGNAL( result( KLDAP::LdapSearch* ) ), 
    this, SLOT ( result ( KLDAP::LdapSearch* ) ) );

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
      return "uid=" + user.getName();
    case KU_PrefsBase::EnumLdapuserrdn::uidNumber:
      return "uidNumber=" + QString::number( user.getUID() );
    case KU_PrefsBase::EnumLdapuserrdn::cn: {
      QString cn = mCfg->ldapcnfullname() ? user.getFullName() : user.getName();
      if ( cn.isEmpty() ) cn = user.getName();
      return "cn=" + cn;
    }
  }
  return "";
}

void KU_UserLDAP::createPassword( KU_User &user, const QString &password )
{
  switch ( mCfg->ldappasswordhash() ) {
    case KU_PrefsBase::EnumLdappasswordhash::Clear:
      user.setPwd( password );
      break;
    case KU_PrefsBase::EnumLdappasswordhash::CRYPT:
      user.setPwd( "{CRYPT}" + encryptPass( password, false ) );
      break;
    case KU_PrefsBase::EnumLdappasswordhash::MD5: {
      QCryptographicHash md5(QCryptographicHash::Md5);
      md5.addData( password.toUtf8() );
      user.setPwd( "{MD5}" + md5.result().toBase64() );
      break;
    }
    case KU_PrefsBase::EnumLdappasswordhash::SMD5: {
      QCryptographicHash md5(QCryptographicHash::Md5);
      QByteArray salt = genSalt( 8 );
      QByteArray pwd = password.toUtf8() + salt;

      md5.addData( pwd );
      user.setPwd( "{SMD5}" + (md5.result() + salt).toBase64() );
      break;
    }
    case KU_PrefsBase::EnumLdappasswordhash::SHA: {
      QCryptographicHash sha1(QCryptographicHash::Sha1);

      sha1.addData( password.toUtf8() );
      user.setPwd( "{SHA}" + sha1.result().toBase64() );
      break;
    }
    case KU_PrefsBase::EnumLdappasswordhash::SSHA: {
      QCryptographicHash sha1(QCryptographicHash::Sha1);

      QByteArray salt = genSalt( 8 );
      QByteArray pwd = password.toUtf8() + salt;

      sha1.addData( pwd );
      user.setPwd( "{SSHA}" + (sha1.result() + salt).toBase64() );
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
      user.setLMPwd( "" );
    }
  }
}

void KU_UserLDAP::createModStruct( const KU_User &user, int oldindex, KLDAP::LdapOperation::ModOps &ops)
{
  QString gecos, cn, pwd, samflags;
  QList<QByteArray> vals;
  
  bool mod = ( oldindex != -1 );

  pwd = user.getPwd();
  if ( user.getDisabled() ) pwd = "";

  cn = mCfg->ldapcnfullname() ? user.getFullName() : user.getName();
  if ( cn.isEmpty() ) cn = user.getName();

  gecos = QString::fromLatin1("%1,%2,%3,%4")
    .arg(user.getFullName())
    .arg(user.getOffice1())
    .arg(user.getOffice2())
    .arg(user.getAddress());

  samflags = "[U";
  samflags += user.getDisabled() ? 'D' : ' ';
  samflags += "         ]";

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
    kDebug() << user.getName() << " has additional objectclasses: " << ocs.join(",");
    QStringList::iterator it;
    for ( it = ocs.begin(); it != ocs.end(); ++it ) {
      vals.append( (*it).toUtf8() );
    }
  }
  ku_add2ops( ops, "objectClass", vals );
  vals.clear();
  
  ku_add2ops( ops, "cn", cn.toUtf8() );
  ku_add2ops( ops, caps & Cap_InetOrg ? "uid" : "userid", user.getName().toUtf8() );
  
  if ( ( user.getCaps() & KU_User::Cap_POSIX ) || ( caps & Cap_InetOrg ) ) {
    ku_add2ops( ops, "userpassword", pwd.toUtf8(), true );
  }

  if ( user.getCaps() & KU_User::Cap_POSIX ) {
    ku_add2ops( ops, "uidnumber", QString::number(user.getUID()).toUtf8() );
    ku_add2ops( ops, "gidnumber", QString::number(user.getGID()).toUtf8() );
    ku_add2ops( ops, "gecos", !mCfg->ldapgecos() ? QByteArray() : QByteArray( gecos.toLatin1() ) );
    ku_add2ops( ops, "homedirectory", user.getHomeDir().toUtf8() );
    ku_add2ops( ops, "loginshell", user.getShell().toUtf8() );
  } else if (mod) {
    ku_add2ops( ops, "uidnumber" );
    ku_add2ops( ops, "gidnumber" );
    ku_add2ops( ops, "gecos" );
    ku_add2ops( ops, "homedirectory" );
    ku_add2ops( ops, "loginshell" );
  }

  if ( caps & Cap_InetOrg ) {
    ku_add2ops( ops, "sn", user.getSurname().toUtf8() );
    ku_add2ops( ops, "mail", user.getEmail().toUtf8() );
    ku_add2ops( ops, "displayName", user.getFullName().toUtf8() );
    ku_add2ops( ops, "postaladdress", user.getAddress().toUtf8() );
    vals.append( user.getOffice1().toUtf8() );
    vals.append( user.getOffice2().toUtf8() );
    ku_add2ops( ops, "telephoneNumber", vals );
    vals.clear();
  }

  if ( caps & Cap_Samba ) {
    if ( user.getCaps() & KU_User::Cap_Samba ) {
      ku_add2ops( ops, "sambadomainname", user.getDomain().toUtf8() );
      ku_add2ops( ops, "sambauserworkstations", user.getWorkstations().toUtf8() );
      ku_add2ops( ops, "sambahomepath", user.getHomePath().toUtf8() );
      ku_add2ops( ops, "sambahomedrive", user.getHomeDrive().toUtf8() );
      ku_add2ops( ops, "sambalogonscript", user.getLoginScript().toUtf8() );
      ku_add2ops( ops, "sambaprofilepath", user.getProfilePath().toUtf8() );
      ku_add2ops( ops, "sambalmpassword", user.getLMPwd().toUtf8() );
      ku_add2ops( ops, "sambantpassword", user.getNTPwd().toUtf8() );
      ku_add2ops( ops, "sambasid", user.getSID().getSID().toUtf8() );
      ku_add2ops( ops, "sambaacctflags", samflags.toUtf8() );
      ku_add2ops( ops, "sambaprimarygroupsid", user.getPGSID().getSID().toUtf8() );
      ku_add2ops( ops, "sambapwdlastset", QString::number( user.getLastChange() ).toUtf8() );
      if ( user.getExpire() != -1 )
        vals.append( QString::number( user.getExpire() ).toUtf8() );
      ku_add2ops( ops, "sambakickofftime", vals );
      vals.clear();
    } else if (mod) {
      ku_add2ops( ops, "sambadomainname" );
      ku_add2ops( ops, "sambauserworkstations" );
      ku_add2ops( ops, "sambahomepath" );
      ku_add2ops( ops, "sambahomedrive" );
      ku_add2ops( ops, "sambalogonscript" );
      ku_add2ops( ops, "sambaprofilepath" );
      ku_add2ops( ops, "sambalmpassword" );
      ku_add2ops( ops, "sambantpassword" );
      ku_add2ops( ops, "sambasid" );
      ku_add2ops( ops, "sambaacctflags" );
      ku_add2ops( ops, "sambaprimarygroupsid" );
      ku_add2ops( ops, "sambapwdlastset" );
      ku_add2ops( ops, "sambakickofftime" );
      if ( schemaversion > 0 ) {
        ku_add2ops( ops, "sambapasswordhistory" );
        ku_add2ops( ops, "sambalogonhours" );
      }
    }
  }

  if ( caps & Cap_Shadow ) {
    if ( user.getCaps() & KU_User::Cap_POSIX ) {
      ku_add2ops( ops, "shadowlastchange",  QString::number( timeToDays( user.getLastChange() ) ).toUtf8() );
      ku_add2ops( ops, "shadowmin", QString::number( user.getMin() ).toUtf8() );
      ku_add2ops( ops, "shadowmax", QString::number( user.getMax() ).toUtf8() );
      ku_add2ops( ops, "shadowwarning", QString::number( user.getWarn() ).toUtf8() );
      ku_add2ops( ops, "shadowinactive", QString::number( user.getInactive() ).toUtf8() );
      ku_add2ops( ops, "shadowexpire", QString::number( timeToDays( user.getExpire() ) ).toUtf8() );
      ku_add2ops( ops, "shadowflag", QString::number( user.getFlag() ).toUtf8() );
    } else if (mod) {
      ku_add2ops( ops, "shadowlastchange" );
      ku_add2ops( ops, "shadowmin" );
      ku_add2ops( ops, "shadowmax" );
      ku_add2ops( ops, "shadowwarning" );
      ku_add2ops( ops, "shadowinactive" );
      ku_add2ops( ops, "shadowexpire" );
      ku_add2ops( ops, "shadowflag" );
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
      int ret = op.rename_s( KLDAP::LdapDN( oldrdn + ',' + mUrl.dn().toString() ), 
        newrdn, 
        mUrl.dn().toString().toUtf8(),
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
    int ret = op.modify_s( KLDAP::LdapDN( getRDN( it.value() ) + ',' + mUrl.dn().toString() ), ops );
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
    int ret = op.add_s( KLDAP::LdapDN( getRDN( (*it) ) + ',' + mUrl.dn().toString() ), ops );
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
    int ret = op.del_s( KLDAP::LdapDN( getRDN( at((*it)) ) + ',' + mUrl.dn().toString() ) );
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
