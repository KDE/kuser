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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/


#ifndef _EDITGROUP_H_
#define _EDITGROUP_H_

#include <qcheckbox.h>
#include <kdialogbase.h>
#include <klistview.h>
#include <kcombobox.h>
#include <klineedit.h>

#include "kgroup.h"

class editGroup : public KDialogBase
{
  Q_OBJECT
public:

  editGroup(KGroup *akg, bool samba, bool add, 
    QWidget* parent = NULL, const char* name = NULL);

  virtual ~editGroup();

protected slots:
  virtual void slotOk();
  void addClicked();
  void delClicked();
  void ridSelected( int index );

private:
  bool mSamba, mAdd;
  bool ro;
  KGroup *kg;
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
  QValueList<RID> mRids;
};
#endif // _EDITGROUP_H_
