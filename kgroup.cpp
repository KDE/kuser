#include "globals.h"

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#ifdef HAVE_SHADOW
#include <shadow.h>
#endif

#include <qstring.h>
#include <qdir.h>

#include "kglobal_.h"
#include "kgroup.h"
#include "misc.h"
#include "editDefaults.h"

#ifdef _KU_QUOTA
#include "mnt.h"
#include "quota.h"
#endif

// This is to simplify compilation for Red Hat Linux systems, where
// gid's for regular users' private groups start at 500 <duncan@kde.org>
#ifdef KU_FIRST_USER
#define _KU_FIRST_GID KU_FIRST_USER
#else 
#define _KU_FIRST_GID 1001 
#endif

KGroup::KGroup() : pwd("*") {
  gid = 0;
}
  
KGroup::KGroup(KGroup *copy) {
  name    = copy->name;
  pwd     = copy->pwd;
  gid     = copy->gid;
}

KGroup::~KGroup() {
}

const QString &KGroup::getName() const {
  return name;
}

const QString &KGroup::getPwd() const {
  return pwd;
}

gid_t KGroup::getGID() const {
  return gid;
}

void KGroup::setName(const QString &data) {
  name = data;
}

void KGroup::setPwd(const QString &data) {
  pwd = data;
}

void KGroup::setGID(gid_t data) {
  gid = data;
}

bool KGroup::lookup_user(const QString &name) {
  return (u.find(name) != u.end());
}

void KGroup::addUser(const QString &name) {
  if (!lookup_user(name))
     u.append(name);
}

void KGroup::removeUser(const QString &name) {
  u.remove(name);
}

uint KGroup::count() const {
  return u.count();
}

QString KGroup::user(uint i) {
  return u[i];
}

void KGroup::clear() {
  u.clear();
}

KGroups::KGroups() {
  gr_backuped = FALSE;
  gn_backuped = FALSE;

  mode = 0644;
  uid = 0;
  gid = 0;

  g.setAutoDelete(TRUE);

  if (!load())
    err->display();
}

bool KGroups::load() {
  struct group *p;
  KGroup *tmpKG = 0;
  struct stat st;
  QString filename;
  QString group_filename;
  QString nisgroup_filename;
  int rc = 0;
  int group_errno = 0;
  int nisgroup_errno = 0;
  char processing_file = '\0';
  #define GROUP    0x01
  #define NISGROUP 0x02
  #define MAXFILES 2

  // Prepare to read KUser configuration

  config->setGroup("sources");
  group_filename = config->readEntry("groupsrc");
  nisgroup_filename = config->readEntry("nisgroupsrc");
  if(!group_filename.isEmpty()) {
    processing_file = processing_file | GROUP;
    filename.append(group_filename);
  }

  // Start reading group file(s)

  for(int k = 0; k < MAXFILES; k++) {
    rc = stat(QFile::encodeName(filename), &st);
    if(rc != 0) {
      err->addMsg(i18n("stat call on file %1 failed: %2\nCheck KUser Settings (Sources)\n").arg(filename).arg(strerror(errno)));
      err->display();
      if( (processing_file & GROUP) != 0 ) {
        group_errno = errno;
        if(!nisgroup_filename.isEmpty()) {
          processing_file = processing_file & ~GROUP;
          processing_file = processing_file | NISGROUP;
          filename.truncate(0);
          filename.append(nisgroup_filename);
        }
        continue;
      }
      else{
        nisgroup_errno = errno;
        break;
      }
    }

    mode = st.st_mode;
    uid = st.st_uid;
    gid = st.st_gid;

  // We are reading our configuration specified group file
#ifdef HAVE_FGETGRENT
    FILE *fgrp = fopen(QFile::encodeName(filename), "r");
    QString tmp;
    if (fgrp == NULL) {
      err->addMsg(i18n("Error opening %1 for reading").arg(filename));
      return FALSE;
    }

    while ((p = fgetgrent(fgrp)) != NULL) {
#else
    while ((p = getgrent()) != NULL) {
#endif
      tmpKG = new KGroup();
      tmpKG->setGID(p->gr_gid);
      tmpKG->setName(p->gr_name);
      tmpKG->setPwd(p->gr_passwd);

      char *u_name;
      int i = 0;
      while ((u_name = p->gr_mem[i])!=0) {
        tmpKG->addUser(u_name);
        i++;
      }

      g.append(tmpKG);
    }

    // End reading filename

#ifdef HAVE_FGETGRENT
    fclose(fgrp);
#endif
    if(!nisgroup_filename.isEmpty()) {
      if(nisgroup_filename == group_filename)
        break;
      processing_file = processing_file & ~GROUP;
      processing_file = processing_file | NISGROUP;
      filename.truncate(0);
      filename.append(nisgroup_filename);
    }
    else
      break;

  }	// end of processing files, for loop

  if( (group_errno == 0) && (nisgroup_errno == 0) )
    return(TRUE);
  if( (group_errno != 0) && (nisgroup_errno != 0) )
    return(FALSE);
  else
    return(TRUE);
}

bool KGroups::save() {
  FILE *group_fd = NULL;
  FILE *nisgroup_fd = NULL;
  int mingid = 0;
  int nis_groups_written = 0;
  gid_t tmp_gid = 0;
  QString tmpS;
  QString tmp;
  QString group_filename;
  QString nisgroup_filename;
  QString qs_mingid;

  const char *c_mingid;
  const char *gr_filename;
  const char *gn_filename;
  char errors_found = '\0';
    #define NOMINGID    0x01
    #define NONISGROUP  0x02

  // read KUser configuration info

  config->setGroup("sources");
  group_filename = config->readEntry("groupsrc");
  nisgroup_filename = config->readEntry("nisgroupsrc");
  qs_mingid = config->readEntry("nismingid");
  if( (!qs_mingid.isEmpty()) && (nisgroup_filename != group_filename) ) {
    c_mingid = qs_mingid.latin1();
    mingid = atoi(c_mingid);
  }

  // Backup file(s)


  if(!group_filename.isEmpty()) {
    if (!gr_backuped) {
      gr_filename = group_filename.latin1();
      backup(gr_filename);
      gr_backuped = TRUE;
    }
  }
  if(!nisgroup_filename.isEmpty() && (nisgroup_filename != group_filename)) {
    if (!gn_backuped) {
      gn_filename = nisgroup_filename.latin1();
      backup(gn_filename);
      gn_backuped = TRUE;
    }
  }

  // Open file(s)

  if(!group_filename.isEmpty()) {
    if((group_fd = fopen(QFile::encodeName(group_filename), "w")) == NULL)
      err->addMsg(i18n("Error opening %1 for writing").arg(group_filename));
  }  

  if(!nisgroup_filename.isEmpty() && (nisgroup_filename != group_filename)) {
    if((nisgroup_fd = fopen(QFile::encodeName(nisgroup_filename), "w")) == NULL)
      err->addMsg(i18n("Error opening %1 for writing").arg(nisgroup_filename));
  }  

  umask(0077);


  for (unsigned int i=0; i<g.count(); i++) {
    KGroup *gr = g.at(i);
    tmp_gid = gr->getGID();
    tmpS = QString::fromLatin1("%1:%2:%3:")
      .arg(gr->getName())
      .arg(gr->getPwd())
      .arg(gr->getGID());
    for (uint j=0; j<gr->count(); j++) {
       if (j != 0)
	 tmpS += ',';
       tmpS += gr->user(j);
    }
    tmpS += '\n';

    if( (nisgroup_fd != 0) && (mingid != 0) ) {
      if(mingid <= tmp_gid) {
        fputs(tmpS.local8Bit(), nisgroup_fd);
        nis_groups_written++;
        continue;
      }
      else{
        fputs(tmpS.local8Bit(), group_fd);
        continue;
      }
    }

    if( (nisgroup_fd != 0) && (mingid == 0) ) {
      errors_found = errors_found | NOMINGID;
      fputs(tmpS.local8Bit(), group_fd);
      continue;
    }

    if( (nisgroup_fd == 0) && (mingid != 0) ) {
      errors_found = errors_found | NONISGROUP;
      fputs(tmpS.local8Bit(), group_fd);
      continue;
    }

    fputs(tmpS.local8Bit(), group_fd);

  }		/* end for i loop */

  if(group_fd) {
    fclose(group_fd);
    chmod(QFile::encodeName(group_filename), mode);
    chown(QFile::encodeName(group_filename), uid, gid);
  }

  if(nisgroup_fd) {
    fclose(nisgroup_fd);
    chmod(QFile::encodeName(nisgroup_filename), mode);
    chown(QFile::encodeName(nisgroup_filename), uid, gid);
  }

  if( (errors_found & NOMINGID) != 0 ) {
    err->addMsg(i18n("Unable to process NIS group file without a minimum GID specified.\nPlease update KUser Settings (Sources)"));
    err->display();
  }

  if( (errors_found & NONISGROUP) != 0 ) {
    err->addMsg(i18n("Specifying NIS minimum GID requires NIS file(s).\nPlease update KUser Settings (Sources)"));
    err->display();
  }

#ifdef GRMKDB
  if( (nis_groups_written > 0) || (nisgroup_filename == group_filename) ) {
    if (system(GRMKDB) != 0) {
      err->addMsg(i18n("Unable to build NIS group databases"));
      return FALSE;
    }
  }
#endif

  return TRUE;
}

KGroup *KGroups::lookup(const QString &name) {
  for (uint i = 0; i<g.count(); i++)
    if (g.at(i)->getName() == name)
      return g.at(i);
  return NULL;
}

KGroup *KGroups::lookup(gid_t gid) {
  for (uint i = 0; i<g.count(); i++)
    if (g.at(i)->getGID() == gid)
      return g.at(i);
  return NULL;
}

gid_t KGroups::first_free() {
  gid_t t = _KU_FIRST_GID ;

  for (t = _KU_FIRST_GID ; t<65534; t++)
    if (lookup(t) == NULL)
      return t;

  err->addMsg(i18n("You have more than 65534 groups!?!? You have run out of gid space!"));
  return (-1);
}

KGroups::~KGroups() {
  g.clear();
}

KGroup *KGroups::operator[](uint num) {
  return g.at(num);
}

KGroup *KGroups::first() {
  return g.first();
}

KGroup *KGroups::next() {
  return g.next();
}

void KGroups::add(KGroup *ku) {
  g.append(ku);
}

void KGroups::del(KGroup *au) {
  g.remove(au);
}

uint KGroups::count() const {
  return g.count();
}

