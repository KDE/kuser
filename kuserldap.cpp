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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include <qstring.h>

#include <kdebug.h>
#include <kmdcodec.h>
#include <kmessagebox.h>

#include "kglobal_.h"
#include "kuserldap.h"
#include "misc.h"
#include "sha1.h"
#include "md4.h"

#include "kuserldap.moc"


KUserLDAP::KUserLDAP(KUserPrefsBase *cfg) : KUsers( cfg )
{
  if ( mCfg->ldapssl() )
    mUrl.setProtocol("ldaps");
  else
    mUrl.setProtocol("ldap");
      
  mUrl.setHost( mCfg->ldaphost() );
  mUrl.setPort( mCfg->ldapport() );
  mUrl.setDn( mCfg->ldapuserbase() + "," + mCfg->ldapdn() );
  mUrl.setUser( mCfg->ldapuser() );
  mUrl.setPass( mCfg->ldappassword() );
  mUrl.setFilter( mCfg->ldapfilter() );

  if ( mCfg->ldaptls() ) mUrl.setExtension( "x-tls", "" );
  if ( mCfg->ldapsasl() ) {
    mUrl.setExtension( "x-sasl", "" );
    mUrl.setExtension( "x-mech", mCfg->ldapsaslmech() );
  }
    
  mUrl.setScope(KABC::LDAPUrl::One);
  mUrl.setExtension("x-dir","base");
  
  caps = Cap_Passwd;
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

void KUserLDAP::data( KIO::Job *job, const QByteArray& data )
{
  if ( data.size() ) {
    mParser.setLDIF( data );
  } else {
    QByteArray dummy( 3 );
    dummy[ 0 ] = '\n';
    dummy[ 1 ] = '\n';
    dummy[ 2 ] = '\n';
    mParser.setLDIF( dummy );
  }

  KABC::LDIF::ParseVal ret;
  QString name;
  QByteArray value;
  do {
    ret = mParser.nextItem();
    switch ( ret ) {
      case KABC::LDIF::Item:
        name = mParser.attr().lower();
        value = mParser.val();
        if ( name == "uidnumber" ) 
          mUser->setUID( QString::fromUtf8( value, value.size() ).toLong() );
        else if ( name == "gidnumber" )
          mUser->setGID( QString::fromUtf8( value, value.size() ).toLong() );
        else if ( name == "uid" )
          mUser->setName(QString::fromUtf8( value, value.size() ));
        else if ( name == "sn" )
          mUser->setSurname(QString::fromUtf8( value, value.size() ));
        else if ( name == "mail" )
          mUser->setEmail(QString::fromUtf8( value, value.size() ));
        else if ( name == "homedirectory" )
          mUser->setHomeDir(QString::fromUtf8( value, value.size() ));
        else if ( name == "loginshell" )
          mUser->setShell(QString::fromUtf8( value, value.size() ));
        else if ( name == "gecos" )
          fillGecos( mUser, QCString( value, value.size()+1 ));
        else if ( name == "cn" )
          mUser->setFullName(QString::fromUtf8( value, value.size() ));
        else if ( name == "userpassword" ) {
          QString pwd = QString::fromUtf8( value, value.size() );
          if ( !pwd.isEmpty() ) {
            /* FIXME!!!!!! this is not in any RFC, but does the job
             * (password starting with "!" means disabled account)
             */
            if ( pwd.startsWith("!") ) {
              pwd.remove( 0, 1 );
            } else {
              mUser->setDisabled( false );
            }
            mUser->setPwd( pwd );
            mUser->setSPwd( pwd );
          }
        } else if ( name == "shadowlastchange" ) {
          if ( mUser->getLastChange() == 0 ) //sambapwdlastset is more precise
            mUser->setLastChange(daysToTime(QString::fromUtf8( value, value.size() ).toLong()));
        } else if ( name == "shadowmin" )
          mUser->setMin(QString::fromUtf8( value, value.size() ).toInt());
        else if ( name == "shadowmax" )
          mUser->setMax(QString::fromUtf8( value, value.size() ).toLong());
        else if ( name == "shadowwarning" )
          mUser->setWarn(QString::fromUtf8( value, value.size() ).toLong());
        else if ( name == "shadowinactive" )
          mUser->setInactive(QString::fromUtf8( value, value.size() ).toLong());
        else if ( name == "shadowexpire" )
          mUser->setExpire(daysToTime(QString::fromUtf8( value, value.size() ).toLong()));
        else if ( name == "shadowflag" )
          mUser->setFlag(QString::fromUtf8( value, value.size() ).toLong());
        else if ( name == "sambaacctflags" ) {
          QString samflags = QString::fromUtf8( value, value.size() );
          if ( !samflags.contains( 'D' ) ) mUser->setDisabled( false );
        } else if ( name == "sambasid" )
          mUser->setSID(QString::fromUtf8( value, value.size() ));
        else if ( name == "sambaprimarygroupsid" )
          mUser->setPGSID(QString::fromUtf8( value, value.size() ));
        else if ( name == "sambalmpassword" )
          mUser->setLMPwd(QString::fromUtf8( value, value.size() ));
        else if ( name == "sambantpassword" )
          mUser->setNTPwd(QString::fromUtf8( value, value.size() ));
        else if ( name == "sambahomepath" )
          mUser->setHomePath(QString::fromUtf8( value, value.size() ));
        else if ( name == "sambahomedrive" )
          mUser->setHomeDrive(QString::fromUtf8( value, value.size() ));
        else if ( name == "sambalogonscript" )
          mUser->setLoginScript(QString::fromUtf8( value, value.size() ));
        else if ( name == "sambaprofilepath" )
          mUser->setProfilePath(QString::fromUtf8( value, value.size() ));
        else if ( name == "sambapwdlastset" )
          mUser->setLastChange(QString::fromUtf8( value, value.size() ).toLong());
        break;
      case KABC::LDIF::EndEntry: {
        KUser newUser;
        mUsers.append( new KUser( mUser ) );
        mUser->copy( &newUser );
        newUser.setPwd( "" );
        newUser.setSPwd( "" );
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
  mUser = new KUser();
  mUser->setPwd( "" );
  mUser->setSPwd( "" );
  mParser.startParsing();
  mCancel = true;
  mProg = new KProgressDialog( 0, "", "", i18n("Loading users from LDAP"), true );
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
  mProg->exec();
  if ( mCancel ) job->kill();
  delete mUser;
  return( mOk );
}

QString KUserLDAP::getRDN(KUser *user)
{
  switch ( mCfg->ldapuserrdn() ) {
    case KUserPrefsBase::EnumLdapuserrdn::uid:
      return "uid=" + user->getName();
    case KUserPrefsBase::EnumLdapuserrdn::uidNumber:
      return "uidNumber=" + user->getUID();
  }
  return "";
}

void KUserLDAP::createPassword( KUser *user, const QString &password )
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
    struct md4_ctx ctx;
    const QChar *data = password.unicode();
    Q_UINT8 hash[16], hex[33];

/*  FIXME: maybe this is neccessary for BIGENDIAN systems
    QByteArray passwd( 2 * password.length() );
    for ( uint i = 0; i<password.length(); i++ ) {
      passwd[ i ] = data[i] >> 8 |  data[i] << 8;
    }
*/    
    md4_init( &ctx );
    md4_update( &ctx, (const Q_UINT8*) /*passwd.data()*/ data, 2 * password.length() );
    md4_final( &ctx, (Q_UINT8*) &hash );
    
    snprintf( (char*) &hex, 33, 
      "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
            hash[0], hash[1], hash[2], hash[3], hash[4], hash[5],
            hash[6], hash[7], hash[8], hash[9], hash[10], hash[11],
            hash[12], hash[13], hash[14], hash[15]);

    user->setNTPwd( QString::fromLatin1( (const char*) &hex, 32 ) );
/* FIXME: only NT (MD4) password handled, may need to handle LanManager passwords, too */
    user->setLMPwd( "" );  
  }
}

void KUserLDAP::getLDIF( KUser *user, bool mod )
{
  QString gecos, cn, pwd, samflags;
  ldif.resize( 0 );
  
  pwd = user->getPwd();
  if ( !pwd.isEmpty() && user->getDisabled() ) pwd = "!" + pwd;
  
  cn = user->getFullName();
  if ( cn.isEmpty() ) cn = user->getName();
  
  gecos = QString::fromLatin1("%1,%2,%3,%4")
    .arg(user->getFullName())
    .arg(user->getOffice1())
    .arg(user->getOffice2())
    .arg(user->getAddress());
  
  samflags = "[U";
  samflags += user->getDisabled() ? 'D' : ' ';
  samflags += "         ]";
  
  ldif = "dn: " + getRDN( user ).utf8() + "," + mUrl.dn().utf8() + "\n";
  if ( mod ) { 
    ldif += "changetype: modify\n";
    ldif += "replace: objectClass\n";
  }
  
  if ( caps & Cap_InetOrg )
    ldif += "objectClass: inetOrgPerson\n";
  else
    ldif += "objectClass: account\n";
    
  ldif += "objectClass: posixAccount\n";
  if ( caps & Cap_Shadow ) {
    ldif += "objectClass: shadowAccount\n";
  }
  if ( caps & Cap_Samba ) {
    ldif += "objectClass: sambaSamAccount\n";
  }
  if ( mod ) ldif += "-\nreplace: cn\n";
  ldif += KABC::LDIF::assembleLine( "cn", cn )+"\n";
  if ( mod ) ldif += "-\nreplace: uid\n";
  ldif += KABC::LDIF::assembleLine( "uid", user->getName() ) + "\n";
  if ( mod ) ldif += "-\nreplace: uidnumber\n";
  ldif += KABC::LDIF::assembleLine( "uidnumber", 
    QString::number( user->getUID() ) )+"\n";
  if ( mod ) ldif += "-\nreplace: gidnumber\n";
  ldif += KABC::LDIF::assembleLine( "gidnumber",
    QString::number( user->getGID() ) )+"\n";
  if ( mod ) ldif += "-\nreplace: userpassword\n";
  ldif += KABC::LDIF::assembleLine( "userpassword", pwd )+"\n";
//  if ( mod ) ldif += "-\nreplace: gecos\n";
//  ldif += KABC::LDIF::assembleLine( "gecos", gecos )+"\n";
  if ( mod ) ldif += "-\nreplace: homedirectory\n";
  ldif += KABC::LDIF::assembleLine( "homedirectory", 
    user->getHomeDir() )+"\n";
  if ( mod ) ldif += "-\nreplace: loginshell\n";
  ldif += KABC::LDIF::assembleLine( "loginshell", 
    user->getShell() )+"\n";
  if ( mod ) ldif += "-\n";
  
  if ( caps & Cap_InetOrg ) {
    if ( mod ) ldif += "replace: sn\n";
    ldif += KABC::LDIF::assembleLine( "sn", user->getSurname() ) + "\n";
    if ( mod ) ldif += "-\nreplace: mail\n";
    ldif += KABC::LDIF::assembleLine( "mail", user->getEmail() ) + "\n";
    if ( mod ) ldif += "-\n";
  }
  
  if ( caps & Cap_Samba ) {
        
    if ( mod ) ldif += "replace: sambahomepath\n";
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
  }
    
  if ( caps & Cap_Shadow ) {
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
  }
  ldif += "\n";
  kdDebug() << "ldif: " << ldif << endl;
}

void KUserLDAP::delData( KUser *user )
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
  
  mProg = new KProgressDialog( 0, "", i18n("LDAP operation"), "", true );
  KIO::Job *job = KIO::put( mUrl, -1, false, false, false );
  connect( job, SIGNAL( dataReq( KIO::Job*, QByteArray& ) ),
    this, SLOT( putData( KIO::Job*, QByteArray& ) ) );
  connect( job, SIGNAL( result( KIO::Job* ) ),
    this, SLOT( result( KIO::Job* ) ) );
  mProg->exec();
  return( mOk );
}

void KUserLDAP::putData( KIO::Job *job, QByteArray& data )
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
    mDel.remove();
    mDelUser = 0;
  }
  if ( mUser ) {
    mModSucc.insert( mUser, mit.data() );
    mMod.remove( mit );
    mit = mMod.begin();
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
