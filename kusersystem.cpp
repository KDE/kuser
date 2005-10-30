/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
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

#include "globals.h"
#include <errno.h>
#include <pwd.h>
#ifdef HAVE_SHADOW
#include <shadow.h>
#endif

#include <qstring.h>

#include <kmessagebox.h>
#include <kdebug.h>

#include "kglobal_.h"
#include "misc.h"
#include "kusersystem.h"

KUserSystem::KUserSystem(KUserPrefsBase *cfg) : KU::KUsers( cfg )
{
  mUsers.setAutoDelete(TRUE);

  caps = Cap_ReadOnly | Cap_Passwd;
#ifdef HAVE_SHADOW
  if ( !mCfg->shadowsrc().isEmpty() ) caps |= Cap_Shadow;
#endif
#if defined(__FreeBSD__) || defined(__bsdi__)
  caps |= Cap_BSD;
#endif

  reload();
}

KUserSystem::~KUserSystem()
{
}

bool KUserSystem::reload() 
{
  if (!loadpwd())
    return FALSE;

  if (!loadsdw())
    return FALSE;

  return TRUE;
}

// Load passwd file

bool KUserSystem::loadpwd()
{
  passwd *p;
  KU::KUser *tmpKU = 0;
  QString tmp;

  setpwent(); //This should be enough for BSDs
  while ((p = getpwent()) != NULL) {
    tmpKU = new KU::KUser();
    tmpKU->setUID(p->pw_uid);
    tmpKU->setGID(p->pw_gid);
    tmpKU->setName(QString::fromLocal8Bit(p->pw_name));
    tmp  = QString::fromLocal8Bit( p->pw_passwd );
    if ( tmp != "x" && tmp != "*" && !tmp.startsWith("!") )
      tmpKU->setDisabled( false );
    else
      tmpKU->setDisabled( true );
    if ( tmp.startsWith("!") ) tmp.remove(0, 1);
    tmpKU->setPwd( tmp );
    tmpKU->setHomeDir(QString::fromLocal8Bit(p->pw_dir));
    tmpKU->setShell(QString::fromLocal8Bit(p->pw_shell));
#if defined(__FreeBSD__) || defined(__bsdi__)
    tmpKU->setClass(QString::fromLatin1(p->pw_class));
    tmpKU->setLastChange(p->pw_change);
    tmpKU->setExpire(p->pw_expire);
#endif

    if ((p->pw_gecos != 0) && (p->pw_gecos[0] != 0))
      fillGecos(tmpKU, p->pw_gecos);
    mUsers.append(tmpKU);
  }

  endpwent();
  return(TRUE);
}

// Load shadow passwords

bool KUserSystem::loadsdw()
{
#ifdef HAVE_SHADOW
  struct spwd *spw;
  KU::KUser *up = NULL;
  QString tmp;

  setspent();
  while ((spw = getspent())) {     // read a shadow password structure

    if ((up = lookup(QString::fromLocal8Bit(spw->sp_namp))) == NULL) {
      continue;
    }

    tmp = QString::fromLocal8Bit( spw->sp_pwdp );
    if ( tmp.startsWith("!!") || tmp == "*" ) {
      up->setDisabled( true );
      tmp.remove( 0, 2 );
    } else
      up->setDisabled( false );

    up->setSPwd( tmp );        // cp the encrypted pwd
    up->setLastChange( daysToTime( spw->sp_lstchg ) );
    up->setMin(spw->sp_min);
    up->setMax(spw->sp_max);
#ifndef _SCO_DS
    up->setWarn(spw->sp_warn);
    up->setInactive(spw->sp_inact);
    up->setExpire( daysToTime( spw->sp_expire ) );
    up->setFlag(spw->sp_flag);
#endif
  }

  endspent();
#endif //HAVE_SHADOW  
  return true;
}
