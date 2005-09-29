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

#ifndef _KU_GLOBALS_H_
#define _KU_GLOBALS_H_

#define _KU_VERSION "2.1"

#include <config.h>
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
#include <kconfig.h>
#include "kuserprefs.h"

#define KU_BACKUP_EXT ".bak"
#define KU_CREATE_EXT ".new"

#ifdef HAVE_PATHS_H
  #define SHELL_FILE _PATH_SHELLS
  #define MAIL_SPOOL_DIR _PATH_MAILDIR
#else
  #define SHELL_FILE "/etc/shells"
  #define MAIL_SPOOL_DIR "/var/spool/mail"
#endif

#if defined(__FreeBSD__) || defined(__bsdi__)
  #undef HAVE_SHADOW
  #include <pwd.h>
  #define PASSWORD_FILE _PATH_MASTERPASSWD
  #define PASSWORD_FILE_MASK S_IRUSR | S_IWUSR
  #define PWMKDB _PATH_PWD_MKDB" -p "PASSWORD_FILE
  #define SKELDIR "/usr/share/skel"
  #define SKEL_FILE_PREFIX "dot"
  #define CRONTAB_DIR "/var/cron/tabs"
#else
  #define PASSWORD_FILE "/etc/passwd"
  #define PASSWORD_FILE_MASK S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
  #define PWMKDB "cd /var/yp; make 2>&1 >> /var/log/kuser"
  #define GRMKDB "cd /var/yp; make 2>&1 >> /var/log/kuser"
  #define SKELDIR "/etc/skel"
  #define SKEL_FILE_PREFIX ""
  #define CRONTAB_DIR "/var/spool/cron"
#endif

#define GROUP_FILE "/etc/group"
#define GROUP_FILE_MASK S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH

#endif // _KU_GLOBALS_H_
