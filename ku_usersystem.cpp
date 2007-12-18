/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
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

#include "globals.h"
#include <errno.h>
#include <pwd.h>
#ifdef HAVE_SHADOW_H
#include <shadow.h>
#endif

#include <kdebug.h>

#include "ku_misc.h"
#include "ku_usersystem.h"

KU_UserSystem::KU_UserSystem(KU_PrefsBase *cfg) : KU_Users( cfg )
{
  caps = Cap_ReadOnly | Cap_Passwd;
#ifdef HAVE_SHADOW_H
  if ( !mCfg->shadowsrc().isEmpty() ) caps |= Cap_Shadow;
#endif
#if defined(__FreeBSD__) || defined(__bsdi__)
  caps |= Cap_BSD;
#endif

}

KU_UserSystem::~KU_UserSystem()
{
}

bool KU_UserSystem::reload() 
{
  mErrorString = mErrorDetails = QString();
  if (!loadpwd())
    return false;

  if (!loadsdw())
    return false;

  return true;
}

// Load passwd file

bool KU_UserSystem::loadpwd()
{
  passwd *p;
  KU_User user;
  QString tmp;

  setpwent(); //This should be enough for BSDs
  while ((p = getpwent()) != NULL) {
    user = KU_User();
    user.setCaps( KU_User::Cap_POSIX );
    user.setUID(p->pw_uid);
    user.setGID(p->pw_gid);
    user.setName(QString::fromLocal8Bit(p->pw_name));
    tmp  = QString::fromLocal8Bit( p->pw_passwd );
    if ( tmp != "x" && tmp != "*" && !tmp.startsWith('!') )
      user.setDisabled( false );
    else
      user.setDisabled( true );
    if ( tmp.startsWith('!') ) tmp.remove(0, 1);
    user.setPwd( tmp );
    user.setHomeDir(QString::fromLocal8Bit(p->pw_dir));
    user.setShell(QString::fromLocal8Bit(p->pw_shell));
#if defined(__FreeBSD__) || defined(__bsdi__)
    user.setClass(QString::fromLatin1(p->pw_class));
    user.setLastChange(p->pw_change);
    user.setExpire(p->pw_expire);
#endif

    if ((p->pw_gecos != 0) && (p->pw_gecos[0] != 0))
      fillGecos(user, p->pw_gecos);
    append(user);
  }

  endpwent();
  return true;
}

// Load shadow passwords

bool KU_UserSystem::loadsdw()
{
#ifdef HAVE_SHADOW_H
  struct spwd *spw;
  KU_User user;
  QString tmp;
  int index;
  
  setspent();
  while ((spw = getspent())) {     // read a shadow password structure


    if ((index = lookup(QString::fromLocal8Bit(spw->sp_namp))) == -1) {
      continue;
    }

    user = at( index );
    tmp = QString::fromLocal8Bit( spw->sp_pwdp );
    if ( tmp.startsWith("!!") || tmp == "*" ) {
      user.setDisabled( true );
      tmp.remove( 0, 2 );
    } else
      user.setDisabled( false );

    user.setSPwd( tmp );        // cp the encrypted pwd
    user.setLastChange( daysToTime( spw->sp_lstchg ) );
    user.setMin(spw->sp_min);
    user.setMax(spw->sp_max);
#ifndef _SCO_DS
    user.setWarn(spw->sp_warn);
    user.setInactive(spw->sp_inact);
    user.setExpire( daysToTime( spw->sp_expire ) );
    user.setFlag(spw->sp_flag);
#endif
    replace( index, user );
  }

  endspent();
#endif //HAVE_SHADOW_H  
  return true;
}
