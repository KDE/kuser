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

#ifndef _KU_ADDUSER_H_
#define _KU_ADDUSER_H_

#include <qcheckbox.h>
#include "propdlg.h"

class addUser: public propdlg {
  Q_OBJECT
public:
#ifdef _KU_QUOTA
  addUser(KUser *AUser, Quota *AQuota, QWidget *parent = 0, const char *name = 0, int isprep = false);
#else
  addUser(KUser *AUser, QWidget *parent = 0, const char *name = 0, int isprep = false);
#endif

  void setCreateHomeDir(bool b)
     { createhome->setChecked(b); }

  void setCopySkel(bool b)
     { copyskel->setChecked(b); }

  void setUsePrivateGroup(bool b)
     { usePrivateGroup->setChecked(b); }

  bool getUsePrivateGroup()
     { return usePrivateGroup->isChecked(); }


protected slots:
  virtual void slotOk();
protected:
  bool checkHome();
  bool checkMailBox();

  QCheckBox *createhome;
  QCheckBox *copyskel;
  QCheckBox *usePrivateGroup;
};

#endif // _KU_ADDUSER_H


