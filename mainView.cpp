#define _KU_MAIN
#include "mainView.h"

#include "globals.h"

#include <stdio.h>

#include <unistd.h>
#include <signal.h>

#include <qtooltip.h>
#include <qmessagebox.h>
#include <qsplitter.h>

#include <ktoolbar.h>
#include <kiconloader.h>

#include "misc.h"
#include "kglobal.h"
#include "usernamedlg.h"
#include "grpnamedlg.h"
#include "propdlg.h"
#include "addUser.h"
#include "pwddlg.h"
#include "editGroup.h"
#include "editDefaults.h"

mainView::mainView(QWidget *parent) : QWidget(parent) {
  changed = FALSE;
  prev = 0;

  usort = -1;
  gsort = -1;
  
  init();
}

void mainView::init() {
  kp = new QSplitter(QSplitter::Horizontal, this, "splitter");
  kp->setGeometry(10, 80, 380, 416);
  
  lbusers = new KUserView(kp, "lbusers");
  //  lbusers->setGeometry(10,80,380,208);

  lbgroups = new KGroupView(kp, "lbgroups");
  //  lbgroups->setGeometry(10,400,380,208);

  QObject::connect(lbusers, SIGNAL(headerClicked(int)), this, SLOT(setUsersSort(int)));
  QObject::connect(lbusers, SIGNAL(selected(int)), this, SLOT(userSelected(int)));

  QObject::connect(lbgroups, SIGNAL(headerClicked(int)), this, SLOT(setGroupsSort(int)));
  QObject::connect(lbgroups, SIGNAL(selected(int)), this, SLOT(groupSelected(int)));

  reloadUsers(0);
  reloadGroups(0);
}

mainView::~mainView() {
  lbusers->setAutoUpdate(FALSE);
  lbgroups->setAutoUpdate(FALSE);

  delete lbusers;
  delete lbgroups;

  delete kp;
}

void mainView::setUsersSort(int col) {
  if (usort == col)
    usort = -1;
  else
    usort = col;

  lbusers->sortBy(usort);

  reloadUsers(lbusers->currentItem());
}

void mainView::setGroupsSort(int col) {
  if (gsort == col)
    gsort = -1;
  else
    gsort = col;

  lbgroups->sortBy(gsort);

  reloadGroups(lbgroups->currentItem());
}

void mainView::reloadUsers(int id) {
  KUser *ku;

  lbusers->setAutoUpdate(FALSE);
  lbusers->clear();

  for (uint i = 0; i<kug->getUsers().count(); i++) {
    ku = kug->getUsers()[i];
    lbusers->insertItem(ku);
  }

  lbusers->setAutoUpdate(TRUE);
  lbusers->setCurrentItem(id);
  lbusers->repaint();
}

void mainView::reloadGroups(int gid) {
  KGroup *kg;

  lbgroups->setAutoUpdate(FALSE);
  lbgroups->clear();

  for (uint i = 0; i<kug->getGroups().count(); i++) {
    kg = kug->getGroups()[i];
    lbgroups->insertItem(kg);
  }

  lbgroups->setAutoUpdate(TRUE);
  lbgroups->setCurrentItem(gid);
  lbgroups->repaint();
}

void mainView::useredit() {
  userSelected(lbusers->currentItem());
}

void mainView::userdel() {
  uint i = 0;
  bool islast = FALSE;
  KUser *user = lbusers->getCurrentUser();

  if (QMessageBox::warning(0, i18n("WARNING"),
                     i18n("Do you really want to delete user '%1'?")
		       .arg(user->getName()),
                     i18n("Cancel"), i18n("Delete")) != 1)
    return;

  i = lbusers->currentItem();
  if (i == kug->getUsers().count()-1)
    islast = TRUE;

  uint uid = user->getUID();

#ifdef _KU_QUOTA
  if (kug->getUsers().lookup(uid) == NULL)
    kug->getQuotas().delQuota(uid);
#endif

  kug->getUsers().del(user);

  prev = -1;

  if (!islast)
    reloadUsers(i);
  else
    reloadUsers(i-1);
  changed = TRUE;

  config->setGroup("template"); 
  if (!config->readBoolEntry("userPrivateGroup", KU_USERPRIVATEGROUP))
    return;
    
  const QString &userName = user->getName();
  KGroup *group = NULL;

  for (uint i=0; i<kug->getGroups().count(); i++)
    if (kug->getGroups()[i]->getName() == userName) {
      group = kug->getGroups()[i];
      break;
    }

  if (group == NULL)
    return;
    
  if (QMessageBox::information(0, i18n("WARNING"),
        i18n("You are using private groups.\n"
             "Do you want delete user's private group '%1'?")
	       .arg(group->getName()),
	     i18n("Cancel"), i18n("Delete")) != 1)
    return;
    
  uint oldc = lbgroups->currentItem();
  kug->getGroups().del(group);
  if (oldc == kug->getGroups().count())
    reloadGroups(oldc-1);
  else
    reloadGroups(oldc);
}

void mainView::useradd() {
  KUser *tk;
#ifdef _KU_QUOTA
  Quota *tq;
#endif // _KU_QUOTA

  int uid;

  if ((uid = kug->getUsers().first_free()) == -1) {
    err->display();
    return;
  }

  tk = new KUser();
  tk->setUID(uid);

#ifdef _KU_QUOTA
  tq = new Quota(tk->getUID(), FALSE);
#endif // _KU_QUOTA

  usernamedlg ud(tk, this);
  if (ud.exec() == 0)
    return;

  config->setGroup("template");
  tk->setShell(readentry("shell"));
  tk->setHomeDir(readentry("homeprefix", KU_HOMEPREFIX)+"/"+tk->getName());
  tk->setGID(readnumentry("gid"));
  tk->setFullName(readentry("p_fname"));
#ifdef __FreeBSD__
  tk->setOffice(readentry("p_office"));
  tk->setWorkPhone(readentry("p_ophone"));
  tk->setHomePhone(readentry("p_hphone"));
  tk->setClass(readentry("p_class"));
  tk->setLastChange(readnumentry("p_change"));
  tk->setExpire(readnumentry("p_expire"));
#else
  tk->setOffice1(readentry("p_office1"));
  tk->setOffice2(readentry("p_office2"));
  tk->setAddress(readentry("p_address"));
#endif

#ifdef _KU_SHADOW
  tk->setLastChange(today());
  if (config->hasKey("s_min"))
    tk->setMin(readnumentry("s_min"));

  if (config->hasKey("s_max"))
    tk->setMax(readnumentry("s_max"));

  if (config->hasKey("s_warn"))
    tk->setWarn(readnumentry("s_warn"));

  if (config->hasKey("s_inact"))
    tk->setInactive(readnumentry("s_inact"));

  if (config->hasKey("s_expire"))
    tk->setExpire(readnumentry("s_expire"));

  if (config->hasKey("s_flag"))
    tk->setFlag(readnumentry("s_flag"));
#endif

  if (is_quota == 0)
    tq = NULL;
  
#ifdef _KU_QUOTA
  addUser au(*tk, *tq, this, "userin");
#else
  addUser au(*tk, this, "userin");
#endif

  au.setCreateHomeDir(config->readBoolEntry("createHomeDir", true));
  au.setCopySkel(config->readBoolEntry("copySkel", true));
  au.setUserPrivateGroup(config->readBoolEntry("userPrivateGroup", KU_USERPRIVATEGROUP));

  if (au.exec() != 0) {
    if (au.getUserPrivateGroup()) {
      KGroup *tg;

      if ((tg = kug->getGroups().lookup(tk->getName())) == NULL) {
        tg = new KGroup();
        tg->setGID(kug->getGroups().first_free());
        tg->setName(tk->getName());
        kug->getGroups().add(tg);
        reloadGroups(lbgroups->currentItem());
      }

      tk->setGID(tg->getGID());
    }
    kug->getUsers().add(tk);
#ifdef _KU_QUOTA
    kug->getQuotas().addQuota(tq);
#endif
    changed = TRUE;
  }
  else {
    delete tk;
#ifdef _KU_QUOTA
    delete tq;
#endif
  }

  reloadUsers(kug->getUsers().count()-1);
}


void mainView::save() {
  if (changed != TRUE)
    return;
    
  if (QMessageBox::information(0, i18n("Data was modified"),
                               i18n("Would you like to save changes ?"),
                               i18n("Save"), i18n("Discard changes")) != 0)
    return;
    
  if (!kug->getUsers().save())
    err->display();

  if (!kug->getGroups().save())
    err->display();
#ifdef _KU_QUOTA
  if (is_quota != 0)
    if (!kug->getQuotas().save())
      err->display();
#endif
  changed = FALSE;
}

void mainView::quit() {
  save();

  qApp->quit();
}

void mainView::setpwd() {
  pwddlg d(*(lbusers->getCurrentUser()), this, "pwddlg");
  if (d.exec() != 0)
    changed = TRUE;
}

void mainView::help() {
  kapp->invokeHTMLHelp(0,0);
}

void mainView::properties() {
  editDefaults eddlg;

  eddlg.setCaption(i18n("Edit defaults"));

  config->setGroup("template");
  eddlg.setShell(config->readEntry("shell", ""));
  eddlg.setHomeBase(config->readEntry("homeBase", "/home"));
  eddlg.setCreateHomeDir(config->readBoolEntry("createHomeDir", true));
  eddlg.setCopySkel(config->readBoolEntry("copySkel", true));
  eddlg.setUserPrivateGroup(config->readBoolEntry("userPrivateGroup", KU_USERPRIVATEGROUP));

  if (eddlg.exec() != 0) {
    config->setGroup("template");
    config->writeEntry("shell", eddlg.getShell());
    config->writeEntry("homeBase", eddlg.getHomeBase());
    config->writeEntry("createHomeDir", eddlg.getCreateHomeDir());
    config->writeEntry("copySkel", eddlg.getCopySkel());
    config->writeEntry("userPrivateGroup", eddlg.getUserPrivateGroup());
  }
}

void mainView::groupSelected(int i) {
  KGroup *tmpKG;

  tmpKG = kug->getGroups()[i];

  if (tmpKG == NULL) {
    printf("Null pointer tmpKG in mainView::groupSelected(%d)\n", i);
    return;
  }

  editGroup egdlg(*tmpKG);

  if (egdlg.exec() != 0)
    changed = TRUE;
}

void mainView::userSelected(int i) {
  KUser *tmpKU;

  tmpKU =  lbusers->getCurrentUser();
  if (tmpKU == NULL) {
    printf("Null pointer tmpKU in mainView::userSelected(%d)\n", i);
    return;
  }

#ifdef _KU_QUOTA
  Quota *tmpQ = 0;

  if (is_quota != 0) {
    tmpQ = kug->getQuotas()[tmpKU->getUID()];
    if (tmpQ == NULL) {
      printf("Null pointer tmpQ in mainView::selected(%d)\n", i);
      return;
    }
  }
  
  propdlg editUser(*tmpKU, *tmpQ, this, "userin");
#else
  propdlg editUser(*tmpKU, this, "userin");
#endif

  if (editUser.exec() != 0) {
    reloadUsers(lbusers->currentItem());
    changed = TRUE;
  }
}

void mainView::resizeEvent (QResizeEvent *rse) {
  QSize sz;

  sz = rse->size();

  kp->setGeometry(10, 10, sz.width()-20, sz.height()-20);
}

void mainView::grpadd() {
  int gid;

  if ((gid = kug->getGroups().first_free()) == -1) {
    err->display();
    return;
  }

  KGroup *tk = new KGroup();
  tk->setGID(gid);
  
  grpnamedlg gd(*tk, this);
  if (gd.exec() == 0) {
    delete tk;
    return;
  }

  kug->getGroups().add(tk);
  reloadGroups(lbgroups->currentItem());
  changed = TRUE;
}

void mainView::grpedit() {
  groupSelected(lbgroups->currentItem());
}

void mainView::grpdel() {
  if (QMessageBox::information(0, i18n("WARNING"),
                     i18n("Do you really want to delete group ?"),
                     i18n("Cancel"), i18n("Delete")) != 1)
    return;

  uint i = 0;
  bool islast = FALSE;

  i = lbgroups->currentItem();
  if (i == kug->getGroups().count()-1)
    islast = TRUE;

  kug->getGroups().del(lbgroups->getCurrentGroup());

  prev = -1;

  if (!islast)
    reloadGroups(i);
  else
    reloadGroups(i-1);
  changed = TRUE;
}

