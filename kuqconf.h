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
#elif defined(HAVE_LINUX)
#    if defined __GLIBC__ && __GLIBC__ >= 2
       typedef unsigned int __u32;
#      define MNTTYPE_EXT2 "ext2"
#    endif
#    ifdef HAVE_SYS_QUOTA_H
#      include <sys/quota.h>
#    elif defined(HAVE_LINUX_QUOTA_H)
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
#  elif defined(HAVE_IRIX)
#    include <sys/quota.h>
#    include <sys/param.h>
#    define CORRECT_FSTYPE(type) (!strcmp(type,MNTTYPE_EFS) || !strcmp(type,MNTTYPE_XFS))
#    define _KU_QUOTAFILENAME "quotas"
#  elif defined(__osf__)                /* OSF1, aka Digital Unix, aka Tru64 Unix */
#      undef _POSIX_SOURCE
#      define _OSF_SOURCE
#      include <machine/machparam.h> /* for dbtob and the like */
#      include <sys/types.h>
#      include <ufs/quota.h>
#      include <fstab.h>
#      define _KU_QUOTAFILENAME "quota.user"
#  elif defined(BSD) /* I'm not 100% sure about this, but it should work with Net/OpenBSD */
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
#  elif defined(_AIX)
#    include <fstab.h>
#    include <sys/types.h>
#    include <jfs/quota.h>
#    include <sys/vfs.h>
#    include <sys/vmount.h>
#    include <fshelp.h>
#    define _KU_QUOTAFILENAME "quotas"
     typedef struct fstab *(*getfstype_proto)(char* type);
#  else
#        error "Your platform is not supported"
#endif // HAVE_SYS_FS_UFS_QUOTA_H

#ifdef HAVE_IF_DQBLK
#  define STRUCT_DQBLK struct if_dqblk
#else
#  define STRUCT_DQBLK struct dqblk
#endif

#endif
