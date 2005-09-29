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

#ifndef _KU_MAINVIEW_H_
#define _KU_MAINVIEW_H_

#include <stdlib.h>

#include <qevent.h>
#include <qptrlist.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qtabwidget.h>

#include "kuservw.h"
#include "kgroupvw.h"

class mainView : public QTabWidget {
Q_OBJECT
public:
  mainView(QWidget *parent = 0);
  ~mainView();

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

  KUserView *lbusers;
  KGroupView *lbgroups;
  bool mShowSys;
};

#endif // _KU_MAINVIEW_H_
