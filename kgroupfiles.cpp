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
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <qstring.h>
#include <qdir.h>

#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>

#include "kglobal_.h"
#include "kgroupfiles.h"
#include "misc.h"
#include "editDefaults.h"

KGroupFiles::KGroupFiles( KUserPrefsBase *cfg ) : KGroups( cfg )
{
  gs_backuped = FALSE;
  gr_backuped = FALSE;
  gn_backuped = FALSE;

  smode = 0400;
  mode = 0644;
  uid = 0;
  gid = 0;

  caps = Cap_Passwd;

  reload();
}

KGroupFiles::~KGroupFiles()
{
  mGroups.clear();
}

bool KGroupFiles::reload()
{
  struct group *p;
  KU::KGroup *tmpKG = 0;
  struct stat st;
  QString filename;
  QString group_filename;
  QString nisgroup_filename;
  int rc = 0;
  int group_errno = 0;
  int nisgroup_errno = 0;
  char processing_file = '\0';
  #define GROUP    0x01
  #define NISGROUP 0x02
  #define MAXFILES 2

  // Prepare to read KUser configuration

  group_filename = mCfg->groupsrc();
  nisgroup_filename = mCfg->nisgroupsrc();
  if(!group_filename.isEmpty()) {
    processing_file = processing_file | GROUP;
    filename.append(group_filename);
  }

  // Start reading group file(s)

  for(int k = 0; k < MAXFILES; k++) {
    rc = stat(QFile::encodeName(filename), &st);
    if(rc != 0) {
      KMessageBox::error( 0, i18n("stat call on file %1 failed: %2\nCheck KUser settings.").
        arg(filename).arg(QString::fromLatin1(strerror(errno))) );
      if( (processing_file & GROUP) != 0 ) {
        group_errno = errno;
        if(!nisgroup_filename.isEmpty()) {
          processing_file = processing_file & ~GROUP;
          processing_file = processing_file | NISGROUP;
          filename.truncate(0);
          filename.append(nisgroup_filename);
        }
        continue;
      }
      else{
        nisgroup_errno = errno;
        break;
      }
    }

    mode = st.st_mode;
    uid = st.st_uid;
    gid = st.st_gid;

  // We are reading our configuration specified group file
#ifdef HAVE_FGETGRENT
    FILE *fgrp = fopen(QFile::encodeName(filename), "r");
    QString tmp;
    if (fgrp == NULL) {
      KMessageBox::error( 0, i18n("Error opening %1 for reading.").arg(filename) );
      return FALSE;
    }

    while ((p = fgetgrent(fgrp)) != NULL) {
#else
    setgrent();
    while ((p = getgrent()) != NULL) {
#endif
      tmpKG = new KU::KGroup();
      tmpKG->setGID(p->gr_gid);
      tmpKG->setName(QString::fromLocal8Bit(p->gr_name));
      tmpKG->setPwd(QString::fromLocal8Bit(p->gr_passwd));

      char *u_name;
      int i = 0;
      while ((u_name = p->gr_mem[i])!=0) {
        tmpKG->addUser(QString::fromLocal8Bit(u_name));
        i++;
      }

      mGroups.append(tmpKG);
    }

    // End reading filename

#ifdef HAVE_FGETGRENT
    fclose(fgrp);
#else
    endgrent();
#endif
    if(!nisgroup_filename.isEmpty()) {
      if(nisgroup_filename == group_filename)
        break;
      processing_file = processing_file & ~GROUP;
      processing_file = processing_file | NISGROUP;
      filename.truncate(0);
      filename.append(nisgroup_filename);
    }
    else
      break;

  }	// end of processing files, for loop

  if( (group_errno == 0) && (nisgroup_errno == 0) )
    return(TRUE);
  if( (group_errno != 0) && (nisgroup_errno != 0) )
    return(FALSE);
  else
    return(TRUE);
}

bool KGroupFiles::save()
{
  kdDebug() << "KGroupFiles::save() " << endl;
  FILE *group_fd = NULL;
  FILE *gshadow_fd = NULL;
  FILE *nisgroup_fd = NULL;
  gid_t mingid = 0;
  int nis_groups_written = 0;
  gid_t tmp_gid = 0;
  QString tmpGe, tmpSe, tmp2;
  QString group_filename, new_group_filename;
  QString gshadow_filename, new_gshadow_filename;
  QString nisgroup_filename, new_nisgroup_filename;

  char errors_found = '\0';
    #define NOMINGID    0x01
    #define NONISGROUP  0x02

  // read KUser configuration info

  group_filename = mCfg->groupsrc();
  new_group_filename = group_filename + QString::fromLatin1(KU_CREATE_EXT);
#ifdef HAVE_SHADOW
  gshadow_filename = mCfg->gshadowsrc();
  if ( !KStandardDirs::exists( gshadow_filename ) )
      gshadow_filename = QString::null;
  else
      new_gshadow_filename = gshadow_filename + QString::fromLatin1(KU_CREATE_EXT);
#endif
  nisgroup_filename = mCfg->nisgroupsrc();
  new_nisgroup_filename = nisgroup_filename + QString::fromLatin1(KU_CREATE_EXT);
  if( nisgroup_filename != group_filename ) {
    mingid = mCfg->nismingid();
  }

  // Backup file(s)

  if(!group_filename.isEmpty()) {
    if (!gr_backuped) {
      if ( !backup(group_filename) ) return false;
      gr_backuped = TRUE;
    }
  }
  if(!gshadow_filename.isEmpty()) {
    if (!gs_backuped) {
      if ( !backup(gshadow_filename) ) return false;
      gs_backuped = TRUE;
    }
  }
  if(!nisgroup_filename.isEmpty() && (nisgroup_filename != group_filename)) {
    if (!gn_backuped) {
      if ( !backup(nisgroup_filename) ) return false;
      gn_backuped = TRUE;
    }
  }

  // Open file(s)

  if(!group_filename.isEmpty()) {
    if((group_fd = fopen(QFile::encodeName(new_group_filename), "w")) == NULL) {
      KMessageBox::error( 0, i18n("Error opening %1 for writing.").arg(new_group_filename) );
      return false;
    }
  }

  if(!gshadow_filename.isEmpty()) {
    if((gshadow_fd = fopen(QFile::encodeName(new_gshadow_filename), "w")) == NULL) {
      KMessageBox::error( 0, i18n("Error opening %1 for writing.").arg(new_gshadow_filename) );
      if ( group_fd ) fclose ( group_fd );
      return false;
    }
  }

  if(!nisgroup_filename.isEmpty() && (nisgroup_filename != group_filename)) {
    if((nisgroup_fd = fopen(QFile::encodeName(new_nisgroup_filename), "w")) == NULL) {
      KMessageBox::error( 0, i18n("Error opening %1 for writing.").arg(new_nisgroup_filename) );
      if ( group_fd ) fclose ( group_fd );
      if ( gshadow_fd ) fclose ( gshadow_fd );
      return false;
    }
  }

  QPtrListIterator<KU::KGroup> it( mGroups );
  KU::KGroup *gr;
  bool addok = false;

  gr = (*it);

  while (true) {

    if ( gr == 0 ) {
      if ( addok ) break;
      it = QPtrListIterator<KU::KGroup> ( mAdd );
      gr = (*it);
      addok = true;
      if ( gr == 0 ) break;
    };

    if ( mDel.containsRef( gr ) ) {
      ++it;
      gr = (*it);
      continue;
    }
    if ( mMod.contains( gr ) ) gr = &( mMod[ gr ] );

#ifdef HAVE_SHADOW
    if ( addok && !mCfg->gshadowsrc().isEmpty() )
      gr->setPwd("x");
#endif

    tmpGe = gr->getName();
    tmpGe.replace( ',', "_" );
    tmpGe.replace( ':', "_" );
    gr->setName( tmpGe );

    tmp_gid = gr->getGID();
    tmpGe += ":" +
            gr->getPwd() + ":" +
            QString::number( gr->getGID() ) + ":";
    tmpSe = gr->getName() + ":!::";
    for (uint j=0; j<gr->count(); j++) {
       if (j != 0) {
         tmpGe += ',';
         tmpSe += ',';
       }
       gr->user( j ).replace( ',', "_" );
       gr->user( j ).replace( ':', "_" );
       tmpGe += gr->user( j) ;
       tmpSe += gr->user( j );
    }
    tmpGe += '\n'; tmpSe += '\n';

    if( (nisgroup_fd != 0) && (mingid != 0) ) {
      if(mingid <= tmp_gid) {
        fputs(tmpGe.local8Bit(), nisgroup_fd);
        nis_groups_written++;
        ++it;
        gr = (*it);
        continue;
      }
    }

    if( (nisgroup_fd != 0) && (mingid == 0) ) {
      errors_found = errors_found | NOMINGID;
    }

    if( (nisgroup_fd == 0) && (mingid != 0) ) {
      errors_found = errors_found | NONISGROUP;
    }

    fputs( tmpGe.local8Bit(), group_fd );
    if ( gshadow_fd ) fputs( tmpSe.local8Bit(), gshadow_fd );
    ++it;
    gr = (*it);
  }

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

  if(nisgroup_fd) {
    fclose(nisgroup_fd);
    chmod(QFile::encodeName(nisgroup_filename), mode);
    chown(QFile::encodeName(nisgroup_filename), uid, gid);
    rename(QFile::encodeName(new_nisgroup_filename),
      QFile::encodeName(nisgroup_filename));
  }

  if( (errors_found & NOMINGID) != 0 ) {
    KMessageBox::error( 0, i18n("Unable to process NIS group file without a minimum GID specified.\nPlease update KUser settings (File Source Settings).") );
  }

  if( (errors_found & NONISGROUP) != 0 ) {
    KMessageBox::error( 0, i18n("Specifying NIS minimum GID requires NIS file(s).\nPlease update KUser settings (File Source Settings).") );
  }

#ifdef GRMKDB
  if( (nis_groups_written > 0) || (nisgroup_filename == group_filename) ) {
    if (system(GRMKDB) != 0) {
      KMessageBox::error( 0, i18n("Unable to build NIS group databases.") );
      return FALSE;
    }
  }
#endif

  return TRUE;
}

bool KGroupFiles::dbcommit()
{
  bool ret;
  mode_t mode;

  kdDebug() << "KGroupFiles dbcommit" << endl;
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

