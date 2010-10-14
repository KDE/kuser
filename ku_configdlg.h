/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
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

#ifndef _KU_CONFIGDLG_H_
#define _KU_CONFIGDLG_H_

#include <QList>
#include <QProgressDialog>

#include <kconfigdialog.h>
#include <kldap/ldif.h>
#include <kprogressdialog.h>
#include <kio/job.h>

class Ui_KU_LdapSamba;
class Ui_KU_LdapSettings;
class Ui_KU_FilesSettings;
namespace KLDAP { class LdapConfigWidget; }
namespace KIO { class Job; }
struct KU_SambaDomain {
  QString name;
  QString sid;
  uint ridbase;
};

class KU_ConfigDlg : public KConfigDialog {
  Q_OBJECT
public:
  KU_ConfigDlg( KConfigSkeleton *config, QWidget* parent, const char * name = 0 );
  ~KU_ConfigDlg();
private:
  QProgressDialog *mProg;
  Ui_KU_LdapSamba *sambaui;
  Ui_KU_LdapSettings *ldapui;
  Ui_KU_FilesSettings *fileui;
  KLDAP::LdapConfigWidget *ldconf;
  KLDAP::Ldif mLdif;
  QString mErrorMsg;
  QList<KU_SambaDomain> mResult;
  KU_SambaDomain mDomain;
private slots:
  void slotQueryClicked();
  void loadData( KIO::Job*, const QByteArray& d );
  void loadResult( KJob* job);
};

#endif // _KU_CONFIGDLG_H_
