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

#ifndef _KU_USERVW_H_
#define _KU_USERVW_H_

#include <qwidget.h>

#include <klistview.h>

#include "kuser.h"

class KUserViewItem : public KListViewItem
{
public:
  KUserViewItem(KListView *parent, KUser *aku);
  KUser *user() { return mUser; }
private:  
  virtual QString text ( int ) const;
  virtual void paintCell( QPainter *p, const QColorGroup &cg,
    int column, int width, int alignment );
  virtual int compare( QListViewItem *i, int col, bool ascending ) const;
  KUser *mUser;
};

class KUserView : public KListView
{
    Q_OBJECT

public:
  KUserView( QWidget* parent = 0, const char* name = 0 );

  virtual ~KUserView();

  void insertItem(KUser *aku);
  void removeItem(KUser *aku);
  KUser *getCurrentUser();
  void init();
};

#endif // _KU_USERVW_H_
