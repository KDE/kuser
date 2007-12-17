/*
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
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

#ifndef _KU_SELECTCONN_H_
#define _KU_SELECTCONN_H_

#include <kdialog.h>
#include <kcombobox.h>

class KU_SelectConn : public KDialog {
  Q_OBJECT
public:
  KU_SelectConn( const QString &selected, QWidget* parent );
  QString connSelected();
protected slots:
  void slotUser1();
  void slotUser2();
  void slotUser3();  
  void slotNewApplySettings();
  void slotApplySettings();
  void slotApply();
  
protected:
  KComboBox *mCombo;
  QString conn, newconn, mSelected;
};

#endif // _KU_SELECTCONN_H_
