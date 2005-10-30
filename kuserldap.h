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

#ifndef _KUSERLDAP_H_
#define _KUSERLDAP_H_

#include <sys/types.h>

#include <qobject.h>
#include <qstring.h>
#include <qptrlist.h>

#include <kprogress.h>
#include <kabc/ldapurl.h>
#include <kabc/ldif.h>
#include <kio/job.h>

#include "kuser.h"

class KUserLDAP : public QObject, public KU::KUsers {
Q_OBJECT
public:
  KUserLDAP(KUserPrefsBase *cfg);
  virtual ~KUserLDAP();

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
  bool mOk, mCancel;
  KU::KUser *mUser, *mDelUser, *mAddUser;
  int mAdv;
  QCString ldif;
  int schemaversion;
  QStringList mOc;
  QMap<KU::KUser*, QStringList> mObjectClasses;
  
  QString getRDN( KU::KUser *user );
  void getLDIF( KU::KUser *user, bool mod );
  void delData( KU::KUser *user );
  
  virtual void createPassword( KU::KUser *user, const QString &password );
};

#endif // _KUSERLDAP_H_
