#ifdef _KU_QUOTA
#include <stdlib.h>
#include <stdio.h>
#include "mnt.h"
#include "quota.h"

MntEnt::MntEnt(const char *afsname, const char *adir,
               const char *atype, const char *aopts,
               const char *aquotafilename){
    fsname.setStr(afsname);
    dir.setStr(adir);
    type.setStr(atype);
    opts.setStr(aopts);
    quotafilename.setStr(aquotafilename);
}

MntEnt::~MntEnt() {
}

Mounts::Mounts() {
  Mounts::m.setAutoDelete(TRUE);

#ifdef OLD_GETMNTENT
  struct mnttab *mt = NULL;
#elif __FreeBSD__
  struct fstab *mt = NULL;
#else
  struct mntent *mt = NULL;
#endif
#ifndef __FreeBSD__
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

    quotafilename.sprintf("%s%s%s", mt->mnt_mountp,
                          (mt->mnt_mountp[strlen(mt->mnt_mountp) - 1] == '/') ? "" : "/",
                          _KU_QUOTAFILENAME);
#elif __FreeBSD__ /* Heh, heh, so much for standards, eh FreeBSD? */
  while ((mt=getfsent()) != NULL) {
    if (strstr(mt->fs_mntops,"quota")==NULL)
      continue;
    if (strcasecmp(mt->fs_vfstype,"ufs") != 0)
      continue;
    quotafilename.sprintf("%s%s%s",mt->fs_file,
 	                  (mt->fs_file[strlen(mt->fs_file) -1] == '/') ? "" : "/",
 		          _KU_QUOTAFILENAME);
#else
  fp = setmntent(MNTTAB, "r");
  while ((mt = getmntent(fp)) != (struct mntent *)0) {
    if (strstr(mt->mnt_opts, "quota") == NULL)
      continue;

    if (!CORRECT_FSTYPE((const char *)mt->mnt_type))
      continue;

    quotafilename.sprintf("%s%s%s", mt->mnt_dir,
                          (mt->mnt_dir[strlen(mt->mnt_dir) - 1] == '/') ? "" : "/",
                          _KU_QUOTAFILENAME);
#endif

    QFile *f = new QFile(quotafilename);
    if (f->exists() == FALSE)
      continue;

#ifdef OLD_GETMNTENT
    mnt = new MntEnt(mt->mnt_special, mt->mnt_mountp, mt->mnt_fstype,
                     mt->mnt_mntopts, quotafilename);
#elif __FreeBSD__
  mnt = new MntEnt(mt->fs_spec,mt->fs_file,mt->fs_vfstype,
		   mt->fs_mntops,quotafilename);
#else
    mnt = new MntEnt(mt->mnt_fsname, mt->mnt_dir, mt->mnt_type,
                     mt->mnt_opts, quotafilename);
#endif
    m.append(mnt);
    is_quota = 1;
  }
#ifdef OLD_GETMNTENT
  fclose(fp);
#elif __FreeBSD__
  endfsent();
#else
  endmntent(fp);
#endif
}

Mounts::~Mounts() {
  m.clear();
}

MntEnt *Mounts::getMount(uint num) {
  return (m.at(num));
}

uint Mounts::getMountsNumber() {
  return (m.count());
}
#endif
