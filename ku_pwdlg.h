/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Maintained by Adriaan de Groot <groot@kde.org>
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

#ifndef _KU_PWDLG_H_
#define _KU_PWDLG_H_


#include <klineedit.h>
#include <kdialog.h>

class KU_PwDlg : public KDialog {
  Q_OBJECT

public:
  KU_PwDlg( QWidget* parent = 0 );
  ~KU_PwDlg();

  QString getPassword() const;
protected slots:
  virtual void accept();

private:
  KLineEdit   *lepw1;
  KLineEdit   *lepw2;
};

#endif // _KU_PWDLG_H_
