#ifdef _KU_QUOTA

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/file.h>
#include "../config.h"

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <errno.h>

#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif

#ifdef HAVE_SYS_MNTENT_H
#include <sys/mntent.h>
#endif

#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <limits.h>

#ifdef HAVE_PATH_H
#include <paths.h>
#endif

#include "includes.h"
#include "misc.h"

#if defined(__alpha__)
#include <syscall.h>
#include <asm/unistd.h>

int quotactl(int cmd, const char * special, int id, caddr_t addr)
{
  return syscall(__NR_quotactl, cmd, special, id, addr);
}
#else
#include <sys/types.h>
#define __LIBRARY__

#ifdef HAVE_LINUX_UNISTD_H
#include <linux/unistd.h>
#endif

#endif

#include <kmsgbox.h>

char s[120];

#define   FOUND   0x01

void getquota(long int id, QList<Quota> *q)
{
  if (is_quota == 0)
    return;

  int fd;
  static int warned = 0;
  struct dqblk dq;
#ifdef _KU_UFS_QUOTA
  struct quotctl qctl;
  int dd = 0;
#endif

  if (is_quota == 0)
    return;

#ifdef _KU_UFS_QUOTA
  for (uint i=0; i<mounts.count(); i++) {
    qctl.op = Q_GETQUOTA;
    qctl.uid = id;
    qctl.addr = (caddr_t) &dq;

    fd = open((const char *)mounts.at(i)->quotafilename, O_RDONLY);

    if ((dd = ioctl(fd, Q_QUOTACTL, &qctl)) != 0)
      if (errno == ESRCH) {
        q->append(new Quota(0,0,0,0,0,0));
        close(fd);
        continue;
      }
      else
      {
/*
        if ((errno == EOPNOTSUPP || errno == ENOSYS) && !warned) {
*/
          warned++;
	  //                    KMsgBox::message(0, _("Error"), _("Quotas are not compiled into this kernel."), KMsgBox::STOP);
          printf("errno: %i, ioctl: %i\n", errno, dd);
          sleep(3);
          is_quota = 0;
          close(fd);
	  break;
      }
    q->append(new Quota(dbtob(dq.dqb_curblocks)/1024,
                        dbtob(dq.dqb_bsoftlimit)/1024,
                        dbtob(dq.dqb_bhardlimit)/1024,
                        dq.dqb_curfiles,
                        dq.dqb_fsoftlimit,
                        dq.dqb_fhardlimit));
    close(fd);
  }
#endif

#ifdef _KU_EXT2_QUOTA
  int qcmd = QCMD(Q_GETQUOTA, USRQUOTA);

  for (uint i=0; i<mounts.count(); i++) {
      if (quotactl(qcmd, (const char *)mounts.at(i)->fsname, id, (caddr_t) &dq) != 0) {
/*
        if ((errno == EOPNOTSUPP || errno == ENOSYS) && !warned) {
*/
          warned++;
          KMsgBox::message(0, _("Error"), _("Quotas are not compiled into this kernel."), KMsgBox::STOP);
          sleep(3);
          is_quota = 0;
	  break;
/*
        }
*/
      }
      q->append(new Quota(dbtob(dq.dqb_curblocks)/1024,
                          dbtob(dq.dqb_bsoftlimit)/1024,
        		  dbtob(dq.dqb_bhardlimit)/1024,
			  dq.dqb_curinodes,
			  dq.dqb_isoftlimit,
			  dq.dqb_ihardlimit));
    }
#endif
}

void setquota(long int id, QList<Quota> *q) 
{
  if (is_quota == 0)
    return;

  int fd;
  struct dqblk dq;

#ifdef _KU_UFS_QUOTA
  struct quotctl qctl;
  int dd = 0;

  qctl.op = Q_SETQUOTA;
  qctl.uid = id;
  qctl.addr = (caddr_t) &dq;

  for (uint i=0; i<mounts.count(); i++) {
    dq.dqb_curblocks  = btodb(q->at(i)->fcur*1024);
    dq.dqb_bsoftlimit = btodb(q->at(i)->fsoft*1024);
    dq.dqb_bhardlimit = btodb(q->at(i)->fhard*1024);
    dq.dqb_curfiles  = q->at(i)->icur;
    dq.dqb_fsoftlimit = q->at(i)->isoft;
    dq.dqb_fhardlimit = q->at(i)->ihard;
    dq.dqb_btimelimit = DQ_BTIMELIMIT;
    dq.dqb_ftimelimit = DQ_FTIMELIMIT;
    
    fd = open((const char *)mounts.at(i)->quotafilename, O_WRONLY);

    if ((dd = ioctl(fd, Q_QUOTACTL, &qctl)) != 0)
      if (errno == ESRCH) {
//      printf("Warning ESRCH %il \n", id);
      }
      else
      {
        sleep(3);
        is_quota = 0;
        close(fd);
        break;
      }
    close(fd);
  }
#endif

#ifdef _KU_EXT2_QUOTA
  int dd = 0;
  int qcmd = QCMD(Q_SETQUOTA, USRQUOTA);
  for (uint i=0; i<mounts.count(); i++) {
    dq.dqb_curblocks  = btodb(q->at(i)->fcur*1024);
    dq.dqb_bsoftlimit = btodb(q->at(i)->fsoft*1024);
    dq.dqb_bhardlimit = btodb(q->at(i)->fhard*1024);
    dq.dqb_curinodes  = q->at(i)->icur;
    dq.dqb_isoftlimit = q->at(i)->isoft;
    dq.dqb_ihardlimit = q->at(i)->ihard;
    dq.dqb_btime = MAX_DQ_TIME;
    dq.dqb_itime = MAX_IQ_TIME;

    if ((dd =quotactl(qcmd, (const char *)mounts.at(i)->fsname, id, (caddr_t) &dq)) != 0) {
      printf("Quotactl returned: %d\n", dd);
      continue;
    }
  }
#endif
}

void quota_read() {
  if (is_quota == 0)
    return;

  for (uint i=0; i<users.count(); i++) {
    getquota(users.at(i)->p_uid, &users.at(i)->quota);
    if (is_quota == 0)
      return;
  }
}

void quota_write() {
  if (is_quota == 0)
    return;

  for (uint i=0; i<users.count(); i++) {
    setquota(users.at(i)->p_uid, &users.at(i)->quota);
  }
}

Quota::Quota() {
  fcur = 0; fsoft = 0; fhard = 0;
  icur = 0; isoft = 0; ihard = 0;
}
  
Quota::Quota(int afcur, int afsoft, int afhard, int aicur, int aisoft, int aihard) {
  fcur = afcur; fsoft = afsoft; fhard = afhard;
  icur = aicur; isoft = aisoft; ihard = aihard;
}

Quota& Quota::operator= (const Quota& q) {
  fcur = q.fcur; fsoft = q.fsoft; fhard = q.fhard;
  icur = q.icur; isoft = q.isoft; ihard = q.ihard;
}

Quota::Quota(const Quota *q) {
  fcur = q->fcur; fsoft = q->fsoft; fhard = q->fhard;
  icur = q->icur; isoft = q->isoft; ihard = q->ihard;
}

#endif // _KU_QUOTA
