/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi GyĂśrgy <gyurco@freemail.hu>
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

#include "ku_groupfiles.h"

#include "globals.h"

#include <ku_config.h>
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>


#include <kdebug.h>
#include <kstandarddirs.h>
#include <klocale.h>

#include "ku_misc.h"

KU_GroupFiles::KU_GroupFiles( KU_PrefsBase *cfg ) : KU_Groups( cfg )
{
  gs_backuped = false;
  gr_backuped = false;

  smode = 0400;
  mode = 0644;
  uid = 0;
  gid = 0;

  caps = Cap_Passwd;
}

KU_GroupFiles::~KU_GroupFiles()
{
}

bool KU_GroupFiles::reload()
{
  struct group *p;
  KU_Group group;
  struct stat st;
  QString group_filename;
  int rc = 0;

  mErrorString = mErrorDetails = QString();

  group_filename = mCfg->groupsrc();
  if(group_filename.isEmpty()) {
    mErrorString = i18n("Groups file name not set, please check 'Settings/Files'");
    return false;
  }

  // Start reading group file(s)

  rc = stat(QFile::encodeName(group_filename), &st);
  if(rc != 0) {
    mErrorString = i18n("stat() call on file %1 failed: %2\nCheck KUser settings.",
        group_filename, QString::fromLocal8Bit(strerror(errno)));
    return false;
  }

  mode = st.st_mode;
  uid = st.st_uid;
  gid = st.st_gid;

  // We are reading our configuration specified group file
#ifdef HAVE_FGETGRENT
  FILE *fgrp = fopen(QFile::encodeName(group_filename), "r");
  QString tmp;
  if (fgrp == NULL) {
    mErrorString = i18n("Error opening %1 for reading.", group_filename);
    return false;
  }

  while ((p = fgetgrent(fgrp)) != NULL) {
#else
  setgrent();
  while ((p = getgrent()) != NULL) {
#endif
      group = KU_Group();
      group.setGID(p->gr_gid);
      group.setName(QString::fromLocal8Bit(p->gr_name));
      group.setPwd(QString::fromLocal8Bit(p->gr_passwd));

      char *u_name;
      int i = 0;
      while ((u_name = p->gr_mem[i])!=0) {
        group.addUser(QString::fromLocal8Bit(u_name));
        i++;
      }

      append(group);
  }

  // End reading filename

#ifdef HAVE_FGETGRENT
  fclose(fgrp);
#else
  endgrent();
#endif

  return true;
}

bool KU_GroupFiles::save()
{
  kDebug() << "KU_GroupFiles::save() ";
  FILE *group_fd = NULL;
  FILE *gshadow_fd = NULL;
  gid_t tmp_gid = 0;
  QString tmpGe, tmpSe, tmp2;
  QString group_filename, new_group_filename;
  QString gshadow_filename, new_gshadow_filename;

  // read KUser configuration info

  group_filename = mCfg->groupsrc();
  new_group_filename = group_filename + QString::fromLatin1(KU_CREATE_EXT);
#ifdef HAVE_SHADOW_H
  gshadow_filename = mCfg->gshadowsrc();
  if ( !KStandardDirs::exists( gshadow_filename ) )
      gshadow_filename = QString();
  else
      new_gshadow_filename = gshadow_filename + QString::fromLatin1(KU_CREATE_EXT);
#endif

  // Backup file(s)

  if(!group_filename.isEmpty()) {
    if (!gr_backuped) {
      if ( !backup(group_filename) ) return false;
      gr_backuped = true;
    }
  }
  if(!gshadow_filename.isEmpty()) {
    if (!gs_backuped) {
      if ( !backup(gshadow_filename) ) return false;
      gs_backuped = true;
    }
  }

  // Open file(s)

  if( !group_filename.isEmpty() ) {
    if((group_fd = fopen(QFile::encodeName(new_group_filename), "w")) == NULL) {
      mErrorString = i18n("Error opening %1 for writing.", new_group_filename);
      return false;
    }
  }

  if( !gshadow_filename.isEmpty() ) {
    if((gshadow_fd = fopen(QFile::encodeName(new_gshadow_filename), "w")) == NULL) {
      mErrorString = i18n("Error opening %1 for writing.", new_gshadow_filename);
      if ( group_fd ) fclose ( group_fd );
      return false;
    }
  }

/******************/
  KU_Group group;
  int groupsindex = 0, addindex = 0;
  while (true) {

    if ( groupsindex == count() ) {
      if ( addindex == mAdd.count() ) break;
      group = mAdd.at(addindex);
      addindex++;
    } else {
        if ( mDel.contains( groupsindex ) ) {
          groupsindex++;
          continue;
        }
        if ( mMod.contains(groupsindex) )
          group = mMod.value(groupsindex);
        else
          group = at(groupsindex);
        groupsindex++;
    }

#ifdef HAVE_SHADOW_H
    if ( addindex && !mCfg->gshadowsrc().isEmpty() )
      group.setPwd(QLatin1String( "x" ));
#endif

    tmpGe = group.getName();
    tmpGe.replace( QLatin1Char( ',' ), QLatin1String( "_" ) );
    tmpGe.replace( QLatin1Char( ':' ), QLatin1String( "_" ) );
    group.setName( tmpGe );

    tmp_gid = group.getGID();
    tmpGe += QLatin1Char( ':' ) +
            group.getPwd() + QLatin1Char( ':' ) +
            QString::number( group.getGID() ) + QLatin1Char( ':' );
    tmpSe = group.getName() + QLatin1String( ":!::" );	//krazy:exclude=doublequote_chars
    for (uint j=0; j<group.count(); j++) {
       if (j != 0) {
         tmpGe += QLatin1Char( ',' );
         tmpSe += QLatin1Char( ',' );
       }
       group.user( j ).replace( QLatin1Char( ',' ), QLatin1String( "_" ) );
       group.user( j ).replace( QLatin1Char( ':' ), QLatin1String( "_" ) );
       tmpGe += group.user( j) ;
       tmpSe += group.user( j );
    }
    tmpGe += QLatin1Char( '\n' ); tmpSe += QLatin1Char( '\n' );

    if ( group_fd )
        fputs( tmpGe.toLocal8Bit(), group_fd );
    if ( gshadow_fd ) fputs( tmpSe.toLocal8Bit(), gshadow_fd );
  }
/***********************/
  if(group_fd) {
    fclose(group_fd);
    chmod(QFile::encodeName(new_group_filename), mode);
    chown(QFile::encodeName(new_group_filename), uid, gid);
    rename(QFile::encodeName(new_group_filename),
      QFile::encodeName(group_filename));
  }

  if(gshadow_fd) {
    fclose(gshadow_fd);
    chmod(QFile::encodeName(new_gshadow_filename), mode);
    chown(QFile::encodeName(new_gshadow_filename), uid, gid);
    rename(QFile::encodeName(new_gshadow_filename),
      QFile::encodeName(gshadow_filename));
  }

  return true;
}

bool KU_GroupFiles::dbcommit()
{
  bool ret;
  mode_t mode;

  kDebug() << "KU_GroupFiles dbcommit";
  mAddSucc.clear();
  mDelSucc.clear();
  mModSucc.clear();
  if ( mDel.isEmpty() && mAdd.isEmpty() && mMod.isEmpty() )
    return true;

  mode = umask(0077);
  ret = save();
  umask( mode );
  if ( !ret ) return false;

  mDelSucc = mDel;
  mAddSucc = mAdd;
  mModSucc = mMod;
  mDel.clear();
  mAdd.clear();
  mMod.clear();

  return true;
}
