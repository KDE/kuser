#ifdef _KU_QUOTA

#include <kmsgbox.h>

#include "misc.h"
#include "maindlg.h"
#include "mnt.h"
#include "quota.h"

#ifdef HAVE_LINUX_QUOTA_H
#  ifndef QUOTACTL_IN_LIBC
#    include <syscall.h>

int quotactl(int cmd, const char * special, int id, caddr_t addr) {
  return syscall(SYS_quotactl, cmd, special, id, addr);
}

#  endif
#endif

QuotaMnt::QuotaMnt() {
  fcur = 0; fsoft = 0; fhard = 0; ftime = 0;
  icur = 0; isoft = 0; ihard = 0; itime = 0;
}
  
QuotaMnt::QuotaMnt(int afcur, int afsoft, int afhard, int aicur, int aisoft, int aihard) {
  fcur = afcur; fsoft = afsoft; fhard = afhard; ftime = 0;
  icur = aicur; isoft = aisoft; ihard = aihard; itime = 0;
}

QuotaMnt::QuotaMnt(int afcur, int afsoft, int afhard, int aicur, int aisoft, int aihard, int aftime, int aitime) {
  fcur = afcur; fsoft = afsoft; fhard = afhard; ftime = aftime;
  icur = aicur; isoft = aisoft; ihard = aihard; itime = aitime;
}

QuotaMnt& QuotaMnt::operator= (const QuotaMnt &q) {
  fcur = q.fcur; fsoft = q.fsoft; fhard = q.fhard; ftime = q.ftime;
  icur = q.icur; isoft = q.isoft; ihard = q.ihard; itime = q.itime;
  return (*this);
}

QuotaMnt::~QuotaMnt() {
}

QuotaMnt::QuotaMnt(const QuotaMnt *q) {
  fcur = q->fcur; fsoft = q->fsoft; fhard = q->fhard; ftime = q->ftime;
  icur = q->icur; isoft = q->isoft; ihard = q->ihard; itime = q->itime;
}

Quota::Quota(unsigned int auid, bool doget) {
  uid = auid;

  q.setAutoDelete(TRUE);

  if (is_quota == 0)
    return;

  if (!doget) {
    for (uint i=0; i<mounts->getMountsNumber(); i++)
      q.append(new QuotaMnt);
    return;
  }

  static int warned = 0;
  struct dqblk dq;
#ifdef _KU_UFS_QUOTA
  int fd;
  struct quotctl qctl;
  int dd = 0;
#endif

  if (is_quota == 0)
    return;

#ifdef _KU_UFS_QUOTA
  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    qctl.op = Q_GETQUOTA;
    qctl.uid = uid;
    qctl.addr = (caddr_t) &dq;
    
    fd = open((const char *)mounts->getMount(i)->quotafilename, O_RDONLY);
    
    if ((dd = ioctl(fd, Q_QUOTACTL, &qctl)) != 0)
      if (errno == ESRCH) {
        q.append(new QuotaMnt());
        close(fd);
        continue;
      }
      else {
/*
        if ((errno == EOPNOTSUPP || errno == ENOSYS) && !warned) {
*/
	warned++;
	KMsgBox::message(0, _("Error"), _("Quotas are not compiled into this kernel."), KMsgBox::STOP);
	printf("errno: %i, ioctl: %i\n", errno, dd);
	sleep(3);
	is_quota = 0;
	close(fd);
	break;
      }
    q.append(new QuotaMnt(dbtob(dq.dqb_curblocks)/1024,
                          dbtob(dq.dqb_bsoftlimit)/1024,
                          dbtob(dq.dqb_bhardlimit)/1024,
                          dq.dqb_curfiles,
                          dq.dqb_fsoftlimit,
                          dq.dqb_fhardlimit,
                          dq.dqb_btimelimit,
                          dq.dqb_ftimelimit));
    close(fd);
  }
#endif

#ifdef _KU_EXT2_QUOTA
  int qcmd = QCMD(Q_GETQUOTA, USRQUOTA);

  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    if (quotactl(qcmd, (const char *)mounts->getMount(i)->fsname, uid, (caddr_t) &dq) != 0) {
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
      q.append(new QuotaMnt(dbtob(dq.dqb_curblocks)/1024,
			    dbtob(dq.dqb_bsoftlimit)/1024,
			    dbtob(dq.dqb_bhardlimit)/1024,
			    dq.dqb_curinodes,
			    dq.dqb_isoftlimit,
			    dq.dqb_ihardlimit,
			    dq.dqb_btime,
			    dq.dqb_itime));
  }
#endif

#ifdef HAVE_IRIX
  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    if (quotactl(Q_GETQUOTA, (const char *)mounts->getMount(i)->fsname, uid, (caddr_t) &dq) != 0) {
	if(!warned) {
            warned++;
            printf("errno: %i\n", errno);
            sleep(3);
            is_quota = 0;
	  }
	  break;
      }
      q.append(new QuotaMnt(dbtob(dq.dqb_curblocks)/1024,
                            dbtob(dq.dqb_bsoftlimit)/1024,
                            dbtob(dq.dqb_bhardlimit)/1024,
                            dq.dqb_curfiles,
                            dq.dqb_fsoftlimit,
                            dq.dqb_fhardlimit,
                            dq.dqb_btimelimit,
                            dq.dqb_ftimelimit));
  }
#endif

#ifdef __FreeBSD__
  int qcmd = QCMD(Q_GETQUOTA, USRQUOTA);

  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    if (quotactl((const char *)mounts->getMount(i)->fsname, qcmd, uid, (caddr_t) &dq) !=0) {
      //  printf("%d\n",errno);
      warned++;
      KMsgBox::message(0, _("Error"), _("Quotas are not compiled into this kernel."), KMsgBox::STOP);
      printf("errno: %i while calling quotactl\n", errno);
      sleep(3);
      is_quota = 0;
      break;
    }
    q.append(new QuotaMnt(dbtob(dq.dqb_curblocks)/1024,
			  dbtob(dq.dqb_bsoftlimit)/1024,
			  dbtob(dq.dqb_bhardlimit)/1024,
			  dq.dqb_curinodes,
			  dq.dqb_isoftlimit,
			  dq.dqb_ihardlimit,
			  dq.dqb_btime,
			  dq.dqb_btime));
  }
#endif

#ifdef _KU_HPUX_QUOTA
  int fd;
  int dd = 0;

  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    
    if ((dd = quotactl(Q_GETQUOTA, (const char *)mounts->getMount(i)->quotafilename, uid, &dq)) != 0)
      if (errno == ESRCH) {
        q.append(new QuotaMnt());
        close(fd);
        continue;
      }
      else {
/*
        if ((errno == EOPNOTSUPP || errno == ENOSYS) && !warned) {
*/
	warned++;
	KMsgBox::message(0, _("Error"), _("Quotas are not compiled into this kernel."), KMsgBox::STOP);
	printf("errno: %i, ioctl: %i\n", errno, dd);
	sleep(3);
	is_quota = 0;
	close(fd);
	break;
      }
    q.append(new QuotaMnt(dbtob(dq.dqb_curblocks)/1024,
                          dbtob(dq.dqb_bsoftlimit)/1024,
                          dbtob(dq.dqb_bhardlimit)/1024,
                          dq.dqb_curfiles,
                          dq.dqb_fsoftlimit,
                          dq.dqb_fhardlimit,
                          dq.dqb_btimelimit,
                          dq.dqb_ftimelimit));
    close(fd);
  }
#endif

}

Quota::~Quota() {
  q.clear();
}

QuotaMnt *Quota::getQuotaMnt(uint mntnum) {
  return (q.at(mntnum));
}

uint Quota::getMountsNumber() {
  return (q.count());
}

void Quota::save() {
  if (is_quota == 0)
    return;

  struct dqblk dq;

#ifdef _KU_UFS_QUOTA
  int fd;
  struct quotctl qctl;
  int dd = 0;

  qctl.op = Q_SETQUOTA;
  qctl.uid = uid;
  qctl.addr = (caddr_t) &dq;

  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    dq.dqb_curblocks  = btodb(q.at(i)->fcur*1024);
    dq.dqb_bsoftlimit = btodb(q.at(i)->fsoft*1024);
    dq.dqb_bhardlimit = btodb(q.at(i)->fhard*1024);
    dq.dqb_curfiles   = q.at(i)->icur;
    dq.dqb_fsoftlimit = q.at(i)->isoft;
    dq.dqb_fhardlimit = q.at(i)->ihard;
    dq.dqb_btimelimit = q.at(i)->ftime;
    dq.dqb_ftimelimit = q.at(i)->itime;
    
    fd = open((const char *)mounts->getMount(i)->quotafilename, O_WRONLY);

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
  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    dq.dqb_curblocks  = btodb(q.at(i)->fcur*1024);
    dq.dqb_bsoftlimit = btodb(q.at(i)->fsoft*1024);
    dq.dqb_bhardlimit = btodb(q.at(i)->fhard*1024);
    dq.dqb_curinodes  = q.at(i)->icur;
    dq.dqb_isoftlimit = q.at(i)->isoft;
    dq.dqb_ihardlimit = q.at(i)->ihard;
    dq.dqb_btime = q.at(i)->ftime;
    dq.dqb_itime = q.at(i)->itime;

    if ((dd =quotactl(qcmd, (const char *)mounts->getMount(i)->fsname, uid, (caddr_t) &dq)) != 0) {
      printf("Quotactl returned: %d\n", dd);
      continue;
    }
  }
#endif

#ifdef HAVE_IRIX
  int dd = 0;
  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    dq.dqb_curblocks  = btodb(q.at(i)->fcur*1024);
    dq.dqb_bsoftlimit = btodb(q.at(i)->fsoft*1024);
    dq.dqb_bhardlimit = btodb(q.at(i)->fhard*1024);
    dq.dqb_curfiles  = q.at(i)->icur;
    dq.dqb_fsoftlimit = q.at(i)->isoft;
    dq.dqb_fhardlimit = q.at(i)->ihard;
    dq.dqb_btimelimit = q.at(i)->ftime;
    dq.dqb_ftimelimit = q.at(i)->itime;

    if ((dd =quotactl(Q_SETQUOTA, (const char *)mounts->getMount(i)->fsname, uid, (caddr_t) &dq)) != 0) {
      printf("Quotactl returned: %d\n", dd);
      continue;
    }
  }
#endif

#ifdef __FreeBSD__
  int dd = 0;
  int qcmd = QCMD(Q_SETQUOTA,USRQUOTA);

  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    dq.dqb_curblocks  = btodb(q->at(i)->fcur*1024);
    dq.dqb_bsoftlimit = btodb(q->at(i)->fsoft*1024);
    dq.dqb_bhardlimit = btodb(q->at(i)->fhard*1024);
    dq.dqb_curinodes  = q->at(i)->icur;
    dq.dqb_isoftlimit = q->at(i)->isoft;
    dq.dqb_ihardlimit = q->at(i)->ihard;
    dq.dqb_btime = q.at(i)->ftime;
    dq.dqb_itime = q.at(i)->itime;

    if ((dd =quotactl((const char *)mounts->getMount(i)->fsname, qcmd, uid, (caddr_t) &dq)) != 0) {
      printf("Quotactl returned: %d\n", dd);
      continue;
    }
#endif

#ifdef _KU_HPUX_QUOTA
  int dd = 0;
  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    dq.dqb_curblocks  = btodb(q.at(i)->fcur*1024);
    dq.dqb_bsoftlimit = btodb(q.at(i)->fsoft*1024);
    dq.dqb_bhardlimit = btodb(q.at(i)->fhard*1024);
    dq.dqb_curfiles  = q.at(i)->icur;
    dq.dqb_fsoftlimit = q.at(i)->isoft;
    dq.dqb_fhardlimit = q.at(i)->ihard;
    dq.dqb_btimelimit = q.at(i)->ftime;
    dq.dqb_ftimelimit = q.at(i)->itime;

    if ((dd =quotactl(Q_SETQUOTA, (const char *)mounts->getMount(i)->fsname, uid, &dq)) != 0) {
      printf("Quotactl returned: %d\n", dd);
      continue;
    }
  }
#endif
}

unsigned int Quota::getUid() {
  return (uid);
}

Quotas::Quotas() {
  q.setAutoDelete(TRUE);
}

Quotas::~Quotas() {
  q.clear();
}

Quota *Quotas::getQuota(unsigned int uid) {
  return (q[uid]);
}

void Quotas::addQuota(unsigned int uid) {
  Quota *tmpQ = NULL;

  if (!q[uid]) {
    tmpQ = new Quota(uid);
    q.insert(uid, tmpQ);
  }
}

void Quotas::addQuota(Quota *aq) {
  unsigned int uid;

  uid = aq->getUid();

  if (!q[uid])
    q.insert(uid, aq);
}

void Quotas::delQuota(unsigned int uid) {
  q.remove(uid);
}

void Quotas::save() {
  QIntDictIterator<Quota> qi(q);

  while (qi.current()) {
    qi.current()->save();
    ++qi;
  }

}

#endif // _KU_QUOTA
