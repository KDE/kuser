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

#ifndef _KU_PWDDLG_H_
#define _KU_PWDDLG_H_

#include <qstring.h>
#include <klineedit.h>
#include <kdialogbase.h>

class pwddlg : public KDialogBase {
  Q_OBJECT

public:
  pwddlg( QWidget* parent = NULL, const char* name = NULL );
  ~pwddlg();

  QString getPassword() const;
protected slots:
  void slotOk();

private:
  KLineEdit   *lepw1;
  KLineEdit   *lepw2;
};

#endif // _KU_PWDDLG_H_
