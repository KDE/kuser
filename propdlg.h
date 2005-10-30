/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Maintained by Adriaan de Groot <groot@kde.org>
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

#ifndef _KU_PROPDLG_H_
#define _KU_PROPDLG_H_

#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qlistbox.h>
#include <qtooltip.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qmap.h>

#include <klineedit.h>
#include <kdatetimewidget.h>
#include <knuminput.h>
#include <kdialogbase.h>
#include <klistview.h>
#include <kcombobox.h>

#include "kuser.h"

class propdlg : public KDialogBase
{
  Q_OBJECT

public:
  propdlg( const QPtrList<KU::KUser> &users,
    QWidget *parent = 0, const char *name = 0 );
  propdlg( KU::KUser *AUser, bool fixedprivgroup, 
    QWidget *parent = 0, const char *name = 0 );
  ~propdlg();

  void mergeUser( KU::KUser *user, KU::KUser *newuser );

protected slots:
  virtual void slotOk();
  void setpwd();
  void changed(); // Change to misc settings
  void cbposixChanged(); // Change to diaable POSIX account info
  void cbsambaChanged(); // Change to diaable POSIX account info
  void gchanged(); // Change to group settings
  void setpgroup(); // Change in primary group

protected:
  void initDlg();
  void selectuser();
  void save();
  bool saveg();
  bool check();
  void loadgroups( bool fixedprivgroup );
  bool checkShell(const QString &shell);
  void addRow( QWidget *parent, QGridLayout *layout, int row, 
  QWidget *widget, const QString &label, const QString &what, 
  bool two_column=true, bool nochange=true );
  void setLE( KLineEdit *le, const QString &val, bool first );
  void setCB( QCheckBox *cb, bool val, bool first );
  void setSB( KIntSpinBox *sb, int val, bool first );
  QString mergeLE( KLineEdit *le, const QString &val, bool one );
  int mergeSB( KIntSpinBox *sb, int val, bool one );

  KIntSpinBox *addDaysGroup( QWidget  *parent, QGridLayout *layout, int row, 
    const QString &title, bool never=true );

  QFrame *frontpage;
  QGridLayout *frontlayout;
  int frontrow;

  QPtrList<KU::KUser> mUsers;
  QMap<QWidget*, QCheckBox*> mNoChanges;
  bool ismoreshells;
  bool ischanged;
  bool isgchanged;
  uid_t olduid;
  uint oldrid;
  QString oldshell;
  QString  primaryGroup;
  bool  primaryGroupWasOn;
  bool ro;

  QString newpass;
  time_t lstchg;

  KListView *lstgrp;

  QPushButton *pbsetpwd;

  QLabel      *lbuser;
  KLineEdit   *leid;
  KLineEdit   *lefname;
  KLineEdit   *lesurname;
  KLineEdit   *lemail;

  KComboBox   *leshell;
  KLineEdit   *lehome;

  KLineEdit   *leoffice;
  KLineEdit   *leophone;
  KLineEdit   *lehphone;
  KLineEdit   *leclass;

  KLineEdit   *leoffice1;
  KLineEdit   *leoffice2;
  KLineEdit   *leaddress;

  QCheckBox   *cbdisabled;
  QCheckBox   *cbposix;
  QCheckBox   *cbsamba;
  QLabel      *leprigr;
  QPushButton *pbprigr;

  QLabel *leslstchg;
  KIntSpinBox *lesmin;
  KIntSpinBox *lesmax;
  KIntSpinBox *leswarn;
  KIntSpinBox *lesinact;
  KDateTimeWidget  *lesexpire;
  QCheckBox *cbexpire;

//samba specific:  
  KLineEdit   *lerid;
  KLineEdit   *leliscript;
  KLineEdit   *leprofile;
  KLineEdit   *lehomedrive;
  KLineEdit   *lehomepath;
  KLineEdit   *leworkstations;
  KLineEdit   *ledomain;
  KLineEdit   *ledomsid;
};

#endif // _KU_PROPDLG_H_

