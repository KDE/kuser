/*
 *  Copyright (c) 2002 Waldo Bastian <bastian@kde.org>
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


#ifndef _KU_DELUSER_H_
#define _KU_DELUSER_H_

#include <kdialog.h>
#include <QCheckBox>

#include "ku_user.h"

class KU_DelUser: public KDialog {
  Q_OBJECT
public:
  KU_DelUser(KU_User *AUser, QWidget *parent = 0);

  bool getDeleteHomeDir()
     { return m_deleteHomeDir->isChecked(); }
  bool getDeleteMailBox()
     { return m_deleteMailBox->isChecked(); }

private:
  QCheckBox *m_deleteHomeDir;
  QCheckBox *m_deleteMailBox;
};

#endif // _KU_DELUSER_H


