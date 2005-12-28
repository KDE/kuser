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


#ifndef _KU_EDITGROUP_H_
#define _KU_EDITGROUP_H_

#include <QCheckBox>
#include <QList>

#include <kdialogbase.h>
#include <klistview.h>
#include <kcombobox.h>
#include <klineedit.h>

#include "ku_group.h"

class KU_EditGroup : public KDialogBase
{
  Q_OBJECT
public:

  KU_EditGroup(KU_Group *akg, bool samba, bool add, 
    QWidget* parent = NULL, const char* name = NULL);

  virtual ~KU_EditGroup();

protected slots:
  virtual void slotOk();
  void addClicked();
  void delClicked();
  void ridSelected( int index );

private:
  bool mSamba, mAdd;
  bool ro;
  KU_Group *kg;
  KListView *m_list_in,*m_list_notin;
  KLineEdit   *legrpname;
  KLineEdit   *legid;
  KComboBox   *lerid;
  KLineEdit   *ledom;
  KLineEdit   *ledispname;
  KLineEdit   *ledesc;
  KLineEdit   *ledomsid;
  KComboBox   *letype;
  QCheckBox   *cbsamba;
  QString mOldName;
  typedef struct _RID {
    uint rid;
    QString name,desc;
  } RID;
  QList<RID> mRids;
};
#endif // _KU_EDITGROUP_H_
