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

#ifndef _KU_USERFILES_H_
#define _KU_USERFILES_H_

#include <sys/types.h>

#include "ku_user.h"

class KU_UserFiles : public KU_Users {
public:
  KU_UserFiles(KU_PrefsBase *cfg);
  virtual ~KU_UserFiles();

  virtual bool dbcommit();
  virtual bool reload();
  virtual void createPassword( KU_User &user, const QString &password );

private:
  bool pw_backuped;
  bool s_backuped;

  mode_t pwd_mode;
  mode_t sdw_mode;

  uid_t pwd_uid;
  gid_t pwd_gid;

  uid_t sdw_uid;
  gid_t sdw_gid;

  bool loadpwd();
  bool loadsdw();

  bool savepwd();
};
#endif // _KU_USERFILES_H_

