#ifdef _KU_QUOTA

#include "globals.h"

#ifdef HAVE_LINUX_QUOTA_H
#  ifndef QUOTACTL_IN_LIBC
#    include <syscall.h>

int quotactl(int cmd, const char * special, int id, caddr_t addr) {
  return syscall(SYS_quotactl, cmd, special, id, addr);
}

#  endif
#endif

#include <qfile.h>

#include "misc.h"
#include "kglobal_.h"
#include "mnt.h"
#include "quota.h"
#include "kuqconf.h"

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

QuotaMnt::QuotaMnt(const QuotaMnt *q) {
  fcur = q->fcur; fsoft = q->fsoft; fhard = q->fhard; ftime = q->ftime;
  icur = q->icur; isoft = q->isoft; ihard = q->ihard; itime = q->itime;
}

QuotaMnt::~QuotaMnt() {
}

long QuotaMnt::getfcur() {
  return fcur;
}

long QuotaMnt::getfsoft() {
  return fsoft;
}

long QuotaMnt::getfhard() {
  return fhard;
}

long QuotaMnt::geticur() {
  return icur;
}

long QuotaMnt::getisoft() {
  return isoft;
}

long QuotaMnt::getihard() {
  return ihard;
}

long QuotaMnt::getftime() {
  return ftime;
}

long QuotaMnt::getitime() {
  return itime;
}

void QuotaMnt::setfcur(long data) {
  fcur = data;
}

void QuotaMnt::setfsoft(long data) {
  fsoft = data;
}

void QuotaMnt::setfhard(long data) {
  fhard = data;
}

void QuotaMnt::seticur(long data) {
  icur = data;
}

void QuotaMnt::setisoft(long data) {
  isoft = data;
}

void QuotaMnt::setihard(long data) {
  ihard = data;
}

void QuotaMnt::setftime(long data) {
  ftime = data;
}

void QuotaMnt::setitime(long data) {
  itime = data;
}

#ifdef _KU_UFS_QUOTA
#define _KU_TIMELIMITDIV (3600*1600)
#else
#define _KU_TIMELIMITDIV 3600
#endif

#if defined(_KU_UFS_QUOTA) || defined(_KU_HPUX_QUOTA)
#define _KU_CURINODES  dq.dqb_curfiles
#define _KU_ISOFTLIMIT dq.dqb_fsoftlimit
#define _KU_IHARDLIMIT dq.dqb_fhardlimit
#define _KU_BTIMELIMIT dq.dqb_btimelimit
#define _KU_ITIMELIMIT dq.dqb_ftimelimit
#else
#define _KU_CURINODES  dq.dqb_curinodes
#define _KU_ISOFTLIMIT dq.dqb_isoftlimit
#define _KU_IHARDLIMIT dq.dqb_ihardlimit
#define _KU_BTIMELIMIT dq.dqb_btime
#define _KU_ITIMELIMIT dq.dqb_itime
#endif

#if defined(_KU_UFS_QUOTA) || defined(HAVE_IRIX) || defined(_KU_HPUX_QUOTA)
#define _KU_GETQUOTA Q_GETQUOTA
#define _KU_SETQUOTA Q_SETQUOTA
#endif

#if defined(_KU_EXT2_QUOTA) || defined(BSD)
#define _KU_GETQUOTA QCMD(Q_GETQUOTA, USRQUOTA)
#define _KU_SETQUOTA QCMD(Q_SETQUOTA, USRQUOTA)
#endif

static int doQuotaCtl(int ACmd, uint AUID, const MntEnt *m, struct dqblk *dq) {
#ifdef _KU_UFS_QUOTA
  int fd;
  struct quotctl qctl;

  qctl.op = ACmd;
  qctl.uid = AUID;
  qctl.addr = (caddr_t)dq;
    
  fd = open(QFile::encodeName(m->getquotafilename()), (ACmd == _KU_GETQUOTA) ? O_RDONLY : O_WRONLY);
  int res = ioctl(fd, Q_QUOTACTL, &qctl);
  close(fd);
  return res;
#else
#  if defined(_KU_EXT2_QUOTA) || defined(HAVE_IRIX)
         return quotactl(ACmd, QFile::encodeName(m->getfsname()), AUID, (caddr_t) dq);
#  else
#    ifdef __osf__
         return quotactl((char*)QFile::encodeName(m->getdir()).data(), ACmd, AUID, (caddr_t) dq);
#    else
#        ifdef BSD
             return quotactl(QFile::encodeName(m->getdir()), ACmd, AUID, (caddr_t) dq);
#        endif
#    endif
#    ifdef _KU_HPUX_QUOTA
         return quotactl(ACmd, QFile::encodeName(m->getquotafilename()), AUID, dq);
#    endif
#  endif
#endif
}

static QuotaMnt *getQuotaMnt(uint AUID, const MntEnt *m) {
  struct dqblk dq;

  int res = doQuotaCtl(_KU_GETQUOTA, AUID, m, &dq);

  if (res == 0)
    return new QuotaMnt(
      dbtob(dq.dqb_curblocks)/1024,
      dbtob(dq.dqb_bsoftlimit)/1024,
      dbtob(dq.dqb_bhardlimit)/1024,
      _KU_CURINODES,
      _KU_ISOFTLIMIT,
      _KU_IHARDLIMIT,
      _KU_BTIMELIMIT/_KU_TIMELIMITDIV,
      _KU_ITIMELIMIT/_KU_TIMELIMITDIV
    );
  if (errno == ESRCH)
    return new QuotaMnt;
  is_quota = 0;
  return NULL;
}

static void setQuotaMnt(uint AUID, const MntEnt *m, QuotaMnt *qm) {
  struct dqblk dq;

  dq.dqb_curblocks  = btodb(qm->getfcur() * 1024);
  dq.dqb_bsoftlimit = btodb(qm->getfsoft() * 1024);
  dq.dqb_bhardlimit = btodb(qm->getfhard() * 1024);
  _KU_CURINODES     = qm->geticur();
  _KU_ISOFTLIMIT    = qm->getisoft();
  _KU_IHARDLIMIT    = qm->getihard();
  _KU_BTIMELIMIT    = qm->getftime() * _KU_TIMELIMITDIV;
  _KU_ITIMELIMIT    = qm->getitime() * _KU_TIMELIMITDIV;

  int res = doQuotaCtl(_KU_SETQUOTA, AUID, m, &dq);

  if (res != 0)
    printf("Quotactl returned: %d\n", res);
}

Quota::Quota(uint auid, bool doget) {
  uid = auid;

  q.setAutoDelete(TRUE);
  if (is_quota == 0)
    return;

  if (!doget) {
    for (uint i=0; i<kug->getMounts().getMountsNumber(); i++)
      q.append(new QuotaMnt);
    return;
  }

  for (uint i=0; i<kug->getMounts().getMountsNumber(); i++)
    q.append(getQuotaMnt(uid, kug->getMounts()[i]));
}

Quota::~Quota() {
  q.clear();
}

QuotaMnt *Quota::operator[](uint mntnum) {
  return q.at(mntnum);
}

uint Quota::getMountsNumber() {
  return q.count();
}

bool Quota::save() {
  if (is_quota == 0)
    return TRUE;

  for (uint i=0; i<kug->getMounts().getMountsNumber(); i++)
    setQuotaMnt(uid, kug->getMounts()[i], q.at(i));

  return TRUE;
}

unsigned int Quota::getUid() {
  return uid;
}

Quotas::Quotas() {
  q.setAutoDelete(TRUE);
}

Quotas::~Quotas() {
  q.clear();
}

Quota *Quotas::operator[](unsigned int uid) {
  return q[uid];
}

void Quotas::addQuota(unsigned int uid) {
  if (!q[uid])
    q.insert(uid, new Quota(uid));
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

bool Quotas::save() {
  QIntDictIterator<Quota> qi(q);

  while (qi.current()) {
    if (!qi.current()->save())
      return FALSE;
    ++qi;
  }
  return TRUE;
}

#endif // _KU_QUOTA
