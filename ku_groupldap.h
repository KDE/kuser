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

#ifndef _KU_GROUPLDAP_H_
#define _KU_GROUPLDAP_H_


#include <QProgressDialog>

#include <kldap/ldapsearch.h>
#include <kldap/ldapobject.h>
#include <kldap/ldapoperation.h>
#include <kldap/ldapurl.h>
#include <kio/job.h>

#include "ku_group.h"

class KU_GroupLDAP : public QObject, public KU_Groups {
Q_OBJECT
public:
  KU_GroupLDAP( KU_PrefsBase *cfg );
  virtual ~KU_GroupLDAP();

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

  QString getRDN( const KU_Group &group ) const;
  void createModStruct( const KU_Group &group, int oldindex, KLDAP::LdapOperation::ModOps &ops);
};

#endif // _KU_GROUPLDAP_H_
