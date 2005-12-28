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

#ifndef _KU_GROUPLDAP_H_
#define _KU_GROUPLDAP_H_

#include <sys/types.h>

#include <QString>
#include <QStringList>
#include <Q3PtrList>
#include <QByteArray>

#include <kprogress.h>
#include <kabc/ldapurl.h>
#include <kabc/ldif.h>
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
  void data( KIO::Job*, const QByteArray& );
  void putData( KIO::Job *job, QByteArray& data );
  void result( KIO::Job* );
private:
  KABC::LDIF mParser;
  KABC::LDAPUrl mUrl;
  KProgressDialog *mProg;
    
  KU_Group *mGroup, *mDelGroup, *mAddGroup;
  
  bool first, mOk, mCancel;
  int mAdv;
  QByteArray ldif;

  QString getRDN( KU_Group *group );
  void addData( KU_Group *group );
  void delData( KU_Group *group );
  void modData( KU_Group *group );
};

#endif // _KU_GROUPLDAP_H_

