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

#include <kdebug.h>
#include <klocale.h>

#include "ku_groupldap.h"

KU_GroupLDAP::KU_GroupLDAP( KU_PrefsBase *cfg ) : KU_Groups( cfg )
{
  if ( mCfg->ldapssl() )
    mUrl.setProtocol("ldaps");
  else
    mUrl.setProtocol("ldap");

  mUrl.setHost( mCfg->ldaphost() );
  mUrl.setPort( mCfg->ldapport() );
  mUrl.setDn( mCfg->ldapgroupbase() + "," + mCfg->ldapdn() );
  if ( !mCfg->ldapanon() ) {
    mUrl.setUser( mCfg->ldapuser() );
    mUrl.setPass( mCfg->ldappassword() );
  }
  mUrl.setFilter( mCfg->ldapgroupfilter() );

  if ( mCfg->ldaptls() ) mUrl.setExtension("x-tls","");
  if ( mCfg->ldapsasl() ) {
    mUrl.setExtension( "x-sasl", "" );
    mUrl.setExtension( "x-mech", mCfg->ldapsaslmech() );
  }

  mUrl.setScope(KABC::LDAPUrl::One);
  mUrl.setExtension("x-dir","base");

  if ( mCfg->ldaptimelimit() )
    mUrl.setExtension("x-timelimit",QString::number(mCfg->ldaptimelimit()));
  if ( mCfg->ldapsizelimit() )
    mUrl.setExtension("x-sizelimit",QString::number(mCfg->ldapsizelimit()));
  if ( mCfg->ldappagesize() )
    mUrl.setExtension("x-pagesize",QString::number(mCfg->ldappagesize()));

  caps = Cap_Passwd;
  if ( mCfg->ldapsam() ) {
    caps |= Cap_Samba;
    domsid = mCfg->samdomsid();
  }
}

KU_GroupLDAP::~KU_GroupLDAP()
{
}

QString KU_GroupLDAP::getRDN( const KU_Group &group ) const
{
  switch ( mCfg->ldapgrouprdn() ) {
    case KU_PrefsBase::EnumLdapgrouprdn::cn:
      return "cn=" + group.getName();
    case KU_PrefsBase::EnumLdapgrouprdn::gidNumber:
      return "gidNumber=" + QString::number( group.getGID() );
    default:
      return "";
  }
}

void KU_GroupLDAP::result( KJob *job )
{
  mProg->close();
  if ( job->error() ) {
    QString errstr = job->errorString();
    if ( !errstr.isEmpty() ) {
      mErrorString =  errstr;
//      mErrorDetails = ldif;
    } else {
      mErrorString = i18n("Unknown error");
    }
    mOk = false;
  } else {
    mOk = true;
  }
}

void KU_GroupLDAP::data( KIO::Job*, const QByteArray& data )
{
  if ( data.size() ) {
    mParser.setLDIF( data );
  } else {
    mParser.endLDIF();
  }

  KABC::LDIF::ParseValue ret;
  QString name, val;
  QByteArray value;
  do {
    ret = mParser.nextItem();
    switch ( ret ) {
      case KABC::LDIF::Item:
        name = mParser.attr().toLower();
        value = mParser.value();
        val = QString::fromUtf8( value, value.size() );
        if ( name == "objectclass" ) {
          if ( val.toLower() == "sambagroupmapping" ) 
            mGroup.setCaps( KU_Group::Cap_Samba );
        } else if ( name == "gidnumber" )
          mGroup.setGID( val.toLong() );
        else if ( name == "cn" )
          mGroup.setName( val );
        else if ( name == "userpassword" )
          mGroup.setPwd( val );
        else if ( name == "memberuid" )
          mGroup.addUser( val );
        else if ( name == "sambasid" )
          mGroup.setSID( val );
        else if ( name == "sambagrouptype" )
          mGroup.setType( val.toInt() );
        else if ( name == "displayname" )
          mGroup.setDisplayName( val );
        else if ( name == "description" )
          mGroup.setDesc( val );
        break;
      case KABC::LDIF::EndEntry: {
        append( mGroup );
        mGroup = KU_Group();
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
  } while ( ret != KABC::LDIF::MoreData );
}

bool KU_GroupLDAP::reload()
{
  kDebug() << "KU_GroupLDAP::reload()" << endl;
  mErrorString = mErrorDetails = QString();
  
  mGroup = KU_Group();
  mParser.startParsing();

  mProg = new QProgressDialog( 0 );
  mProg->setLabel( new QLabel (i18n("Loading Groups From LDAP")) );
  mProg->setAutoClose( false );
  mProg->setMaximum( 100 );
  mAdv = 1;

  KIO::Job *job = KIO::get( mUrl, true, false );
  connect( job, SIGNAL( data( KIO::Job*, const QByteArray& ) ),
    this, SLOT( data( KIO::Job*, const QByteArray& ) ) );
  connect( job, SIGNAL( result( KJob* ) ),
    this, SLOT( result( KJob* ) ) );
  mProg->exec();
  if ( mProg->wasCanceled() ) job->kill();

  return( mOk );
}

bool KU_GroupLDAP::dbcommit()
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
  mProg->setLabel( new QLabel(i18n("LDAP Operation")) );
  mProg->setAutoClose( false );
  mProg->setAutoReset( false );
  mProg->setMaximum( mAdd.count() + mMod.count() + mDel.count() );
  KIO::Job *job = KIO::put( mUrl, -1, false, false, false );
  connect( job, SIGNAL( dataReq( KIO::Job*, QByteArray& ) ),
    this, SLOT( putData( KIO::Job*, QByteArray& ) ) );
  connect( job, SIGNAL( result( KJob* ) ),
    this, SLOT( result( KJob* ) ) );
  mProg->exec();
  delete mProg;
  return( mOk );
}

void KU_GroupLDAP::putData( KIO::Job*, QByteArray& data )
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
    case Del:
      mDelSucc.append( mDel.at( mDelIndex ) );
      mDelIndex++;
      break;
    default:
      break;
  }

  if ( mModIt != mMod.end() ) {
    data = modData( mModIt.value(), mModIt.key() );
    mLastOperation = Mod;
  } else if ( mDelIndex < mDel.count() ) {
    data = delData( at(mDel.at( mDelIndex ) ));
    mLastOperation = Del;
  } else if ( mAddIndex < mAdd.count() ) {
    data = addData( mAdd.at( mAddIndex ) );
    mLastOperation = Add;
  } else {
    data.resize( 0 );
  }
}

QByteArray KU_GroupLDAP::addData( const KU_Group &group ) const
{
  QByteArray ldif = "dn: " + getRDN( group ).toUtf8() + "," +
    mUrl.dn().toUtf8() + "\n" + "objectclass: posixGroup\n";

  ldif +=
    KABC::LDIF::assembleLine( "cn", group.getName() ) + "\n" +
    KABC::LDIF::assembleLine( "gidnumber", QString::number(group.getGID()) ) + "\n" +
    KABC::LDIF::assembleLine( "userpassword", group.getPwd() ) + "\n";
  for ( uint i=0; i < group.count(); i++ ) {
    ldif += KABC::LDIF::assembleLine( "memberuid", group.user(i) ) + "\n";
  }
  if ( ( getCaps() & Cap_Samba ) && ( group.getCaps() & KU_Group::Cap_Samba ) ) {
    ldif += "objectclass: sambagroupmapping\n" +
      KABC::LDIF::assembleLine( "sambasid", group.getSID().getSID() ) + "\n" +
      KABC::LDIF::assembleLine( "displayname", group.getDisplayName() ) + "\n" +
      KABC::LDIF::assembleLine( "description", group.getDesc() ) + "\n" +
      KABC::LDIF::assembleLine( "sambagrouptype", QString::number( group.getType() ) ) + "\n";
  }
  ldif += "\n\n";
  kDebug() << "ldif: " << ldif << endl;
  return ldif;
}

QByteArray KU_GroupLDAP::delData( const KU_Group &group ) const
{
  QByteArray ldif = "dn: " + getRDN( group ).toUtf8() + "," +
    mUrl.dn().toUtf8() + "\n" + "changetype: delete\n\n";
  kDebug() << "ldif: " << ldif << endl;
  return ldif;
}

QByteArray KU_GroupLDAP::modData( const KU_Group &group, int oldindex ) const
{
  QByteArray ldif;
  QString oldrdn = getRDN( at( oldindex ) );
  QString newrdn = getRDN( group );

  if ( oldrdn != newrdn ) {
    ldif = "dn: " + oldrdn.toUtf8() + "," + mUrl.dn().toUtf8() + "\n" +
      "changetype: modrdn\n" +
      "newrdn: " + newrdn.toUtf8() + "\n" +
      "deleteoldrdn: 1\n\n";      
  }

  ldif += "dn: " + newrdn.toUtf8() + "," + mUrl.dn().toUtf8() + "\n" +
    "changetype: modify\n" +
    "replace: objectclass\n" +
    "objectclass: posixgroup\n";
  if ( ( getCaps() & Cap_Samba ) && ( group.getCaps() & KU_Group::Cap_Samba ) ) {
    ldif += "objectclass: sambagroupmapping\n";
  }
  ldif +=
    "-\nreplace: cn\n" +
    KABC::LDIF::assembleLine( "cn", group.getName() ) +
    "\n-\nreplace: gidnumber\n" +
    KABC::LDIF::assembleLine( "gidnumber", QString::number(group.getGID()) ) +
    "\n-\nreplace: userpassword\n" +
    KABC::LDIF::assembleLine( "userpassword", group.getPwd() ) +
    "\n-\nreplace: memberuid\n";
  for ( uint i=0; i < group.count(); i++ ) {
    ldif += KABC::LDIF::assembleLine( "memberuid", group.user(i)) + "\n";
  }
  if ( getCaps() & Cap_Samba ) {
    if ( group.getCaps() & KU_Group::Cap_Samba ) {
      ldif +=
        "-\nreplace: sambasid\n" +
        KABC::LDIF::assembleLine( "sambasid", group.getSID().getSID() ) +
        "\n-\nreplace: displayname\n" +
        KABC::LDIF::assembleLine( "displayname", group.getDisplayName() ) +
        "\n-\nreplace: description\n" +
        KABC::LDIF::assembleLine( "description", group.getDesc() ) +
        "\n-\nreplace: sambagrouptype\n" +
        KABC::LDIF::assembleLine( "sambagrouptype", QString::number( group.getType() ) ) + "\n";
    } else {
      ldif += "-\nreplace: sambasid\n";
      ldif += "-\nreplace: displayname\n";
      ldif += "-\nreplace: description\n";
      ldif += "-\nreplace: sambagrouptype\n";
      ldif += "-\nreplace: sambasidlist\n";
    }
  }

  ldif += "-\n\n";
  return ldif;
}

#include "ku_groupldap.moc"
