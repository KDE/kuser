/*
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
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

#include <qstring.h>

#include <kdebug.h>
#include <kmdcodec.h>
#include <kmessagebox.h>
#include <kio/kntlm.h>

#include "kglobal_.h"
#include "kuserldap.h"
#include "misc.h"
#include "sha1.h"

#include "kuserldap.moc"

KUserLDAP::KUserLDAP(KUserPrefsBase *cfg) : KU::KUsers( cfg )
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

  mUrl.setScope(KABC::LDAPUrl::One);
  mUrl.setExtension("x-dir","base");

  caps = Cap_Passwd | Cap_Disable_POSIX;
  if ( mCfg->ldapshadow() ) caps |= Cap_Shadow;
  if ( mCfg->ldapstructural() ==
    KUserPrefsBase::EnumLdapstructural::inetOrgPerson )
    caps |= Cap_InetOrg;

  if ( mCfg->ldapsam() ) {
    caps |= Cap_Samba;
    domsid = mCfg->samdomsid();
  }

  reload();
}

KUserLDAP::~KUserLDAP()
{
  mUsers.clear();
}

void KUserLDAP::result( KIO::Job *job )
{
  delete mProg;
  mCancel = false;
  if ( job->error() ) {
    QString errstr = job->errorString();
    if ( !errstr.isEmpty() ) {
      if ( ldif.isEmpty() )
        KMessageBox::error( 0, errstr );
      else
        KMessageBox::detailedError( 0, errstr, QString::fromUtf8( ldif, ldif.size()-1 ) );
    }
    mOk = false;
  } else {
    mOk = true;
  }
}

void KUserLDAP::data( KIO::Job *, const QByteArray& data )
{
  if ( data.size() ) {
    mParser.setLDIF( data );
  } else {
    mParser.endLDIF();
  }

  KABC::LDIF::ParseVal ret;
  QString name, val;
  QByteArray value;
  do {
    ret = mParser.nextItem();
    switch ( ret ) {
      case KABC::LDIF::Item:
        name = mParser.attr().lower();
        value = mParser.val();
        val = QString::fromUtf8( value, value.size() );
        if ( name == "objectclass" ) {
          if ( val.lower() == "posixaccount" )
            mUser->setCaps( mUser->getCaps() | KU::KUser::Cap_POSIX );
          else if ( val.lower() == "sambasamaccount" )
            mUser->setCaps( mUser->getCaps() | KU::KUser::Cap_Samba );
	  else if ( val.lower() != "inetorgperson" &&
	            val.lower() != "shadowaccount" &&
                    val.lower() != "account" )
            mOc.append( val );
          
        } else if ( name == "uidnumber" )
          mUser->setUID( val.toLong() );
        else if ( name == "gidnumber" )
          mUser->setGID( val.toLong() );
        else if ( name == "uid" || name == "userid" )
          mUser->setName( val );
        else if ( name == "sn" )
          mUser->setSurname( val );
        else if ( name == "mail" )
          mUser->setEmail( val );
        else if ( name == "homedirectory" )
          mUser->setHomeDir( val );
        else if ( name == "loginshell" )
          mUser->setShell( val );
        else if ( name == "postaladdress" )
          mUser->setAddress( val );
        else if ( name == "telephonenumber" ) {
          if ( mUser->getOffice1().isEmpty() )
            mUser->setOffice1( val );
          else
            mUser->setOffice2( val );
        } else if ( name == "gecos" ) {
          QString name, f1, f2, f3;
          parseGecos( QCString( value.data(), value.size()+1 ), name, f1, f2, f3 );
          if ( mUser->getFullName().isEmpty() ) mUser->setFullName( val );
          if ( mUser->getOffice1().isEmpty() ) mUser->setOffice1( f1 );
          if ( mUser->getOffice2().isEmpty() ) mUser->setOffice2( f1 );
          if ( mUser->getAddress().isEmpty() ) mUser->setAddress( f1 );
        } else if ( name == "cn" ) {
          if ( mUser->getFullName().isEmpty() || mCfg->ldapcnfullname() )
            mUser->setFullName( val );
          if ( mUser->getName().isEmpty() )
            mUser->setName( val );
        } else if ( name == "displayname" ) {
          mUser->setFullName( val );
        } else if ( name == "userpassword" ) {
          if ( !val.isEmpty() ) mUser->setDisabled( false );
          mUser->setPwd( val );
        } else if ( name == "shadowlastchange" ) {
          if ( mUser->getLastChange() == 0 ) //sambapwdlastset is more precise
            mUser->setLastChange( daysToTime( val.toLong() ) );
        } else if ( name == "shadowmin" )
          mUser->setMin( val.toInt() );
        else if ( name == "shadowmax" )
          mUser->setMax( val.toLong() );
        else if ( name == "shadowwarning" )
          mUser->setWarn( val.toLong() );
        else if ( name == "shadowinactive" )
          mUser->setInactive( val.toLong() );
        else if ( name == "shadowexpire" )
          mUser->setExpire( val.toLong() );
        else if ( name == "shadowflag" )
          mUser->setFlag( val.toLong() );
        else if ( name == "sambaacctflags" ) {
          if ( !val.contains( 'D' ) ) mUser->setDisabled( false );
        } else if ( name == "sambasid" )
          mUser->setSID( val );
        else if ( name == "sambaprimarygroupsid" )
          mUser->setPGSID( val );
        else if ( name == "sambalmpassword" )
          mUser->setLMPwd( val );
        else if ( name == "sambantpassword" )
          mUser->setNTPwd( val );
        else if ( name == "sambahomepath" )
          mUser->setHomePath( val );
        else if ( name == "sambahomedrive" )
          mUser->setHomeDrive( val );
        else if ( name == "sambalogonscript" )
          mUser->setLoginScript( val );
        else if ( name == "sambaprofilepath" )
          mUser->setProfilePath( val );
        else if ( name == "sambauserworkstations" )
          mUser->setWorkstations( val );
        else if ( name == "sambadomainname" )
          mUser->setDomain( val );
        else if ( name == "sambapwdlastset" )
          mUser->setLastChange( val.toLong() );
	//these new attributes introduced around samba 3.0.6
	else if ( name == "sambapasswordhistory" || name == "sambalogonhours" ) 
          schemaversion = 1; 
        break;
      case KABC::LDIF::EndEntry: {
        KU::KUser emptyUser, *newUser;
        kdDebug() << "new user: " << mUser->getName() << endl;
        newUser = new KU::KUser( mUser );
        mUsers.append( newUser );
        if ( !mOc.isEmpty() ) {
          mObjectClasses.insert( newUser, mOc );
          kdDebug() << "user: " << newUser->getName() << " other objectclasses: " << mOc.join(",") << endl;
        }
        mOc.clear();
        mUser->copy( &emptyUser );
        mUser->setDisabled( true );

        if ( ( mUsers.count() & 7 ) == 7 ) {
          mProg->progressBar()->advance( mAdv );
          if ( mProg->progressBar()->progress() == 0 ) mAdv = 1;
          if ( mProg->progressBar()->progress() == mProg->progressBar()->totalSteps()-1 ) mAdv = -1;
        }

        break;
      }
      default:
        break;
    }
  } while ( ret != KABC::LDIF::MoreData );
}

bool KUserLDAP::reload()
{
  kdDebug() << "kuserldap::reload()" << endl;
  mObjectClasses.clear();
  mOc.clear();
  mUser = new KU::KUser();
  mUser->setPwd( "" );
  mUser->setSPwd( "" );
  mParser.startParsing();
  mCancel = true;
  mProg = new KProgressDialog( 0, "", "", i18n("Loading Users From LDAP"), true );
  mProg->setAutoClose( false );
  mProg->progressBar()->setFormat("");
  mProg->progressBar()->setTotalSteps( 100 );
  mAdv = 1;
  ldif = "";

  KIO::Job *job = KIO::get( mUrl, true, false );
  connect( job, SIGNAL( data( KIO::Job*, const QByteArray& ) ),
    this, SLOT( data( KIO::Job*, const QByteArray& ) ) );
  connect( job, SIGNAL( result( KIO::Job* ) ),
    this, SLOT( result( KIO::Job* ) ) );
//  job->addMetaData( "SERVER_CTRL0", "1.2.840.113556.1.4.473 true: uidNumber");
  mProg->exec();
  if ( mCancel ) job->kill();
  delete mUser;
  return( mOk );
}

QString KUserLDAP::getRDN(KU::KUser *user)
{
  switch ( mCfg->ldapuserrdn() ) {
    case KUserPrefsBase::EnumLdapuserrdn::uid:
      return "uid=" + user->getName();
    case KUserPrefsBase::EnumLdapuserrdn::uidNumber:
      return "uidNumber=" + QString::number( user->getUID() );
    case KUserPrefsBase::EnumLdapuserrdn::cn: {
      QString cn = mCfg->ldapcnfullname() ? user->getFullName() : user->getName();
      if ( cn.isEmpty() ) cn = user->getName();
      return "cn=" + cn;
    }
  }
  return "";
}

void KUserLDAP::createPassword( KU::KUser *user, const QString &password )
{
  switch ( mCfg->ldappasswordhash() ) {
    case KUserPrefsBase::EnumLdappasswordhash::Clear:
      user->setPwd( password );
      break;
    case KUserPrefsBase::EnumLdappasswordhash::CRYPT:
      user->setPwd( "{CRYPT}" + encryptPass( password, false ) );
      break;
    case KUserPrefsBase::EnumLdappasswordhash::MD5: {
      KMD5 md5( password.utf8() );
      user->setPwd( "{MD5}" + md5.base64Digest() );
      break;
    }
    case KUserPrefsBase::EnumLdappasswordhash::SMD5: {
      QCString salt = genSalt( 4 );
      QCString pwd = password.utf8() + salt;
      KMD5::Digest digest;
      QByteArray hash(20);

      KMD5 md5( pwd );
      md5.rawDigest( digest );
      memcpy( hash.data(), digest, 16 );
      memcpy( &(hash.data()[16]), salt.data(), 4 );
      user->setPwd( "{SMD5}" + KCodecs::base64Encode( hash ) );
      break;
    }
    case KUserPrefsBase::EnumLdappasswordhash::SHA: {
      struct sha1_ctx ctx;
      QByteArray hash(20);

      sha1_init( &ctx );
      sha1_update( &ctx, (const Q_UINT8*) password.utf8().data(),
        password.utf8().length() );
      sha1_final( &ctx, (Q_UINT8*) hash.data() );
      user->setPwd( "{SHA}" + KCodecs::base64Encode( ( hash ) ) );
      break;
    }
    case KUserPrefsBase::EnumLdappasswordhash::SSHA: {
      struct sha1_ctx ctx;
      QByteArray hash(24);
      QCString salt = genSalt( 4 );
      QCString pwd = password.utf8() + salt;

      sha1_init( &ctx );
      sha1_update( &ctx, (const Q_UINT8*) pwd.data(), pwd.length() );
      sha1_final( &ctx, (Q_UINT8*) hash.data() );
      memcpy( &(hash.data()[ 20 ]), salt.data(), 4 );
      user->setPwd( "{SSHA}" + KCodecs::base64Encode( ( hash ) ) );
      break;
    }
  }

  if ( caps & Cap_Samba ) {
    Q_UINT8 hex[33];

    QByteArray ntlmhash;
    ntlmhash = KNTLM::ntlmHash( password );
    unsigned char *hash = (unsigned char*) ntlmhash.data();

    snprintf( (char*) &hex, 33,
      "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
            hash[0], hash[1], hash[2], hash[3], hash[4], hash[5],
            hash[6], hash[7], hash[8], hash[9], hash[10], hash[11],
            hash[12], hash[13], hash[14], hash[15]);

    user->setNTPwd( QString::fromLatin1( (const char*) &hex, 32 ) );

    if ( mCfg->lanmanhash() ) {

      QByteArray lmhash;
      lmhash = KNTLM::lmHash( password );
      unsigned char *hash = (unsigned char*) lmhash.data();
      snprintf( (char*) &hex, 33,
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
            hash[0], hash[1], hash[2], hash[3], hash[4], hash[5],
            hash[6], hash[7], hash[8], hash[9], hash[10], hash[11],
            hash[12], hash[13], hash[14], hash[15]);

      user->setLMPwd( QString::fromLatin1( (const char*) &hex, 32 ) );
    } else {
      user->setLMPwd( "" );
    }
  }
}

void KUserLDAP::getLDIF( KU::KUser *user, bool mod )
{
  QString gecos, cn, pwd, samflags;
  ldif.resize( 0 );

  pwd = user->getPwd();
  if ( user->getDisabled() ) pwd = "";

  cn = mCfg->ldapcnfullname() ? user->getFullName() : user->getName();
  if ( cn.isEmpty() ) cn = user->getName();

  gecos = QString::fromLatin1("%1,%2,%3,%4")
    .arg(user->getFullName())
    .arg(user->getOffice1())
    .arg(user->getOffice2())
    .arg(user->getAddress());

  samflags = "[U";
  samflags += user->getDisabled() ? 'D' : ' ';
  samflags += "         ]";

  ldif = "";

  if ( mod ) {
    QString oldrdn = getRDN( mUser );
    QString newrdn = getRDN( user );

    if ( oldrdn != newrdn ) {
      ldif = "dn: " + oldrdn.utf8() + "," + mUrl.dn().utf8() + "\n" +
        "changetype: modrdn\n" +
        "newrdn: " + newrdn.utf8() + "\n" +
        "deleteoldrdn: 1\n\n";
    }
  }

  ldif += "dn: " + getRDN( user ).utf8() + "," + mUrl.dn().utf8() + "\n";
  if ( mod ) {
    ldif += "changetype: modify\n";
    ldif += "replace: objectClass\n";
  }

  if ( caps & Cap_InetOrg )
    ldif += "objectClass: inetOrgPerson\n";
  else
    ldif += "objectClass: account\n";

  if ( user->getCaps() & KU::KUser::Cap_POSIX ) {
    ldif += "objectClass: posixAccount\n";
  }
  if ( ( caps & Cap_Shadow ) && ( user->getCaps() & KU::KUser::Cap_POSIX ) ) {
    ldif += "objectClass: shadowAccount\n";
  }
  if ( ( caps & Cap_Samba ) && ( user->getCaps() & KU::KUser::Cap_Samba ) ) {
    ldif += "objectClass: sambaSamAccount\n";
  }
  if ( mod && mObjectClasses.contains( mUser ) ) {
    QStringList ocs = mObjectClasses[ mUser ];
    kdDebug() << user->getName() << " has additional objectclasses: " << ocs.join(",") << endl;
    QValueListIterator<QString> it;
    for ( it = ocs.begin(); it != ocs.end(); ++it ) {
      ldif += "objectClass: ";
      ldif += (*it).utf8();
      ldif += "\n";
    }
  }

  if ( mod ) ldif += "-\nreplace: cn\n";
  ldif += KABC::LDIF::assembleLine( "cn", cn )+"\n";
  if ( caps & Cap_InetOrg ) {
    if ( mod ) ldif += "-\nreplace: uid\n";
    ldif += KABC::LDIF::assembleLine( "uid", user->getName() ) + "\n";
  } else {
    if ( mod ) ldif += "-\nreplace: userid\n";
    ldif += KABC::LDIF::assembleLine( "userid", user->getName() ) + "\n";
  }
  if ( mod ) ldif += "-\n";

  if ( ( user->getCaps() & KU::KUser::Cap_POSIX ) || ( caps & Cap_InetOrg ) ) {
    if ( mod ) ldif += "replace: userpassword\n";
    ldif += KABC::LDIF::assembleLine( "userpassword", pwd )+"\n";
    if ( mod ) ldif += "-\n";
  }

  if ( user->getCaps() & KU::KUser::Cap_POSIX ) {
    if ( mod ) ldif += "replace: uidnumber\n";
    ldif += KABC::LDIF::assembleLine( "uidnumber",
      QString::number( user->getUID() ) )+"\n";
    if ( mod ) ldif += "-\nreplace: gidnumber\n";
    ldif += KABC::LDIF::assembleLine( "gidnumber",
      QString::number( user->getGID() ) )+"\n";
    if ( mod ) ldif += "-\nreplace: gecos\n";
    ldif += KABC::LDIF::assembleLine( "gecos", !mCfg->ldapgecos() ? QCString() :
      QCString( gecos.latin1() ) )+"\n";
    if ( mod ) ldif += "-\nreplace: homedirectory\n";
    ldif += KABC::LDIF::assembleLine( "homedirectory",
      user->getHomeDir() )+"\n";
    if ( mod ) ldif += "-\nreplace: loginshell\n";
    ldif += KABC::LDIF::assembleLine( "loginshell",
      user->getShell() )+"\n";
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
    ldif += KABC::LDIF::assembleLine( "sn", user->getSurname() ) + "\n";
    if ( mod ) ldif += "-\nreplace: mail\n";
    ldif += KABC::LDIF::assembleLine( "mail", user->getEmail() ) + "\n";
    if ( mod ) ldif += "-\nreplace: displayName\n";
    ldif += KABC::LDIF::assembleLine( "displayname", user->getFullName() ) + "\n";
    if ( mod ) ldif += "-\nreplace: postaladdress\n";
    ldif += KABC::LDIF::assembleLine( "postaladdress", user->getAddress() ) + "\n";
    if ( mod ) ldif += "-\nreplace: telephoneNumber\n";
    ldif += KABC::LDIF::assembleLine( "telephoneNumber", user->getOffice1() ) + "\n";
    ldif += KABC::LDIF::assembleLine( "telephoneNumber", user->getOffice2() ) + "\n";
    if ( mod ) ldif += "-\n";
  }

  if ( caps & Cap_Samba ) {
    if ( user->getCaps() & KU::KUser::Cap_Samba ) {
      if ( mod ) ldif += "replace: sambadomainname\n";
      ldif += KABC::LDIF::assembleLine( "sambadomainname", user->getDomain() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambauserworkstations\n";
      ldif += KABC::LDIF::assembleLine( "sambauserworkstations", user->getWorkstations() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambahomepath\n";
      ldif += KABC::LDIF::assembleLine( "sambahomepath", user->getHomePath() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambahomedrive\n";
      ldif += KABC::LDIF::assembleLine( "sambahomedrive", user->getHomeDrive() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambalogonscript\n";
      ldif += KABC::LDIF::assembleLine( "sambalogonscript", user->getLoginScript() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambaprofilepath\n";
      ldif += KABC::LDIF::assembleLine( "sambaprofilepath", user->getProfilePath() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambalmpassword\n";
      ldif += KABC::LDIF::assembleLine( "sambalmpassword", user->getLMPwd() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambantpassword\n";
      ldif += KABC::LDIF::assembleLine( "sambantpassword", user->getNTPwd() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambasid\n";
      ldif += KABC::LDIF::assembleLine( "sambasid", user->getSID().getSID() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambaacctflags\n";
      ldif += KABC::LDIF::assembleLine( "sambaacctflags", samflags ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambaprimarygroupsid\n";
      ldif += KABC::LDIF::assembleLine( "sambaprimarygroupsid",
        user->getPGSID().getSID() ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambapwdlastset\n";
      ldif += KABC::LDIF::assembleLine( "sambapwdlastset",
        QString::number( user->getLastChange() ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: sambakickofftime\n";
      if ( user->getExpire() != -1 ) ldif +=
        KABC::LDIF::assembleLine( "sambakickofftime",
        QString::number( user->getExpire() ) ) + "\n";
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
    if ( user->getCaps() & KU::KUser::Cap_POSIX ) {
      if ( mod ) ldif += "replace: shadowlastchange\n"; //sambapwdlastset
      ldif += KABC::LDIF::assembleLine( "shadowlastchange",
        QString::number( timeToDays( user->getLastChange() ) ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: shadowmin\n"; //sambaPwdCanChange
      ldif += KABC::LDIF::assembleLine( "shadowmin",
        QString::number( user->getMin() ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: shadowmax\n"; //sambaPwdMustChange
      ldif += KABC::LDIF::assembleLine( "shadowmax",
        QString::number( user->getMax() ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: shadowwarning\n";
      ldif += KABC::LDIF::assembleLine( "shadowwarning",
        QString::number( user->getWarn() ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: shadowinactive\n";
      ldif += KABC::LDIF::assembleLine( "shadowinactive",
        QString::number( user->getInactive() ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: shadowexpire\n"; //sambaKickoffTime
      ldif += KABC::LDIF::assembleLine( "shadowexpire",
        QString::number( timeToDays( user->getExpire() ) ) ) + "\n";
      if ( mod ) ldif += "-\nreplace: shadowflag\n";
      ldif += KABC::LDIF::assembleLine( "shadowflag",
        QString::number( user->getFlag() ) ) + "\n";
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
//  kdDebug() << "ldif: " << ldif << endl;
}

void KUserLDAP::delData( KU::KUser *user )
{
  ldif = "dn: " + getRDN( user ).utf8() + "," + mUrl.dn().utf8() + "\n" +
    "changetype: delete\n\n";
}

bool KUserLDAP::dbcommit()
{
  mAddSucc.clear();
  mDelSucc.clear();
  mModSucc.clear();
  mAdd.first();
  mDel.first();
  mAddUser = 0; mDelUser = 0; mUser = 0;

  mProg = new KProgressDialog( 0, "", i18n("LDAP Operation"), "", true );
  KIO::Job *job = KIO::put( mUrl, -1, false, false, false );
  connect( job, SIGNAL( dataReq( KIO::Job*, QByteArray& ) ),
    this, SLOT( putData( KIO::Job*, QByteArray& ) ) );
  connect( job, SIGNAL( result( KIO::Job* ) ),
    this, SLOT( result( KIO::Job* ) ) );
  mProg->exec();
  return( mOk );
}

void KUserLDAP::putData( KIO::Job *, QByteArray& data )
{
  ModIt mit = mMod.begin();

  if ( mAddUser ) {
    mAddSucc.append( mAddUser );
    mAdd.remove();
    mAddUser = 0;
  }
  if ( mDelUser ) {
    kdDebug() << "delete ok for: " << mDelUser->getName() << endl;
    mDelSucc.append( mDelUser );
    if ( mObjectClasses.contains( mDelUser ) ) {
      kdDebug() << "deleting additonal objectclasses!" << endl;
      mObjectClasses.remove( mDelUser );
    }
    mDel.remove();
    mDelUser = 0;
  }
  if ( mUser ) {
    mModSucc.insert( mUser, mit.data() );
    mMod.remove( mit );
    mit = mMod.begin();
    mUser = 0;
  }

  if ( (mAddUser = mAdd.current()) ) {
    getLDIF( mAddUser, false );
    data = ldif;
  } else if ( mit != mMod.end() ) {
    mUser = mit.key();
    getLDIF( &(mit.data()), true );
    data = ldif;
  } else if ( (mDelUser = mDel.current()) ) {
    kdDebug() << "deleting: " << mDelUser->getName() << endl;
    delData( mDelUser );
    data = ldif;
  } else
    data.resize(0);
}
