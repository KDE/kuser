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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _KU_QUOTA
#include <stdlib.h>
#include <stdio.h>

#include "mnt.h"
#include "kuqconf.h"

#include <qfile.h>

MntEnt::MntEnt(const QString &afsname, const QString &adir,
               const QString &atype, const QString &aopts,
               const QString &aquotafilename){
    fsname = afsname;
    dir    = adir;
    type   = atype;
    opts   = aopts;
    quotafilename = aquotafilename;
}

MntEnt::~MntEnt() {
}

QString MntEnt::getfsname() const {
  return fsname;
}

QString MntEnt::getdir() const {
  return dir;
}

QString MntEnt::gettype() const {
  return type;
}

QString MntEnt::getopts() const {
  return opts;
}

QString MntEnt::getquotafilename() const {
  return quotafilename;
}

void MntEnt::setfsname(const QString &data) {
  fsname = data;
}

void MntEnt::setdir(const QString &data) {
  dir = data;
}

void MntEnt::settype(const QString &data) {
  type = data;
}

void MntEnt::setopts(const QString &data) {
  opts = data;
}

void MntEnt::setquotafilename(const QString &data) {
  quotafilename = data;
}

Mounts::Mounts() {
  Mounts::m.setAutoDelete(TRUE);

#ifdef OLD_GETMNTENT
  struct mnttab *mt = NULL;
#elif defined(BSD)
  struct fstab *mt = NULL;
#elif defined(_AIX)
  struct fstab *mt = NULL;
  struct vfs_ent *vt = NULL;
#else
  struct mntent *mt = NULL;
#endif
#if !defined(BSD) &&!defined(_AIX)
  FILE *fp;
#endif
  MntEnt *mnt = NULL;
  QString quotafilename;

  if (is_quota == 0)
    return;
  is_quota = 0;

#ifdef OLD_GETMNTENT
  fp = fopen(MNTTAB, "r");
  mt = (struct mnttab *)malloc(sizeof(mnttab));

  while ((getmntent(fp, mt)) == 0) {
    if (strstr(mt->mnt_mntopts, "quota") == NULL)
      continue;

    if (!CORRECT_FSTYPE((const char *)mt->mnt_fstype))
      continue;

    QString sep = (mt->mnt_mountp[strlen(mt->mnt_mountp) - 1] == '/')  ? 
	QString::null : QString::fromLatin1("/");
    quotafilename = QString::fromLatin1("%1%2%3")
                          .arg(QString::fromLatin1(mt->mnt_mountp))
                          .arg(sep)
                          .arg(QString::fromLatin1(_KU_QUOTAFILENAME));
#elif defined(BSD)
  while ((mt=getfsent()) != NULL) {
    if (strstr(mt->fs_mntops,"quota")==NULL)
      continue;
    if (strcasecmp(mt->fs_vfstype,"ufs") != 0)
      continue;
    quotafilename = QString::fromLatin1("%1%2%3")
                          .arg(QString::fromLatin1(mt->fs_file))
                          .arg((mt->fs_file[strlen(mt->fs_file) -1] == '/') ?
			  	QString::null : QString::fromLatin1("/"))
			  .arg(QString::fromLatin1(_KU_QUOTAFILENAME));
#elif defined(_AIX)
  while ((vt=getvfsent()) != NULL) {
    /* The prototype of getfstype() is botched (old K&R without args).
     * Or alternatively the man page is, in which case this code
     * is trash. */
    while ((mt=(*(getfstype_proto)&getfstype)(vt->vfsent_name)) != NULL) {
      if (strstr(mt->fs_spec,FSTAB_RQ)==NULL)
        continue;
      // if (strcasecmp(mt->fs_vfstype,"ufs") != 0) continue;
      quotafilename = QString("%1%2%3")
                            .arg(mt->fs_file)
                            .arg((mt->fs_file[strlen(mt->fs_file) -1] == '/') ? "" : "/")
                            .arg(_KU_QUOTAFILENAME);
#else
  fp = setmntent(MNTTAB, "r");
  while ((mt = getmntent(fp)) != (struct mntent *)0) {
    if (strstr(mt->mnt_opts, "quota") == NULL)
      continue;

    if (!CORRECT_FSTYPE((const char *)mt->mnt_type))
      continue;

    quotafilename = QString::fromLatin1("%1%2%3")
                          .arg(QFile::decodeName(mt->mnt_dir))
                          .arg((mt->mnt_dir[strlen(mt->mnt_dir) - 1] == '/') ? QString::null : QString::fromLatin1("/"))
                          .arg(QFile::decodeName(_KU_QUOTAFILENAME));

#endif

    QFile f(quotafilename);
    if (f.exists() == FALSE) {
      printf("Quota file name %s does not exist\n", quotafilename.local8Bit().data());
      continue;
    }
    printf("Quota file name %s found\n", quotafilename.local8Bit().data());
#ifdef OLD_GETMNTENT
    mnt = new MntEnt(QString::fromLocal8Bit(mt->mnt_special), QString::fromLocal8Bit(mt->mnt_mountp), QString::fromLocal8Bit(mt->mnt_fstype),
                     QString::fromLocal8Bit(mt->mnt_mntopts), quotafilename);
    m.append(mnt);
    is_quota = 1;
  }
  fclose(fp);
#elif defined(BSD)
    mnt = new MntEnt(QString::fromLocal8Bit(mt->fs_spec), QString::fromLocal8Bit(mt->fs_file), QString::fromLocal8Bit(mt->fs_vfstype),
		     QString::fromLocal8Bit(mt->fs_mntops), quotafilename);
    m.append(mnt);
    is_quota = 1;
  }
  endfsent();
#elif defined(_AIX)
      /* Are the mount options parsed away by the mount helpers?
       * I can't find them in the structures.
       */
      mnt = new MntEnt(QString::fromLocal8Bit(mt->fs_spec), QString::fromLocal8Bit(mt->fs_file), QString::fromLocal8Bit(vt->vfsent_name),
		       QString::null, quotafilename);
      m.append(mnt);
      is_quota = 1;
    }
    endfsent();
  }
  endvfsent();
#else
    mnt = new MntEnt(QString::fromLocal8Bit(mt->mnt_fsname), QString::fromLocal8Bit(mt->mnt_dir), QString::fromLocal8Bit(mt->mnt_type),
                     QString::fromLocal8Bit(mt->mnt_opts), quotafilename);
    m.append(mnt);
    is_quota = 1;
  }
  endmntent(fp);
#endif
}

Mounts::~Mounts() {
  m.clear();
}

MntEnt *Mounts::operator[](uint num) {
  return (m.at(num));
}

uint Mounts::getMountsNumber() {
  return (m.count());
}
#endif
