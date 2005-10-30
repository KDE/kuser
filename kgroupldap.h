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

#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>

#include <kprogress.h>
#include <kabc/ldapurl.h>
#include <kabc/ldif.h>
#include <kio/job.h>

#include "kgroup.h"

class KGroupLDAP : public QObject, public KU::KGroups {
Q_OBJECT
public:
  KGroupLDAP( KUserPrefsBase *cfg );
  virtual ~KGroupLDAP();

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
    
  KU::KGroup *mGroup, *mDelGroup, *mAddGroup;
  
  bool first, mOk, mCancel;
  int mAdv;
  QCString ldif;

  QString getRDN( KU::KGroup *group );
  void addData( KU::KGroup *group );
  void delData( KU::KGroup *group );
  void modData( KU::KGroup *group );
};

#endif // _KU_GROUPLDAP_H_

