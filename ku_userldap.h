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

#ifndef _KU_USERLDAP_H_
#define _KU_USERLDAP_H_

#include <QObject>

#include <QProgressDialog>

#include <kldap/ldapurl.h>
#include <kldap/ldif.h>
#include <kldap/ldapsearch.h>
#include <kldap/ldapobject.h>
#include <kldap/ldapoperation.h>

#include <kio/job.h>

#include "ku_user.h"

class KU_UserLDAP : public QObject, public KU_Users {
Q_OBJECT
public:
  KU_UserLDAP(KU_PrefsBase *cfg);
  virtual ~KU_UserLDAP();

  virtual bool reload();
  virtual bool dbcommit();

private slots:
  void result( KLDAP::LdapSearch *search );
  void data( KLDAP::LdapSearch *search, const KLDAP::LdapObject& data );
private:
  KLDAP::LdapUrl mUrl;
  QProgressDialog *mProg;
  bool mOk;
  int mAdv;
  int schemaversion;
  QMap<int, QStringList> mObjectClasses;

  QString getRDN( const KU_User &user ) const;
  void createModStruct( const KU_User &user, int oldindex, KLDAP::LdapOperation::ModOps &ops);
  
  virtual void createPassword( KU_User &user, const QString &password );
};

#endif // _KU_USERLDAP_H_
