#ifdef _XU_QUOTA

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <linux/quota.h>
#include <errno.h>
#include <mntent.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <paths.h>
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
#include <linux/unistd.h>

_syscall4(int, quotactl, int, cmd, const char *, special, int, id, caddr_t, addr);
#endif

#ifdef __cplusplus
extern "C" {
unsigned int sleep __P ((unsigned int __seconds));
int fchown __P ((int __fd, __uid_t __owner, __gid_t __group));
__off_t lseek __P ((int __fd, __off_t __offset, int __whence));
ssize_t read __P ((int __fd, __ptr_t __buf, size_t __nbytes));
ssize_t write __P ((int __fd, __const __ptr_t __buf, size_t __n));
int close __P ((int __fd));
}
#endif

#define CORRECT_FSTYPE(type) \
(!strcmp(type,MNTTYPE_EXT2))

char *qfextension[] = INITQFNAMES;
static char *qfname = QUOTAFILENAME;
static char qfullname[PATH_MAX];

char *quotagroup = QUOTAGROUP;

char s[120];

#define   FOUND   0x01

// all procedures
int hasquota(MntEnt *mnt, int type, char **qfnamep);
void getquota(const char *uname, QList<Quota> *q);
void getmntprivs(long id, QList<Quota> *q);
void setquota(const char *uname, QList<Quota> *q);
void setmntprivs(long id, QList<Quota> *q);
int getentry(const char *name, int quotatype);
int alldigits(const char *s);

int isquotainkernel() {
  int qcmd, fd;
  char *qfpathname = NULL;
  static int warned = 0;
  extern int errno;
  struct dqblk dq;

  if (is_quota == 0)
    return;

  qcmd = QCMD(Q_GETQUOTA, USRQUOTA);

  if (quotactl(qcmd, (const char *)mounts.at(0)->fsname, 0, (caddr_t) &dq) != 0) {
    if ((errno == EOPNOTSUPP || errno == ENOSYS) && !warned) {
      warned++;
      QMessageBox::message(_("Error"), _("Quotas are not compiled into this kernel."), "Ok");
      sleep(3);
      is_quota = 0;
    }
  }

  return (is_quota);
}

int hasquota(struct MntEnt *mnt, int type, char **qfnamep)
{
  char *buf, *option, *pathname;

  if (is_quota != 0) {
    if (!CORRECT_FSTYPE((const char *)mnt->type))
      return (0);
    if ((type == USRQUOTA) 
//    && (option = hasmntopt(mnt, MNTOPT_USRQUOTA)) != (char *)0
    ) {
//      if ((pathname = strchr(option, '=')) == (char *)0) {
        (void) sprintf(qfullname, "%s%s%s.%s", (const char *)mnt->dir,
                       (mnt->dir[mnt->dir.length() - 1] == '/') ? "" : "/",
                        qfname, qfextension[type]);
//      } else {
         /*
          * Splice this option on the start of any following option.
          */
//         if ((option = strchr(++pathname, ',')) != (char *)NULL)
//           *option = '\0';
//         strncpy(qfullname, pathname, sizeof(qfullname));
//      }
      *qfnamep = strdup(qfullname);
      return (1);
    } else
      return (0);
  }
  return (0);
}

void getquota(const char *uname, QList<Quota> *q)
{
  long id;

  if (is_quota != 0)
    if ((id = getentry(uname, USRQUOTA)) != -1) {
      getmntprivs(id, q);
  }
}

void setquota(const char *uname, QList<Quota> *q) 
{
  long id;

  if (is_quota != 0)
    if (id = getentry(uname, USRQUOTA) != -1)
      setmntprivs(id, q);
}

/*
 * This routine converts a name for a particular quota type to an identifier.
 */
int getentry(const char *name, int quotatype)
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

void getmntprivs(long id, QList<Quota> *q)
{
  int qcmd, fd;
  char *qfpathname = NULL;
  static int warned = 0;
  extern int errno;
  struct dqblk dq;

  if (is_quota == 0)
    return;

  qcmd = QCMD(Q_GETQUOTA, USRQUOTA);

  for (uint i=0; i<mounts.count(); i++) {
      if (!hasquota(mounts.at(i), USRQUOTA, &qfpathname))
        continue;

      if (quotactl(qcmd, (const char *)mounts.at(i)->fsname, id, (caddr_t) &dq) != 0) {
        if ((errno == EOPNOTSUPP || errno == ENOSYS) && !warned) {
          warned++;
          QMessageBox::message(_("Error"), _("Quotas are not compiled into this kernel."), "Ok");
          sleep(3);
          is_quota = 0;
        }
        if ((fd = open(qfpathname, O_RDONLY)) < 0) {
          fd = open(qfpathname, O_RDWR | O_CREAT, 0640);
          if (fd < 0 && errno != ENOENT) {
            sprintf(s, _("Error opening %s"), qfpathname);
            QMessageBox::message(_("Error"), s, "Ok");
            continue;
          }
          sprintf(s, _("Creating quota file %s"), qfpathname);
          QMessageBox::message(_("Warning"), s, "Ok");
          sleep(3);
          (void) fchown(fd, getuid(), getentry(quotagroup, GRPQUOTA));
          (void) fchmod(fd, 0640);
        }

        lseek(fd, (long) (id * sizeof(struct dqblk)), L_SET);
        switch (read(fd, &dq, sizeof(struct dqblk))) {
          case 0:/* EOF */
               /*
                * Convert implicit 0 quota (EOF) into an
                * explicit one (zero'ed dqblk)
                */
               bzero((caddr_t) &dq,
                     sizeof(struct dqblk));
               break;

          case sizeof(struct dqblk):   /* OK */
               break;

          default:   /* ERROR */
               sprintf(s, _("Error reading %s"), qfpathname);
               QMessageBox::message(_("Error"), s, "Ok");
               close(fd);
               continue;
        }
        close(fd);
      }
      q->append(new Quota(dbtob(dq.dqb_curblocks),
                          dbtob(dq.dqb_bsoftlimit),
        		  dbtob(dq.dqb_bhardlimit),
			  dq.dqb_curinodes,
			  dq.dqb_isoftlimit,
			  dq.dqb_ihardlimit));
    }
}

void setmntprivs(long id, QList<Quota> *q) {
  int qcmd, fd;
  struct dqblk dq;
  char *qfpathname;

  qcmd = QCMD(Q_SETQUOTA, USRQUOTA);
  if (is_quota != 0) {
    for (uint i=0; i<mounts.count(); i++) {
      if (!hasquota(mounts.at(i), USRQUOTA, &qfpathname))
        continue;

      dq.dqb_curblocks  = btodb(q->at(i)->fcur);
      dq.dqb_bsoftlimit = btodb(q->at(i)->fsoft);
      dq.dqb_bhardlimit = btodb(q->at(i)->fhard);
      dq.dqb_curinodes  = q->at(i)->icur;
      dq.dqb_isoftlimit = q->at(i)->isoft;
      dq.dqb_ihardlimit = q->at(i)->ihard;

      if (quotactl(qcmd, (const char *)mounts.at(i)->fsname, id, (caddr_t) &dq) != 0) {
        continue;
        if ((fd = open(qfpathname, O_WRONLY)) < 0) {
          sprintf(s, _("Error opening %s"), qfpathname);
          QMessageBox::message(_("Error"), s, "Ok");
        }
        else {
          lseek(fd, (long) id * (long) sizeof(struct dqblk), 0);
          if (write(fd, &dq, sizeof(struct dqblk)) != sizeof(struct dqblk)) {
            sprintf(s, _("Error writing %s"), qfpathname);
            QMessageBox::message(_("Error"), s ,"Ok");
          }
          close(fd);
        }
      }
    }
  }
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
    getquota(users.at(i)->p_name, &users.at(i)->quota);
  }
}

void quota_write() {
  if (is_quota == 0)
    return;

  for (uint i=0; i<users.count(); i++)
    setquota(users.at(i)->p_name, &users.at(i)->quota);
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

