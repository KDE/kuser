#define _KU_MAIN
#include "maindlg.h"
#include "maindlg.moc"

#include "globals.h"

#include <qtooltip.h>

#include <kmsgbox.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <knewpanner.h>
#include <kstring.h>

#include <unistd.h>
#include <signal.h>

#ifdef _KU_QUOTA
#include "quota.h"
#include "mnt.h"
#endif

#include "kuser.h"
#include "misc.h"
#include "usernamedlg.h"
#include "grpnamedlg.h"
#include "propdlg.h"
#include "addUser.h"
#include "pwddlg.h"
#include "editGroup.h"

#include "editDefaults.h"

mainDlg::mainDlg(QWidget *parent) :
QWidget(parent)
{
  changed = FALSE;
  prev = 0;

  usort = -1;
  gsort = -1;
}

void mainDlg::init() {
#ifdef _KU_QUOTA
  m = new Mounts();
  q = new Quotas();
#endif

  u = new KUsers();
  g = new KGroups();

  kp = new KNewPanner(this, "panner", KNewPanner::Horizontal);
  kp->setGeometry(10, 80, 380, 416);
  
  lbusers = new KUserView(kp, "lbusers");
  //  lbusers->setGeometry(10,80,380,208);

  lbgroups = new KGroupView(kp, "lbgroups");
  //  lbgroups->setGeometry(10,400,380,208);

  kp->activate(lbusers, lbgroups);
  
  QObject::connect(lbusers, SIGNAL(headerClicked(int)), this, SLOT(setUsersSort(int)));
  QObject::connect(lbusers, SIGNAL(selected(int)), this, SLOT(userSelected(int)));

  QObject::connect(lbgroups, SIGNAL(headerClicked(int)), this, SLOT(setGroupsSort(int)));
  QObject::connect(lbgroups, SIGNAL(selected(int)), this, SLOT(groupSelected(int)));

  reloadUsers(0);
  reloadGroups(0);
}

mainDlg::~mainDlg() {
  lbusers->setAutoUpdate(FALSE);
  lbgroups->setAutoUpdate(FALSE);

  delete lbusers;
  delete lbgroups;

  delete u;
  delete g;

#ifdef _KU_QUOTA
  delete q;
  delete m;
#endif

  delete kp;
}

void mainDlg::setUsersSort(int col) {
  if (usort == col)
    usort = -1;
  else
    usort = col;

  lbusers->sortBy(usort);

  reloadUsers(lbusers->currentItem());
}

void mainDlg::setGroupsSort(int col) {
  if (gsort == col)
    gsort = -1;
  else
    gsort = col;

  lbgroups->sortBy(gsort);

  reloadGroups(lbgroups->currentItem());
}

void mainDlg::reloadUsers(int id) {
  KUser *ku;

  lbusers->setAutoUpdate(FALSE);
  lbusers->clear();

  for (uint i = 0; i<u->getNumber(); i++) {
    ku = u->get(i);
    lbusers->insertItem(ku);
  }

  lbusers->setAutoUpdate(TRUE);
  lbusers->setCurrentItem(id);
  lbusers->repaint();
}

void mainDlg::reloadGroups(int gid) {
  KGroup *kg;

  lbgroups->setAutoUpdate(FALSE);
  lbgroups->clear();

  for (uint i = 0; i<g->getNumber(); i++) {
    kg = g->get(i);
    lbgroups->insertItem(kg);
  }

  lbgroups->setAutoUpdate(TRUE);
  lbgroups->setCurrentItem(gid);
  lbgroups->repaint();
}

void mainDlg::useredit() {
  userSelected(lbusers->currentItem());
}

void mainDlg::userdel() {
  uint i = 0;
  bool islast = FALSE;

  if (KMsgBox::yesNo(0, i18n("WARNING"),
                     i18n("Do you really want to delete user ?"),
                     KMsgBox::STOP,
                     i18n("Cancel"), i18n("Delete")) == 2) {

    i = lbusers->currentItem();
    if (i == u->getNumber()-1)
      islast = TRUE;

    uint uid = lbusers->getCurrentUser()->getp_uid();
    uint gid = lbusers->getCurrentUser()->getp_gid();

#ifdef _KU_QUOTA
    if (u->lookup(uid) == NULL)
      q->delQuota(uid);
#endif

    u->del(lbusers->getCurrentUser());

    prev = -1;

    if (!islast)
      reloadUsers(i);
    else
      reloadUsers(i-1);
    changed = TRUE;
    if (config->readBoolEntry("userPrivateGroup", KU_USERPRIVATEGROUP)) {
      bool found = false;

      for (uint i=0; i<u->getNumber(); i++)
        if (u->get(i)->getp_gid() == gid) {
          found = true;
          break;
        }

      if (!found)
        if (KMsgBox::yesNo(0, i18n("WARNING"),
                           i18n("You are using private groups.\nDo you want delete user's private group ?"),
                           KMsgBox::STOP,
                           i18n("Cancel"), i18n("Delete")) == 2) {
          uint oldc = lbgroups->currentItem();
          g->del(g->lookup(gid));
          if (oldc == g->count())
            reloadGroups(oldc-1);
          else
            reloadGroups(oldc);
        }
    }
  }
}

void mainDlg::useradd() {
  usernamedlg *ud;
  addUser *au;

  KUser *tk;
#ifdef _KU_QUOTA
  Quota *tq;
#endif // _KU_QUOTA

  int uid;

  if ((uid = u->first_free()) == -1) {
    err->display();
    return;
  }

  tk = new KUser();
  tk->setp_uid(uid);

#ifdef _KU_QUOTA
  tq = new Quota(tk->getp_uid(), FALSE);
#endif // _KU_QUOTA

  ud = new usernamedlg(tk, this);
  if (ud->exec() == 0)
    return;

  delete ud;

  config->setGroup("template");
  tk->setp_shell(readentry("shell"));
  tk->setp_dir(readentry("homeprefix", KU_HOMEPREFIX)+"/"+tk->getp_name());
  tk->setp_gid(readnumentry("gid"));
  tk->setp_fname(readentry("p_fname"));
#ifdef __FreeBSD__
  tk->setp_office(readentry("p_office"));
  tk->setp_ophone(readentry("p_ophone"));
  tk->setp_hphone(readentry("p_hphone"));
  tk->setp_class(readentry("p_class"));
  tk->setp_change(readnumentry("p_change"));
  tk->setp_expire(readnumentry("p_expire"));
#else
  tk->setp_office1(readentry("p_office1"));
  tk->setp_office2(readentry("p_office2"));
  tk->setp_address(readentry("p_address"));
#endif

#ifdef _KU_SHADOW
  tk->sets_lstchg(today());
  if (config->hasKey("s_min"))
    tk->sets_min(readnumentry("s_min"));

  if (config->hasKey("s_max"))
    tk->sets_max(readnumentry("s_max"));

  if (config->hasKey("s_warn"))
    tk->sets_warn(readnumentry("s_warn"));

  if (config->hasKey("s_inact"))
    tk->sets_inact(readnumentry("s_inact"));

  if (config->hasKey("s_expire"))
    tk->sets_expire(readnumentry("s_expire"));

  if (config->hasKey("s_flag"))
    tk->sets_flag(readnumentry("s_flag"));
#endif

#ifdef _KU_QUOTA
  if (is_quota != 0)
    au = new addUser(tk, tq, this, "userin");
  else
    au = new addUser(tk, NULL, this, "userin");
#else
  au = new addUser(tk, this, "userin");
#endif

  au->setCreateHomeDir(config->readBoolEntry("createHomeDir", true));
  au->setCopySkel(config->readBoolEntry("copySkel", true));
  au->setUserPrivateGroup(config->readBoolEntry("userPrivateGroup", KU_USERPRIVATEGROUP));

  if (au->exec() != 0) {
    if (au->getUserPrivateGroup()) {
      KGroup *tg;

      if ((tg = g->lookup(tk->getp_name())) == NULL) {
        tg = new KGroup();
        tg->setgid(g->first_free());
        tg->setname(tk->getp_name());
        g->add(tg);
        reloadGroups(lbgroups->currentItem());
      }

      tk->setp_gid(tg->getgid());
    }
    u->add(tk);
#ifdef _KU_QUOTA
    q->addQuota(tq);
#endif
    changed = TRUE;
  }
  else {
    delete tk;
#ifdef _KU_QUOTA
    delete tq;
#endif
  }

  reloadUsers(u->getNumber()-1);

  delete au;
}


void mainDlg::save() {
  if (changed == TRUE)
    if (KMsgBox::yesNo(0, i18n("Data was modified"),
                       i18n("Would you like to save changes ?"),
    		       KMsgBox::INFORMATION,
		       i18n("Save"), i18n("Discard changes")) == 1) {
      if (!u->save())
        err->display();

      if (!g->save())
        err->display();
#ifdef _KU_QUOTA
      if (is_quota != 0)
        if (!q->save())
          err->display();
#endif
    }
}

void mainDlg::quit() {
  save();

  qApp->quit();
}

void mainDlg::setpwd() {
  pwddlg *d;
  
  d = new pwddlg(lbusers->getCurrentUser(), this, "pwddlg");
  d->exec();
  changed = TRUE;
  delete d;
}

void mainDlg::help() {
  kapp->invokeHTMLHelp(0,0);
}

void mainDlg::properties() {
  editDefaults *eddlg;

  eddlg = new editDefaults();

  eddlg->setCaption(i18n("Edit defaults"));

  config->setGroup("template");
  eddlg->setShell(config->readEntry("shell", ""));
  eddlg->setHomeBase(config->readEntry("homeBase", "/home"));
  eddlg->setCreateHomeDir(config->readBoolEntry("createHomeDir", true));
  eddlg->setCopySkel(config->readBoolEntry("copySkel", true));
  eddlg->setUserPrivateGroup(config->readBoolEntry("userPrivateGroup", KU_USERPRIVATEGROUP));

  if (eddlg->exec() != 0) {
    config->setGroup("template");
    config->writeEntry("shell", eddlg->getShell());
    config->writeEntry("homeBase", eddlg->getHomeBase());
    config->writeEntry("createHomeDir", eddlg->getCreateHomeDir());
    config->writeEntry("copySkel", eddlg->getCopySkel());
    config->writeEntry("userPrivateGroup", eddlg->getUserPrivateGroup());
  }

  delete eddlg;
}

void mainDlg::groupSelected(int i) {
  editGroup *egdlg;
  KGroup *tmpKG;

  tmpKG = g->get(i);

  if (tmpKG == NULL) {
    printf(i18n("Null pointer tmpKG in mainDlg::groupSelected(%d)\n"), i);
    return;
  }

  egdlg = new editGroup(tmpKG);

  egdlg->setCaption(i18n("Group properties"));

  if (egdlg->exec() != 0)
    changed = TRUE;

  delete egdlg;
}

void mainDlg::userSelected(int i) {
  propdlg *editUser;
  KUser *tmpKU;

  tmpKU =  lbusers->getCurrentUser();
  if (tmpKU == NULL) {
    printf(i18n("Null pointer tmpKU in mainDlg::userSelected(%d)\n"), i);
    return;
  }

#ifdef _KU_QUOTA
  Quota *tmpQ = 0;

  if (is_quota != 0) {
    tmpQ = q->getQuota(tmpKU->getp_uid());
    if (tmpQ == NULL) {
      printf(i18n("Null pointer tmpQ in mainDlg::selected(%d)\n"), i);
      return;
    }
  }
  
  editUser = new propdlg(tmpKU, tmpQ, this, "userin");
#else
  editUser = new propdlg(tmpKU, this, "userin");
#endif

  if (editUser->exec() != 0) {
    reloadUsers(lbusers->currentItem());
    changed = TRUE;
  }

  delete editUser;
}

KUsers *mainDlg::getUsers() {
  return (u);
}

KGroups *mainDlg::getGroups() {
  return (g);
}

#ifdef _KU_QUOTA
Mounts *mainDlg::getMounts() {
  return (m);
}

Quotas *mainDlg::getQuotas() {
  return (q);
}
#endif

void mainDlg::resizeEvent (QResizeEvent *rse) {
  QSize sz;

  sz = rse->size();

  kp->setGeometry(10, 10, sz.width()-20, sz.height()-20);
}

void mainDlg::grpadd() {
  grpnamedlg *gd;
  int gid;
  KGroup *tk;

  if ((gid = g->first_free()) == -1) {
    err->display();
    return;
  }

  tk = new KGroup();
  tk->setgid(gid);
  
  gd = new grpnamedlg(tk, this);
  if (gd->exec() == 0) {
    delete tk;
    return;
  }

  delete gd;
  g->add(tk);
  reloadGroups(lbgroups->currentItem());
  changed = TRUE;
}

void mainDlg::grpedit() {
  groupSelected(lbgroups->currentItem());
}

void mainDlg::grpdel() {
  uint i = 0;
  bool islast = FALSE;

  if (KMsgBox::yesNo(0, i18n("WARNING"),
                     i18n("Do you really want to delete group ?"),
                     KMsgBox::STOP,
                     i18n("Cancel"), i18n("Delete")) == 2) {

    i = lbgroups->currentItem();
    if (i == g->getNumber()-1)
      islast = TRUE;

    g->del(lbgroups->getCurrentGroup());

    prev = -1;

    if (!islast)
      reloadGroups(i);
    else
      reloadGroups(i-1);
    changed = TRUE;
  }
}

