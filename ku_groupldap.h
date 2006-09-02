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

#include <QString>
#include <QStringList>
#include <QProgressDialog>
#include <QByteArray>

#include <kldap/ldapurl.h>
#include <kldap/ldif.h>
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
  void result( KJob* );
private:
  enum LastOperation{ None, Mod, Add, Del };
  LastOperation mLastOperation;
  KLDAP::Ldif mParser;
  KLDAP::LdapUrl mUrl;
  QProgressDialog *mProg;

  KU_Group mGroup;
  ModList::Iterator mModIt;
  bool mOk;
  int mAdv, mDelIndex, mAddIndex;

  QString getRDN( const KU_Group &group ) const;
  QByteArray addData( const KU_Group &group ) const;
  QByteArray delData( const KU_Group &group ) const;
  QByteArray modData( const KU_Group &group, int oldindex ) const;
};

#endif // _KU_GROUPLDAP_H_
