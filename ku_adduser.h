/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
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

#ifndef _KU_ADDUSER_H_
#define _KU_ADDUSER_H_

#include <QCheckBox>
#include "ku_edituser.h"

class KU_AddUser: public KU_EditUser {
  Q_OBJECT
public:
  KU_AddUser(KU_User &user, bool useprivategroup,
    QWidget *parent = 0 );

  void setCreateHomeDir(bool b)
     { createhome->setChecked(b); }

  void setCopySkel(bool b)
     { copyskel->setChecked(b); }

  const KU_User &getNewUser() const { return mNewUser; }

protected slots:
  virtual void accept();
protected:
  bool checkHome();
  bool checkMailBox();

  QCheckBox *createhome;
  QCheckBox *copyskel;
private:
  KU_User mNewUser;
};

#endif // _KU_ADDUSER_H_
