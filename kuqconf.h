#ifdef _KU_QUOTA

#include "globals.h"

#include <sys/file.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif

#ifdef HAVE_SYS_MNTENT_H
#include <sys/mntent.h>
#define OLD_GETMNTENT
#endif

#ifdef HAVE_SYS_MNTTAB_H
#include <sys/mnttab.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_SYS_FS_UFS_QUOTA_H
#  include <sys/fs/ufs_quota.h>
#  define CORRECT_FSTYPE(type) (!strcmp(type,MNTTYPE_UFS))
#  define _KU_QUOTAFILENAME "quotas"
#  define _KU_UFS_QUOTA
#elif HAVE_LINUX
#    if defined __GLIBC__ && __GLIBC__ >= 2
       typedef unsigned int __u32;
#      define MNTTYPE_EXT2 "ext2"
#    endif
#    ifdef HAVE_SYS_QUOTA_H
#      include <sys/quota.h>
#    elif HAVE_LINUX_QUOTA_H
#      include <linux/quota.h>
#    else
#      error "Cannot find your quota.h"
#    endif

#    ifdef HAVE_SYS_MOUNT_H
#      include <sys/mount.h>
#    endif

#    define CORRECT_FSTYPE(type) (!strcmp(type,MNTTYPE_EXT2))
#    define _KU_QUOTAFILENAME "quota.user"
#    define _KU_EXT2_QUOTA
#  elif HAVE_IRIX
#    include <sys/quota.h>
#    include <sys/param.h>
#    define CORRECT_FSTYPE(type) (!strcmp(type,MNTTYPE_EFS) || !strcmp(type,MNTTYPE_XFS))
#    define _KU_QUOTAFILENAME "quotas"
#  elif __osf__                /* OSF1, aka Digital Unix, aka Tru64 Unix */
#      undef _POSIX_SOURCE
#      define _OSF_SOURCE
#      include <machine/machparam.h> /* for dbtob and the like */
#      include <sys/types.h>
#      include <ufs/quota.h>
#      include <fstab.h>
#      define _KU_QUOTAFILENAME "quota.user"
#  elif BSD /* I'm not 100% sure about this, but it should work with Net/OpenBSD */
#      include <machine/param.h> /* for dbtob and the like */
#      include <sys/types.h>
#      include <ufs/ufs/quota.h>
#      include <fstab.h>
/* Assuming all we're supporting is USER based quotas, not GROUP based ones */
#      define _KU_QUOTAFILENAME "quota.user"
#  elif __hpux
#        include <sys/quota.h>
#        define CORRECT_FSTYPE(type) (!strcmp(type,MNTTYPE_HFS))
#        define _KU_QUOTAFILENAME "quotas"
#        define _KU_HPUX_QUOTA
#  else
#        error "Your platform is not supported"
#endif // HAVE_SYS_FS_UFS_QUOTA_H

#endif
