/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
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

#ifndef _KU_MAINDLG_H
#define _KU_MAINDLG_H

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

  bool queryClose();

  QPixmap pic_user;

public slots:
  void slotTabChanged();

  void save();
  void properties();
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
  void reloadUsers();
  void reloadGroups();

  KUserView *lbusers;
  KGroupView *lbgroups;
private:
  bool changed;
};

#endif // _KU_MAINDLG_H
