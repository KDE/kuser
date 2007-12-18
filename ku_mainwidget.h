/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
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

#ifndef _KU_MAINWIDGET_H_
#define _KU_MAINWIDGET_H_

#include <kxmlguiwindow.h>

class KU_SelectConn;
class KU_MainView;
class KToggleAction;

class KU_MainWidget : public KXmlGuiWindow {
Q_OBJECT
public:
  KU_MainWidget();
  ~KU_MainWidget();
  bool queryClose();

protected:
  void init();
  void setupActions();

protected slots:
  void showSys( bool show );
  void properties();
  void slotApplySettings();
  void slotApplyConnection();
  void selectconn();
  void reload();

private:
  KToggleAction *mShowSys;
  KU_MainView *mv;
  KU_SelectConn *sc;
};

#endif // _KU_MAINWIDGET_H_
