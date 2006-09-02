/*
 *  Copyright (c) 2004 Szombathelyi GyĂśrgy <gyurco@freemail.hu>
 *
 *  This mProgram is free software; you can redistribute it and/or
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

#include <QByteArray>
#include <QLabel>

#include <kdebug.h>
#include <klocale.h>
#include <kcodecs.h>
#include <kio/kntlm.h>

#include "ku_userldap.h"
#include "ku_misc.h"
#include "sha1.h"

KU_UserLDAP::KU_UserLDAP(KU_PrefsBase *cfg) : KU_Users( cfg )
{
  schemaversion = 0;

  if ( mCfg->ldapssl() )
    mUrl.setProtocol("ldaps");
  else
    mUrl.setProtocol("ldap");

  mUrl.setHost( mCfg->ldaphost() );
  mUrl.setPort( mCfg->ldapport() );
  mUrl.setDn( mCfg->ldapuserbase() + "," + mCfg->ldapdn() );
  if ( !mCfg->ldapanon() ) {
    mUrl.setUser( mCfg->ldapuser() );
    mUrl.setPass( mCfg->ldappassword() );
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
  if ( mCfg->ldapstructural() ==
    KU_PrefsBase::EnumLdapstructural::inetOrgPerson )
    caps |= Cap_InetOrg;

  if ( mCfg->ldapsam() ) {
    caps |= Cap_Samba;
    domsid = mCfg->samdomsid();
  }
}

KU_UserLDAP::~KU_UserLDAP()
{
}

void KU_UserLDAP::result( KJob *job )
{
  kDebug() << "LDAP result: " << job->error() << endl;
  mProg->hide();
  if ( job->error() ) {
    QString errstr = job->errorString();
    if ( !errstr.isEmpty() ) {
      mErrorString = errstr;
//      mErrorDetails = ldif;
    } else {
      mErrorString = i18n("Unknown error"); //this is better than nothing (?)
    }
    mOk = false;
  } else {
    mOk = true;
  }
}

void KU_UserLDAP::data( KIO::Job *, const QByteArray& data )
{
  if ( data.size() ) {
    mParser.setLdif( data );
  } else {
    mParser.endLdif();
  }

  KLDAP::Ldif::ParseValue ret;
  QString name, val;
  QByteArray value;
  do {
    ret = mParser.nextItem();
    switch ( ret ) {
      case KLDAP::Ldif::Item:
        name = mParser.attr().toLower();
        value = mParser.value();
        val = QString::fromUtf8( value, value.size() );
        if ( name == "objectclass" ) {
          if ( val.toLower() == "posixaccount" )
            mUser.setCaps( mUser.getCaps() | KU_User::Cap_POSIX );
          else if ( val.toLower() == "sambasamaccount" )
            mUser.setCaps( mUser.getCaps() | KU_User::Cap_Samba );
	  else if ( val.toLower() != "inetorgperson" &&
	            val.toLower() != "shadowaccount" &&
                    val.toLower() != "account" )
            mOc.append( val );

        } else if ( name == "uidnumber" )
          mUser.setUID( val.toLong() );
        else if ( name == "gidnumber" )
          mUser.setGID( val.toLong() );
        else if ( name == "uid" || name == "userid" )
          mUser.setName( val );
        else if ( name == "sn" )
          mUser.setSurname( val );
        else if ( name == "mail" )
          mUser.setEmail( val );
        else if ( name == "homedirectory" )
          mUser.setHomeDir( val );
        else if ( name == "loginshell" )
          mUser.setShell( val );
        else if ( name == "postaladdress" )
          mUser.setAddress( val );
        else if ( name == "telephonenumber" ) {
          if ( mUser.getOffice1().isEmpty() )
            mUser.setOffice1( val );
          else
            mUser.setOffice2( val );
        } else if ( name == "gecos" ) {
          QString name, f1, f2, f3;
          parseGecos( QByteArray( value.data(), value.size()+1 ), name, f1, f2, f3 );
          if ( mUser.getFullName().isEmpty() ) mUser.setFullName( val );
          if ( mUser.getOffice1().isEmpty() ) mUser.setOffice1( f1 );
          if ( mUser.getOffice2().isEmpty() ) mUser.setOffice2( f1 );
          if ( mUser.getAddress().isEmpty() ) mUser.setAddress( f1 );
        } else if ( name == "cn" ) {
          if ( mUser.getFullName().isEmpty() || mCfg->ldapcnfullname() )
            mUser.setFullName( val );
          if ( mUser.getName().isEmpty() )
            mUser.setName( val );
        } else if ( name == "displayname" ) {
          mUser.setFullName( val );
        } else if ( name == "userpassword" ) {
          if ( !val.isEmpty() ) mUser.setDisabled( false );
          mUser.setPwd( val );
        } else if ( name == "shadowlastchange" ) {
          if ( mUser.getLastChange() == 0 ) //sambapwdlastset is more precise
            mUser.setLastChange( daysToTime( val.toLong() ) );
        } else if ( name == "shadowmin" )
          mUser.setMin( val.toInt() );
        else if ( name == "shadowmax" )
          mUser.setMax( val.toLong() );
        else if ( name == "shadowwarning" )
          mUser.setWarn( val.toLong() );
        else if ( name == "shadowinactive" )
          mUser.setInactive( val.toLong() );
        else if ( name == "shadowexpire" )
          mUser.setExpire( val.toLong() );
        else if ( name == "shadowflag" )
          mUser.setFlag( val.toLong() );
        else if ( name == "sambaacctflags" ) {
          if ( !val.contains( 'D' ) ) mUser.setDisabled( false );
        } else if ( name == "sambasid" )
          mUser.setSID( val );
        else if ( name == "sambaprimarygroupsid" )
          mUser.setPGSID( val );
        else if ( name == "sambalmpassword" )
          mUser.setLMPwd( val );
        else if ( name == "sambantpassword" )
          mUser.setNTPwd( val );
        else if ( name == "sambahomepath" )
          mUser.setHomePath( val );
        else if ( name == "sambahomedrive" )
          mUser.setHomeDrive( val );
        else if ( name == "sambalogonscript" )
          mUser.setLoginScript( val );
        else if ( name == "sambaprofilepath" )
          mUser.setProfilePath( val );
        else if ( name == "sambauserworkstations" )
          mUser.setWorkstations( val );
        else if ( name == "sambadomainname" )
          mUser.setDomain( val );
        else if ( name == "sambapwdlastset" )
          mUser.setLastChange( val.toLong() );
	//these new attributes introduced around samba 3.0.6
	else if ( name == "sambapasswordhistory" || name == "sambalogonhours" )
          schemaversion = 1;
        break;
      case KLDAP::Ldif::EndEntry: {
        kDebug() << "new user: " << mUser.getName() << endl;
        if ( !mOc.isEmpty() ) {
          mObjectClasses.insert( count(), mOc );
          kDebug() << "user: " << mUser.getName() << " other objectclasses: " << mOc.join(",") << endl;
        }
        mOc.clear();
        append( mUser );
        mUser = KU_User();
        mUser.setDisabled( true );

        if ( ( count() & 7 ) == 7 ) {
          mProg->setValue( mProg->value() + mAdv );
          if ( mProg->value() == 0 ) mAdv = 1;
          if ( mProg->value() == mProg->maximum()-1 ) mAdv = -1;
        }

        break;
      }
      default:
        break;
    }
  } while ( ret != KLDAP::Ldif::MoreData );
}

bool KU_UserLDAP::reload()
{
  kDebug() << "kuserldap::reload()" << endl;
  mErrorString = mErrorDetails = QString();
  mObjectClasses.clear();
  mOc.clear();
  mUser = KU_User();
  mUser.setPwd( "" );
  mUser.setSPwd( "" );
  mParser.startParsing();
  mProg = new QProgressDialog( 0 );
  mProg->setLabel( new QLabel( i18n("Loading Users From LDAP") ) );
  mProg->setAutoClose( false );
  mProg->setAutoReset( false );
  mProg->setMaximum( 100 );
  mAdv = 1;

  KIO::Job *job = KIO::get( mUrl, true, false );
  connect( job, SIGNAL( data( KIO::Job*, const QByteArray& ) ),
    this, SLOT( data( KIO::Job*, const QByteArray& ) ) );
  connect( job, SIGNAL( result( KJob* ) ),
    this, SLOT( result( KJob* ) ) );
//  job->addMetaData( "SERVER_CTRL0", "1.2.840.113556.1.4.473 true: uidNumber");
  mProg->exec();
  if ( mProg->wasCanceled() ) job->kill();
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
      KMD5 md5( password.toUtf8() );
      user.setPwd( "{MD5}" + md5.base64Digest() );
      break;
    }
    case KU_PrefsBase::EnumLdappasswordhash::SMD5: {
      QByteArray salt = genSalt( 4 );
      QByteArray pwd = password.toUtf8() + salt;
      KMD5::Digest digest;
      QByteArray hash(20, 0);

      KMD5 md5( pwd );
      md5.rawDigest( digest );
      memcpy( hash.data(), digest, 16 );
      memcpy( &(hash.data()[16]), salt.data(), 4 );
      user.setPwd( "{SMD5}" + KCodecs::base64Encode( hash ) );
      break;
    }
    case KU_PrefsBase::EnumLdappasswordhash::SHA: {
      struct sha1_ctx ctx;
      QByteArray hash(20, 0);

      sha1_init( &ctx );
      sha1_update( &ctx, (const quint8*) password.toUtf8().data(),
        password.toUtf8().length() );
      sha1_final( &ctx, (quint8*) hash.data() );
      user.setPwd( "{SHA}" + KCodecs::base64Encode( ( hash ) ) );
      break;
    }
    case KU_PrefsBase::EnumLdappasswordhash::SSHA: {
      struct sha1_ctx ctx;
      QByteArray hash(24, 0);
      QByteArray salt = genSalt( 4 );
      QByteArray pwd = password.toUtf8() + salt;

      sha1_init( &ctx );
      sha1_update( &ctx, (const quint8*) pwd.data(), pwd.length() );
      sha1_final( &ctx, (quint8*) hash.data() );
      memcpy( &(hash.data()[ 20 ]), salt.data(), 4 );
      user.setPwd( "{SSHA}" + KCodecs::base64Encode( ( hash ) ) );
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

QByteArray KU_UserLDAP::getLDIF( const KU_User &user, int oldindex ) const
{
  QString gecos, cn, pwd, samflags;
  QByteArray ldif;
  
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

  ldif = "";

  if ( mod ) {
    QString oldrdn = getRDN( at( oldindex ) );
    QString newrdn = getRDN( user );

    if ( oldrdn != newrdn ) {
      ldif = "dn: " + oldrdn.toUtf8() + "," + mUrl.dn().toUtf8() + "\n" +
        "changetype: modrdn\n" +
        "newrdn: " + newrdn.toUtf8() + "\n" +
        "deleteoldrdn: 1\n\n";
    }
  }

  ldif += "dn: " + getRDN( user ).toUtf8() + "," + mUrl.dn().toUtf8() + "\n";
  if ( oldindex != -1 ) {
    ldif += "changetype: modify\n";
    ldif += "replace: objectClass\n";
  }

  if ( caps & Cap_InetOrg )
    ldif += "objectClass: inetOrgPerson\n";
  else
    ldif += "objectClass: account\n";

  if ( user.getCaps() & KU_User::Cap_POSIX ) {
    ldif += "objectClass: posixAccount\n";
  }
  if ( ( caps & Cap_Shadow ) && ( user.getCaps() & KU_User::Cap_POSIX ) ) {
    ldif += "objectClass: shadowAccount\n";
  }
  if ( ( caps & Cap_Samba ) && ( user.getCaps() & KU_User::Cap_Samba ) ) {
    ldif += "objectClass: sambaSamAccount\n";
  }

  if ( mod && mObjectClasses.contains( oldindex ) ) {
    QStringList ocs = mObjectClasses[ oldindex ];
    kDebug() << user.getName() << " has additional objectclasses: " << ocs.join(",") << endl;
    QStringList::iterator it;
    for ( it = ocs.begin(); it != ocs.end(); ++it ) {
      ldif += "objectClass: ";
      ldif += (*it).toUtf8();
      ldif += "\n";
    }
  }

  if ( mod ) ldif += "-\nreplace: cn\n";
  ldif += KLDAP::Ldif::assembleLine( "cn", cn )+"\n";
  if ( caps & Cap_InetOrg ) {
    if ( mod ) ldif += "-\nreplace: uid\n";
    ldif += KLDAP::Ldif::assembleLine( "uid", user.getName() ) + "\n";
  } else {
    if ( mod ) ldif += "-\nreplace: userid\n";
    ldif += KLDAP::Ldif::assembleLine( "userid", user.getName() ) + "\n";
  }
  if ( mod ) ldif += "-\n";

  if ( ( user.getCaps() & KU_User::Cap_POSIX ) || ( caps & Cap_InetOrg ) ) {
    if ( mod ) ldif += "replace: userpassword\n";
    ldif += KLDAP::Ldif::assembleLine( "userpassword", pwd )+"\n";
    if ( mod ) ldif += "-\n";
  }

  if ( user.getCaps() & KU_User::Cap_POSIX ) {
    if ( mod ) ldif += "replace: uidnumber\n";
    ldif += KLDAP::Ldif::assembleLine( "uidnumber",
      QString::number( user.getUID() ) )+"\n";
    if ( mod ) ldif += "-\nreplace: gidnumber\n";
    ldif += KLDAP::Ldif::assembleLine( "gidnumber",
      QString::number( user.getGID() ) )+"\n";
    if ( mod ) ldif += "-\nreplace: gecos\n";
    ldif += KLDAP::Ldif::assembleLine( "gecos", !mCfg->ldapgecos() ? QByteArray() :
      QByteArray( gecos.toLatin1() ) )+"\n";
    if ( mod ) ldif += "-\nreplace: homedirectory\n";
    ldif += KLDAP::Ldif::assembleLine( "homedirectory",
      user.getHomeDir() )+"\n";
    if ( mod ) ldif += "-\nreplace: loginshell\n";
    ldif += KLDAP::Ldif::assembleLine( "loginshell",
      user.getShell() )+"\n";
    if ( mod ) ldif += "-\n";
  } else {
    if ( mod ) {
      ldif += "replace: uidnumber\n";
      ldif += "-\nreplace: gidnumber\n";
      ldif += "-\nreplace: homedirectory\n";
      ldif += "-\nreplace: loginshell\n";
      ldif += "-\nreplace: gecos\n";
      ldif += "-\n";
    }
  }

  if ( caps & Cap_InetOrg ) {
    if ( mod ) ldif += "replace: sn\n";
    ldif += KLDAP::Ldif::assembleLine( "sn", user.getSurname() ) + "\n";
    if ( mod ) ldif += "-\nreplace: mail\n";
    ldif += KLDAP::Ldif::assembleLine( "mail", user.getEmail() ) + "\n";
    if ( mod ) ldif += "-\nreplace: displayName\n";
    ldif += KLDAP::Ldif::assembleLine( "displayname", user.getFullName() ) + "\n";
    if ( mod ) ldif += "-\nreplace: postaladdress\n";
    ldif += KLDAP::Ldif::assembleLine( "postaladdress", user.getAddress() ) + "\n";
    if ( mod ) ldif += "-\nreplace: telephoneNumber\n";
    ldif += KLDAP::Ldif::assembleLine( "telephoneNumber", user.getOffice1() ) + "\n";
    ldif += KLDAP::Ldif::assembleLine( "telephoneNumber", user.getOffice2() ) + "\n";
    if ( mod ) ldif += "-\n";
  }

  if ( caps & Cap_Samba ) {
    if ( user.getCaps() & KU_User::Cap_Samba ) {
      if ( mod ) ldif += "replace: sambadomainname\n";
      ldif += KLDAP::Ldif::assembleLine( "sambadomainname", user.getDomain() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambauserworkstations\n";
      ldif += KLDAP::Ldif::assembleLine( "sambauserworkstations", user.getWorkstations() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambahomepath\n";
      ldif += KLDAP::Ldif::assembleLine( "sambahomepath", user.getHomePath() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambahomedrive\n";
      ldif += KLDAP::Ldif::assembleLine( "sambahomedrive", user.getHomeDrive() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambalogonscript\n";
      ldif += KLDAP::Ldif::assembleLine( "sambalogonscript", user.getLoginScript() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambaprofilepath\n";
      ldif += KLDAP::Ldif::assembleLine( "sambaprofilepath", user.getProfilePath() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambalmpassword\n";
      ldif += KLDAP::Ldif::assembleLine( "sambalmpassword", user.getLMPwd() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambantpassword\n";
      ldif += KLDAP::Ldif::assembleLine( "sambantpassword", user.getNTPwd() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambasid\n";
      ldif += KLDAP::Ldif::assembleLine( "sambasid", user.getSID().getSID() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambaacctflags\n";
      ldif += KLDAP::Ldif::assembleLine( "sambaacctflags", samflags ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambaprimarygroupsid\n";
      ldif += KLDAP::Ldif::assembleLine( "sambaprimarygroupsid",
        user.getPGSID().getSID() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambapwdlastset\n";
      ldif += KLDAP::Ldif::assembleLine( "sambapwdlastset",
        QString::number( user.getLastChange() ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambakickofftime\n";
      if ( user.getExpire() != -1 ) ldif +=
        KLDAP::Ldif::assembleLine( "sambakickofftime",
        QString::number( user.getExpire() ) ) + "\n";
      if ( mod ) ldif += "-\n";
    } else {
      if ( mod ) {
        ldif += "replace: sambahomepath\n";
        ldif += "-\nreplace: sambahomedrive\n";
        ldif += "-\nreplace: sambalogonscript\n";
        ldif += "-\nreplace: sambaprofilepath\n";
        ldif += "-\nreplace: sambalmpassword\n";
        ldif += "-\nreplace: sambantpassword\n";
        ldif += "-\nreplace: sambasid\n";
        ldif += "-\nreplace: sambaacctflags\n";
        ldif += "-\nreplace: sambaprimarygroupsid\n";
        ldif += "-\nreplace: sambapwdlastset\n";
        ldif += "-\nreplace: sambakickofftime\n";
        ldif += "-\nreplace: sambalogontime\n";
        ldif += "-\nreplace: sambalogofftime\n";
        ldif += "-\nreplace: sambapwdcanchange\n";
        ldif += "-\nreplace: sambapwdmustchange\n";
        ldif += "-\nreplace: sambauserworkstations\n";
        ldif += "-\nreplace: sambadomainname\n";
        ldif += "-\nreplace: sambamungeddial\n";
        ldif += "-\nreplace: sambabadpasswordcount\n";
        ldif += "-\nreplace: sambabadpasswordtime\n";
        ldif += "-\nreplace: sambadomainname\n";
        if ( schemaversion > 0 ) {
          ldif += "-\nreplace: sambapasswordhistory\n";
          ldif += "-\nreplace: sambalogonhours\n";
        }
        ldif += "-\n";
      }
    }
  }

  if ( caps & Cap_Shadow ) {
    if ( user.getCaps() & KU_User::Cap_POSIX ) {
      if ( mod ) ldif += "replace: shadowlastchange\n"; //sambapwdlastset
      ldif += KLDAP::Ldif::assembleLine( "shadowlastchange",
        QString::number( timeToDays( user.getLastChange() ) ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: shadowmin\n"; //sambaPwdCanChange
      ldif += KLDAP::Ldif::assembleLine( "shadowmin",
        QString::number( user.getMin() ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: shadowmax\n"; //sambaPwdMustChange
      ldif += KLDAP::Ldif::assembleLine( "shadowmax",
        QString::number( user.getMax() ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: shadowwarning\n";
      ldif += KLDAP::Ldif::assembleLine( "shadowwarning",
        QString::number( user.getWarn() ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: shadowinactive\n";
      ldif += KLDAP::Ldif::assembleLine( "shadowinactive",
        QString::number( user.getInactive() ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: shadowexpire\n"; //sambaKickoffTime
      ldif += KLDAP::Ldif::assembleLine( "shadowexpire",
        QString::number( timeToDays( user.getExpire() ) ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: shadowflag\n";
      ldif += KLDAP::Ldif::assembleLine( "shadowflag",
        QString::number( user.getFlag() ) ) + "\n";
      if ( mod ) ldif += "-\n";
    } else {
      if ( mod ) {
        ldif += "replace: shadowlastchange\n";
        ldif += "-\nreplace: shadowmin\n";
        ldif += "-\nreplace: shadowmax\n";
        ldif += "-\nreplace: shadowwarning\n";
        ldif += "-\nreplace: shadowinactive\n";
        ldif += "-\nreplace: shadowexpire\n";
        ldif += "-\nreplace: shadowflag\n";
        ldif += "-\n";
      }
    }
  }
  ldif += "\n";
  kDebug() << "ldif: " << QString::fromUtf8(ldif) << endl;
  return ldif;
}

QByteArray KU_UserLDAP::delData( const KU_User &user ) const
{
  QByteArray ldif = "dn: " + getRDN( user ).toUtf8() + "," + mUrl.dn().toUtf8() + "\n" +
    "changetype: delete\n\n";
  return ldif;
}

bool KU_UserLDAP::dbcommit()
{
  mAddSucc.clear();
  mDelSucc.clear();
  mModSucc.clear();
  mAddIndex = 0;
  mDelIndex = 0;
  mModIt = mMod.begin();
  mLastOperation = None;
  mErrorString = mErrorDetails = QString();

  mProg = new QProgressDialog( 0 );
  mProg->setLabel( new QLabel(i18n("LDAP Operation") ) );
  mProg->setAutoClose( false );
  mProg->setAutoReset( false );
  mProg->setMaximum( mAdd.count() + mDel.count() + mMod.count() );
  KIO::Job *job = KIO::put( mUrl, -1, false, false, false );
  connect( job, SIGNAL( dataReq( KIO::Job*, QByteArray& ) ),
    this, SLOT( putData( KIO::Job*, QByteArray& ) ) );
  connect( job, SIGNAL( result( KJob* ) ),
    this, SLOT( result( KJob* ) ) );
  mProg->exec();
  delete mProg;
  return( mOk );
}

void KU_UserLDAP::putData( KIO::Job *, QByteArray& data )
{
  switch ( mLastOperation ) {
    case Mod:
      mModSucc.insert( mModIt.key(), mModIt.value() );
      mModIt++;
      break;
    case Add:
      mAddSucc.append( mAdd.at( mAddIndex ) );
      mAddIndex++;
      break;
    case Del: {
      int deleteindex = mDel.at( mDelIndex );
      
      if ( mObjectClasses.contains( deleteindex ) ) {
        kDebug() << "deleting additonal objectclasses!" << endl;
        mObjectClasses.remove( deleteindex );
      }
      mDelSucc.append( deleteindex );
      mDelIndex++;
      break;
    }
    default:
      break;
  }

  if ( mModIt != mMod.end() ) {
    data = getLDIF( mModIt.value(), mModIt.key() );
    mLastOperation = Mod;
  } else if ( mDelIndex < mDel.count() ) {
    data = delData( at(mDel.at( mDelIndex ) ));
    mLastOperation = Del;
  } else if ( mAddIndex < mAdd.count() ) {
    data = getLDIF( mAdd.at( mAddIndex ), -1 );
    mLastOperation = Add;
  } else {
    data.resize( 0 );
  }
  mProg->setValue( mProg->value() + 1 );
}

#include "ku_userldap.moc"
