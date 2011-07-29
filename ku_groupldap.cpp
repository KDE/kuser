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

#include <QLabel>

#include <kdebug.h>
#include <klocale.h>
#include <kldap/ldapdefs.h>

#include "ku_groupldap.h"
#include "ku_misc.h"

KU_GroupLDAP::KU_GroupLDAP( KU_PrefsBase *cfg ) : KU_Groups( cfg )
{
  if ( mCfg->ldapssl() )
    mUrl.setProtocol(QLatin1String( "ldaps" ));
  else
    mUrl.setProtocol(QLatin1String( "ldap" ));

  mUrl.setHost( mCfg->ldaphost() );
  mUrl.setPort( mCfg->ldapport() );
  mUrl.setDn( KLDAP::LdapDN( mCfg->ldapgroupbase() + QLatin1Char( ',' ) + mCfg->ldapdn() ) );
  if ( !mCfg->ldapanon() ) {
    mUrl.setUser( mCfg->ldapuser() );
    mUrl.setPass( mCfg->ldappassword() );
    QString binddn = mCfg->ldapbinddn();
    if ( !binddn.isEmpty() )
      mUrl.setExtension( QLatin1String( "bindname" ),binddn );
  }
  mUrl.setFilter( mCfg->ldapgroupfilter() );

  if ( mCfg->ldaptls() ) mUrl.setExtension(QLatin1String( "x-tls" ),QLatin1String( "" ));
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
      return QLatin1String( "cn=" ) + group.getName();
    case KU_PrefsBase::EnumLdapgrouprdn::gidNumber:
      return QLatin1String( "gidNumber=" ) + QString::number( group.getGID() );
    default:
      return QLatin1String( "" );
  }
}

void KU_GroupLDAP::result( KLDAP::LdapSearch *search )
{
  kDebug() << "LDAP result: " << search->error();
  mProg->hide();

  if ( search->error() ) {
    mErrorString = KLDAP::LdapConnection::errorString(search->error());
    mOk = false;
  } else {
    mOk = true;
  }
}

void KU_GroupLDAP::data( KLDAP::LdapSearch *, const KLDAP::LdapObject& data )
{
  KU_Group group;

  KLDAP::LdapAttrMap attrs = data.attributes();
  for ( KLDAP::LdapAttrMap::ConstIterator it = attrs.constBegin(); it != attrs.constEnd(); ++it ) {
    QString name = it.key().toLower();

    if ( name == QLatin1String( "objectclass" ) ) {
      for ( KLDAP::LdapAttrValue::ConstIterator it2 = (*it).constBegin(); it2 != (*it).constEnd(); ++it2 ) {
        if ( (*it2).toLower() == "sambagroupmapping" )
          group.setCaps( KU_Group::Cap_Samba );
      }
      continue;
    }

    if ( name == QLatin1String( "memberuid" ) ) {
      for ( KLDAP::LdapAttrValue::ConstIterator it2 = (*it).constBegin(); it2 != (*it).constEnd(); ++it2 ) {
        group.addUser( QLatin1String(*it2) );
      }
      continue;
    }

    KLDAP::LdapAttrValue values = (*it);
    if ( values.isEmpty() ) continue;
    QString val = QString::fromUtf8( values.first(), values.first().size() );
    if ( name == QLatin1String( "gidnumber" ) )
      group.setGID( val.toLong() );
    else if ( name == QLatin1String( "cn" ) )
      group.setName( val );
    else if ( name == QLatin1String( "userpassword" ) )
      group.setPwd( val );
    else if ( name == QLatin1String( "sambasid" ) )
      group.setSID( val );
    else if ( name == QLatin1String( "sambagrouptype" ) )
      group.setType( val.toInt() );
    else if ( name == QLatin1String( "displayname" ) )
      group.setDisplayName( val );
    else if ( name == QLatin1String( "description" ) )
      group.setDesc( val );
  }

  append( group );

  if ( ( count() & 7 ) == 7 ) {
    mProg->setValue( mProg->value() + mAdv );
    if ( mProg->value() == 0 ) mAdv = 1;
    if ( mProg->value() == mProg->maximum()-1 ) mAdv = -1;
  }
}

bool KU_GroupLDAP::reload()
{
  kDebug() << "KU_GroupLDAP::reload()";
  mErrorString = mErrorDetails = QString();
  mProg = new QProgressDialog( 0 );
  mProg->setLabel( new QLabel (i18n("Loading Groups From LDAP")) );
  mProg->setAutoClose( false );
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

bool KU_GroupLDAP::dbcommit()
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
  mProg->setLabel( new QLabel(i18n("LDAP Operation")) );
  mProg->setAutoClose( false );
  mProg->setAutoReset( false );
  mProg->setMaximum( mAdd.count() + mMod.count() + mDel.count() );

  //modify
  for ( KU_Groups::ModList::Iterator it = mMod.begin(); it != mMod.end(); ++it ) {
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
  for ( KU_Groups::AddList::Iterator it = mAdd.begin(); it != mAdd.end(); ++it ) {
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
  for ( KU_Groups::DelList::Iterator it = mDel.begin(); it != mDel.end(); ++it ) {
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

void KU_GroupLDAP::createModStruct( const KU_Group &group, int oldindex, KLDAP::LdapOperation::ModOps &ops)
{
  QList<QByteArray> vals;
  bool mod = ( oldindex != -1 );

  vals.append("posixgroup");
  if ( ( getCaps() & Cap_Samba ) && ( group.getCaps() & KU_Group::Cap_Samba ) ) {
    vals.append("sambagroupmapping");
  }
  ku_add2ops( ops, QLatin1String( "objectClass" ), vals );
  vals.clear();
  ku_add2ops( ops, QLatin1String( "cn" ), group.getName().toUtf8() );
  ku_add2ops( ops, QLatin1String( "gidnumber" ), QString::number(group.getGID()).toUtf8() );
  ku_add2ops( ops, QLatin1String( "userpassword" ), group.getPwd().toUtf8() );
  for ( uint i=0; i < group.count(); i++ ) {
    vals.append( group.user(i).toUtf8() );
  }
  ku_add2ops( ops, QLatin1String( "memberuid" ), vals );
  vals.clear();
  if ( getCaps() & Cap_Samba ) {
    if ( group.getCaps() & KU_Group::Cap_Samba ) {
      ku_add2ops( ops, QLatin1String( "sambasid" ), group.getSID().getSID().toUtf8() );
      ku_add2ops( ops, QLatin1String( "displayname" ), group.getDisplayName().toUtf8() );
      ku_add2ops( ops, QLatin1String( "description" ), group.getDesc().toUtf8() );
      ku_add2ops( ops, QLatin1String( "sambagrouptype" ), QString::number( group.getType() ).toUtf8() );
    } else if (mod) {
      ku_add2ops( ops, QLatin1String( "sambasid" ) );
      ku_add2ops( ops, QLatin1String( "displayname" ) );
      ku_add2ops( ops, QLatin1String( "description" ) );
      ku_add2ops( ops, QLatin1String( "sambagrouptype" ) );
    }
  }
}

#include "ku_groupldap.moc"
