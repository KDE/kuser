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

#ifndef _KU_GROUPFILES_H_
#define _KU_GROUPFILES_H_

#include <sys/types.h>

#include "ku_group.h"

class KU_GroupFiles : public KU_Groups {
public:
  KU_GroupFiles( KU_PrefsBase *cfg );
  virtual ~KU_GroupFiles();

  virtual bool reload();
  virtual bool dbcommit();

private:
  int gr_backuped;
  int gs_backuped;

  int mode, smode;
  uid_t uid;
  gid_t gid;
  
  bool save();
};

#endif // _KU_GROUPFILES_H_

