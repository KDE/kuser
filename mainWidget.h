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

#ifndef _KU_MAINWIDGET_H
#define _KU_MAINWIDGET_H

#include <kmainwindow.h>

class SelectConn;
class mainView;
class KToggleAction;

class mainWidget : public KMainWindow {
Q_OBJECT
public:
  mainWidget(const char *name = 0);
  ~mainWidget();
  bool queryClose();

protected:
  void init();
  void setupActions();

protected slots:
  void showSys();
  void properties();
  void slotApplySettings();
  void slotApplyConnection();
  void selectconn();

private:
  KToggleAction *mShowSys;
  mainView *md;
  SelectConn *sc;

};

#endif // _KU_MAINWIDGET_H
