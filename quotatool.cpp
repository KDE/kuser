#ifdef _XU_QUOTA

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/file.h>
#include "config.h"

#ifdef HAVE_LINUX_QUOTA_H
#include <linux/quota.h>
#endif

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

//_syscall4(int, quotactl, int, cmd, const char *, special, int, id, caddr_t, addr);
#endif

#ifdef HAVE_SYS_FS_UFS_QUOTA_H
#include <sys/fs/ufs_quota.h>
#define CORRECT_FSTYPE(type) (!strcmp(type,MNTTYPE_UFS))
#define QUOTAFILENAME "quotas"
#else
#define CORRECT_FSTYPE(type) (!strcmp(type,MNTTYPE_EXT2))
#define QUOTAFILENAME "quota.user"
#endif

char s[120];

#define   FOUND   0x01

// all procedures
void getquota(const char *uname, QList<Quota> *q);
void setquota(const char *uname, QList<Quota> *q);
int getentry(const char *name);
int alldigits(const char *s);

void getquota(const char *uname, QList<Quota> *q)
{
  long id;

  if (is_quota == 0)
    return;

  if ((id = getentry(uname)) == -1)
    return;
      
  int qcmd, fd;
  static int warned = 0;
  //  extern int errno;
  struct dqblk dq;
#ifdef HAVE_SYS_FS_UFS_QUOTA_H
  struct quotctl qctl;
  int dd = 0;
#endif

  if (is_quota == 0)
    return;

#ifdef _KU_DEBUG
printf("getquota\n");
#endif

#ifdef HAVE_SYS_FS_UFS_QUOTA_H
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
	  //          QMessageBox::message(_("Error"), _("Quotas are not compiled into this kernel."), "Ok");
          printf("errno: %i, ioctl: %i\n", errno, dd);
          sleep(3);
          is_quota = 0;
          close(fd);
	  break;
      }
    q->append(new Quota(dbtob(dq.dqb_curblocks),
                        dbtob(dq.dqb_bsoftlimit),
                        dbtob(dq.dqb_bhardlimit),
                        dq.dqb_curfiles,
                        dq.dqb_fsoftlimit,
                        dq.dqb_fhardlimit));
    close(fd);
  }
#else
  qcmd = QCMD(Q_GETQUOTA, USRQUOTA);

  for (uint i=0; i<mounts.count(); i++) {
      if (quotactl(qcmd, (const char *)mounts.at(i)->fsname, id, (caddr_t) &dq) != 0) {
/*
        if ((errno == EOPNOTSUPP || errno == ENOSYS) && !warned) {
*/
          warned++;
          QMessageBox::message(_("Error"), _("Quotas are not compiled into this kernel."), "Ok");
          sleep(3);
          is_quota = 0;
	  break;
/*
        }
*/
      }
      q->append(new Quota(dbtob(dq.dqb_curblocks),
                          dbtob(dq.dqb_bsoftlimit),
        		  dbtob(dq.dqb_bhardlimit),
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

/*
  if (id = getentry(uname) == -1)
    return;
*/

  int qcmd, fd;
  struct dqblk dq;

#ifdef HAVE_SYS_FS_UFS_QUOTA_H
  struct quotctl qctl;
  int dd = 0;
#endif

#ifdef _KU_DEBUG
//printf("setquota\n");
#endif

#ifdef HAVE_SYS_FS_UFS_QUOTA_H
    qctl.op = Q_SETQUOTA;
    qctl.uid = id;
    qctl.addr = (caddr_t) &dq;

  for (uint i=0; i<mounts.count(); i++) {
    dq.dqb_curblocks  = btodb(q->at(i)->fcur);
    dq.dqb_bsoftlimit = btodb(q->at(i)->fsoft);
    dq.dqb_bhardlimit = btodb(q->at(i)->fhard);
    dq.dqb_curfiles  = q->at(i)->icur;
    dq.dqb_fsoftlimit = q->at(i)->isoft;
    dq.dqb_fhardlimit = q->at(i)->ihard;
    dq.dqb_btimelimit = DQ_BTIMELIMIT;
    dq.dqb_ftimelimit = DQ_FTIMELIMIT;
    
    if (id == 404) {
      printf("id = %d\n%d %d %d\n%d %d %d\n", id, q->at(i)->fcur, q->at(i)->fsoft, q->at(i)->fhard,
      q->at(i)->icur, q->at(i)->isoft, q->at(i)->ihard);
    }

    fd = open((const char *)mounts.at(i)->quotafilename, O_WRONLY);

    if ((dd = ioctl(fd, Q_QUOTACTL, &qctl)) != 0)
      if (errno == ESRCH) {
//      printf("Warning ESRCH %il \n", id);
      }
      else
      {
//        printf("errno: %i, ioctl: %i\n", errno, dd);
        sleep(3);
        is_quota = 0;
        close(fd);
        break;
      }
    qctl.op = Q_SYNC;
//    printf("ioctl %d\n", ioctl(fd, Q_QUOTACTL, &qctl));

    close(fd);
  }
#else
  qcmd = QCMD(Q_SETQUOTA, USRQUOTA);
  for (uint i=0; i<mounts.count(); i++) {
    dq.dqb_curblocks  = btodb(q->at(i)->fcur);
    dq.dqb_bsoftlimit = btodb(q->at(i)->fsoft);
    dq.dqb_bhardlimit = btodb(q->at(i)->fhard);
    dq.dqb_curinodes  = q->at(i)->icur;
    dq.dqb_isoftlimit = q->at(i)->isoft;
    dq.dqb_ihardlimit = q->at(i)->ihard;

    if (quotactl(qcmd, (const char *)mounts.at(i)->fsname, id, (caddr_t) &dq) != 0)
      continue;

    if ((fd = open((const char *)mounts.at(i)->quotafilename, O_WRONLY)) < 0) {
      sprintf(s, _("Error opening %s"), (const char *)mounts.at(i)->quotafilename);
      QMessageBox::message(_("Error"), s, "Ok");
    }
    else {
      lseek(fd, (long) id * (long) sizeof(struct dqblk), 0);
      if (write(fd, &dq, sizeof(struct dqblk)) != sizeof(struct dqblk)) {
        sprintf(s, _("Error writing %s"), (const char *)mounts.at(i)->quotafilename);
        QMessageBox::message(_("Error"), s ,"Ok");
      }
      close(fd);
    }
  }
#endif
}

/*
 * This routine converts a name for a particular quota type to an identifier.
 */
int getentry(const char *name)
{
  if (is_quota != 0) {
    struct passwd  *pw;
    struct group   *gr;

    if (alldigits(name))
      return (atoi(name));
    if (pw = getpwnam(name))
      return (pw->pw_uid);

    sprintf(s, _("%s: no such user"), name);
    QMessageBox::message(_("Error") ,s ,"Ok");  

    sleep(1);
  }
  return (-1);
}

/*
 * Check whether a string is completely composed of digits.
 */
int alldigits(const char *s)
{
  int c;

  c = *s++;
  do {
    if (!isdigit(c))
      return (0);
  } while (c = *s++);
  return (1);
}

void quota_read() {
  if (is_quota == 0)
    return;

  for (uint i=0; i<users.count(); i++) {
printf("%d\n",i);
  if (is_quota == 0)
    return;
    getquota(users.at(i)->p_name, &users.at(i)->quota);
  }
}

void quota_write() {
  if (is_quota == 0)
    return;

#ifdef _KU_DEBUG
printf("quota_write\n");
#endif

  for (uint i=0; i<users.count(); i++) {
#ifdef _KU_DEBUG
//printf("setquota %s -- %li\n", (const char *)users.at(i)->p_name, users.at(i)->p_uid);
#endif
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

#endif

