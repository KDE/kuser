/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi GyĂśrgy <gyurco@freemail.hu>
 *  Maintained by Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 or at your option version 3 as published by
 *  the Free Software Foundation.
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

#ifndef _KU_MAINVIEW_H_
#define _KU_MAINVIEW_H_

#include <stdlib.h>

#include <QTreeView>

#include <ktabwidget.h>

#include "ku_usermodel.h"
#include "ku_groupmodel.h"

class KU_MainView : public KTabWidget {
Q_OBJECT
public:
  KU_MainView(QWidget *parent = 0);
  ~KU_MainView();

  void init();
  void setShowSys( bool b ) { mShowSys = b; }

  bool queryClose();

  void clearUsers();
  void clearGroups();
  void reloadUsers();
  void reloadGroups();

public slots:
  void slotTabChanged();

  void userSelected();
  void groupSelected();

  void useradd();
  void useredit();
  void userdel();

  void grpadd();
  void grpedit();
  void grpdel();

  void setpwd();

signals:
  void userSelected(bool);
  void groupSelected(bool);

protected:
  bool updateUsers();
  bool updateGroups();

  QTreeView *userview;
  QTreeView *groupview;
  
  KU_UserModel *usermodel;
  KU_UserSortingProxyModel userproxymodel;
  
  KU_GroupModel *groupmodel;
  KU_GroupSortingProxyModel groupproxymodel;

  bool mShowSys;

  KU_Groups *groups;
  KU_Users *users;
};

#endif // _KU_MAINVIEW_H_
