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
  struct mnttab *m = NULL;
#elif __FreeBSD__
  struct fstab *m = NULL;
#else
  struct mntent *m = NULL;
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
  m = (struct mnttab *)malloc(sizeof(mnttab));

  while ((getmntent(fp, m)) == 0) {
    if (strstr(m->mnt_mntopts, "quota") == NULL)
      continue;

    if (!CORRECT_FSTYPE((const char *)m->mnt_fstype))
      continue;

    quotafilename.sprintf("%s%s%s", m->mnt_mountp,
                          (m->mnt_mountp[strlen(m->mnt_mountp) - 1] == '/') ? "" : "/",
                          _KU_QUOTAFILENAME);
#elif __FreeBSD__ /* Heh, heh, so much for standards, eh FreeBSD? */
  while ((m=getfsent()) != NULL) {
    if (strstr(m->fs_mntops,"quota")==NULL)
      continue;
    if (strcasecmp(m->fs_vfstype,"ufs") != 0)
      continue;
    quotafilename.sprintf("%s%s%s",m->fs_file,
 	                  (m->fs_file[strlen(m->fs_file) -1] == '/') ? "" : "/",
 		          _KU_QUOTAFILENAME);
    m=getfsent();
#else
  fp = setmntent(MNTTAB, "r");
  while ((m = getmntent(fp)) != (struct mntent *)0) {
    if (strstr(m->mnt_opts, "quota") == NULL)
      continue;

    if (!CORRECT_FSTYPE((const char *)m->mnt_type))
      continue;

    quotafilename.sprintf("%s%s%s", m->mnt_dir,
                          (m->mnt_dir[strlen(m->mnt_dir) - 1] == '/') ? "" : "/",
                          _KU_QUOTAFILENAME);
#endif

    QFile *f = new QFile(quotafilename);
    if (f->exists() == FALSE)
      continue;

#ifdef OLD_GETMNTENT
    mnt = new MntEnt(m->mnt_special, m->mnt_mountp, m->mnt_fstype,
                     m->mnt_mntopts, quotafilename);
#elif __FreeBSD__
  mnt = new MntEnt(m->fs_spec,m->fs_file,m->fs_vfstype,
		   m->fs_mntops,quotafilename);
#else
    mnt = new MntEnt(m->mnt_fsname, m->mnt_dir, m->mnt_type,
                     m->mnt_opts, quotafilename);
#endif
    Mounts::m.append(mnt);
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
