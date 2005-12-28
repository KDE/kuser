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

#ifndef _KU_USERLDAP_H_
#define _KU_USERLDAP_H_

#include <sys/types.h>

#include <qobject.h>
#include <qstring.h>
#include <q3ptrlist.h>
#include <QByteArray>

#include <kprogress.h>
#include <kabc/ldapurl.h>
#include <kabc/ldif.h>
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
  void data( KIO::Job*, const QByteArray& );
  void putData( KIO::Job *job, QByteArray& data );
  void result( KIO::Job* );
private:
  KABC::LDIF mParser;
  KABC::LDAPUrl mUrl;
  KProgressDialog *mProg;
  bool mOk, mCancel;
  KU_User *mUser, *mDelUser, *mAddUser;
  int mAdv;
  QByteArray ldif;
  int schemaversion;
  QStringList mOc;
  QMap<KU_User*, QStringList> mObjectClasses;
  
  QString getRDN( KU_User *user );
  void getLDIF( KU_User *user, bool mod );
  void delData( KU_User *user );
  
  virtual void createPassword( KU_User *user, const QString &password );
};

#endif // _KU_USERLDAP_H_
