#define _KU_MAIN
#include "mainView.h"

#include "globals.h"

#include <stdio.h>

#include <unistd.h>
#include <signal.h>

#include <qtooltip.h>
#include <qsplitter.h>

#include <ktoolbar.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include "misc.h"
#include "kglobal_.h"
#include "usernamedlg.h"
#include "grpnamedlg.h"
#include "propdlg.h"
#include "addUser.h"
#include "pwddlg.h"
#include "editGroup.h"
#include "editDefaults.h"

mainView::mainView(QWidget *parent) : QTabWidget(parent) {
  changed = false;

  init();
}

void mainView::init() {

  lbusers = new KUserView(this, "lbusers");
  addTab(lbusers, i18n("Users"));

  lbgroups = new KGroupView(this, "lbgroups");
  addTab(lbgroups, i18n("Groups"));

  connect(lbusers, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(userSelected()));
  connect(lbusers, SIGNAL(returnPressed(QListViewItem *)), this, SLOT(userSelected()));

  connect(lbgroups, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(groupSelected()));
  connect(lbgroups, SIGNAL(returnPressed(QListViewItem *)), this, SLOT(groupSelected()));

  reloadUsers();
  reloadGroups();

  connect(this, SIGNAL(currentChanged(QWidget *)), this, SLOT(slotTabChanged()));
}

mainView::~mainView() {
}

void mainView::slotTabChanged()
{
  if (currentPage() == lbusers)
  {
     emit userSelected(true);
     emit groupSelected(false);
  }
  else
  {
     emit userSelected(false);
     emit groupSelected(true);
  }
}

void mainView::reloadUsers() {
  KUser *ku;

  lbusers->clear();

  for (uint i = 0; i<kug->getUsers().count(); i++) {
    ku = kug->getUsers()[i];
    lbusers->insertItem(ku);
  }
  if (lbusers->firstChild())
    lbusers->setSelected(lbusers->firstChild(), true);
}

void mainView::reloadGroups() {
  KGroup *kg;

  lbgroups->clear();

  for (uint i = 0; i<kug->getGroups().count(); i++) {
    kg = kug->getGroups()[i];
    lbgroups->insertItem(kg);
  }
  if (lbgroups->firstChild())
    lbgroups->setSelected(lbgroups->firstChild(), true);

}

void mainView::useredit() {
  userSelected();
}

void mainView::userdel() {
  KUser *user = lbusers->getCurrentUser();

  if (KMessageBox::warningContinueCancel(0, i18n("Do you really want to delete user '%1'?")
		       .arg(user->getName()), QString::null,
                     i18n("&Delete")) != KMessageBox::Continue)
    return;

#ifdef _KU_QUOTA
  uint uid = user->getUID();

  if (kug->getUsers().lookup(uid) == 0)
    kug->getQuotas().delQuota(uid);
#endif

  kug->getUsers().del(user);
  lbusers->removeItem(user);

  changed = true;

  config->setGroup("template"); 
  if (!config->readBoolEntry("userPrivateGroup", KU_USERPRIVATEGROUP))
    return;
    
  const QString &userName = user->getName();
  KGroup *group = 0;

  for (uint i=0; i<kug->getGroups().count(); i++)
    if (kug->getGroups()[i]->getName() == userName) {
      group = kug->getGroups()[i];
      break;
    }

  if (!group)
    return;

  if (KMessageBox::warningContinueCancel(0,
        i18n("You are using private groups.\n"
             "Do you want to delete the user's private group '%1'?")
	       .arg(group->getName()), QString::null,
	     i18n("&Delete")) != KMessageBox::Continue)
    return;

  kug->getGroups().del(group);
  lbgroups->removeItem(group);
}

void mainView::useradd() {
  KUser *tk;
#ifdef _KU_QUOTA
  Quota *tq;
#endif // _KU_QUOTA

  showPage(lbusers);

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
  {
    delete tk;
#ifdef _KU_QUOTA
    delete tq;
#endif
    return;
  }

  config->setGroup("template");
  tk->setShell(readentry("shell"));
  tk->setHomeDir(readentry("homeBase", KU_HOMEPREFIX)+"/"+tk->getName());
  tk->setGID(readnumentry("gid"));
  tk->setFullName(readentry("p_fname"));
#if defined(__FreeBSD__) || defined(__bsdi__)
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

#ifdef HAVE_SHADOW
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

#ifdef _KU_QUOTA
  if (is_quota == 0) {
    delete tq;
    tq = 0;
  }
#endif // _KU_QUOTA
  
#ifdef _KU_QUOTA
  addUser au(tk, tq, this, "userin");
#else
  addUser au(tk, this, "userin");
#endif

  au.setCreateHomeDir(config->readBoolEntry("createHomeDir", true));
  au.setCopySkel(config->readBoolEntry("copySkel", true));
  au.setUsePrivateGroup(config->readBoolEntry("userPrivateGroup", KU_USERPRIVATEGROUP));

  if (au.exec() != 0) {
    if (au.getUsePrivateGroup()) {
      KGroup *tg;

      if ((tg = kug->getGroups().lookup(tk->getName())) == 0) {
        tg = new KGroup();
        tg->setGID(kug->getGroups().first_free());
        tg->setName(tk->getName());
        kug->getGroups().add(tg);
        lbgroups->insertItem(tg);
      }

      tk->setGID(tg->getGID());
    }
    kug->getUsers().add(tk);
#ifdef _KU_QUOTA
    if (tq)
      kug->getQuotas().addQuota(tq);
#endif
    changed = true;
    lbusers->insertItem(tk);
  }
  else {
    delete tk;
#ifdef _KU_QUOTA
    delete tq;
#endif
  }

}


void mainView::save() {
  if (!changed)
    return;
    
  if (KMessageBox::questionYesNo(0, i18n("Would you like to save changes?"),
                               i18n("Data Was Modified"),
                               i18n("&Save"), i18n("&Discard Changes")) == KMessageBox::No)
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
  delete this;
}

void mainView::setpwd() {
  pwddlg d(lbusers->getCurrentUser(), this, "pwddlg");
  if (d.exec() != 0)
    changed = true;
}

void mainView::properties() {
  editDefaults eddlg;

  eddlg.setCaption(i18n("Edit Defaults"));

  config->setGroup("template");
  eddlg.setShell(config->readEntry("shell", ""));
  eddlg.setHomeBase(config->readEntry("homeBase", "/home"));	
  eddlg.setCreateHomeDir(config->readBoolEntry("createHomeDir", true));
  eddlg.setCopySkel(config->readBoolEntry("copySkel", true));
  eddlg.setUserPrivateGroup(config->readBoolEntry("userPrivateGroup", KU_USERPRIVATEGROUP));
  config->setGroup("sources");
  eddlg.setPasswdSrc(config->readEntry("passwdsrc", PASSWORD_FILE)); 
  eddlg.setGroupSrc(config->readEntry("groupsrc", GROUP_FILE));
  eddlg.setNISPasswdSrc(config->readEntry("nispasswdsrc", ""));
  eddlg.setNISGroupSrc(config->readEntry("nisgroupsrc", ""));
  eddlg.setMINUID(config->readEntry("nisminuid", ""));
  eddlg.setMINGID(config->readEntry("nismingid", ""));
//  eddlg.setEnableNIS(config->readBoolEntry("enableNIS", true));


  if (eddlg.exec() != 0) {
    config->setGroup("template");
    config->writeEntry("shell", eddlg.getShell());
    config->writeEntry("homeBase", eddlg.getHomeBase());
    config->writeEntry("createHomeDir", eddlg.getCreateHomeDir());
    config->writeEntry("copySkel", eddlg.getCopySkel());
    config->writeEntry("userPrivateGroup", eddlg.getUserPrivateGroup());
    config->setGroup("sources");
    config->writeEntry("passwdsrc", eddlg.getPasswdSrc());
    config->writeEntry("nispasswdsrc", eddlg.getNISPasswdSrc());
    config->writeEntry("groupsrc", eddlg.getGroupSrc());
    config->writeEntry("nisgroupsrc", eddlg.getNISGroupSrc());
    config->writeEntry("nisminuid", eddlg.getMINUID());
    config->writeEntry("nismingid", eddlg.getMINGID());
//    config->writeEntry("enableNIS", eddlg.getEnableNIS());
  }
}

void mainView::groupSelected() {
  KGroup *tmpKG = lbgroups->getCurrentGroup();

  if (!tmpKG) {
    printf("Null pointer tmpKG in mainView::groupSelected()\n");
    return;
  }

  editGroup egdlg(tmpKG);

  if (egdlg.exec() != 0)
    changed = true;
}

void mainView::userSelected() {
  KUser *tmpKU;

  tmpKU =  lbusers->getCurrentUser();
  if (!tmpKU) 
    return;

#ifdef _KU_QUOTA
  Quota *tmpQ = 0;

  if (is_quota != 0) {
    tmpQ = kug->getQuotas()[tmpKU->getUID()];
    if (!tmpQ) {
      printf("Null pointer tmpQ in mainView::selected()\n");
      return;
    }
  }
  
  propdlg editUser(tmpKU, tmpQ, this, "userin");
#else
  propdlg editUser(tmpKU, this, "userin");
#endif

  if (editUser.exec() != 0) {
    changed = true;
  }
}

void mainView::grpadd() {
  showPage(lbgroups);

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
  lbgroups->insertItem(tk);
  changed = true;
}

void mainView::grpedit() {
  groupSelected();
}

void mainView::grpdel() {
  KGroup *group = lbgroups->getCurrentGroup();
  if (KMessageBox::warningContinueCancel(0, i18n("Do you really want to delete the group '%1'?").arg(group->getName()),
                     QString::null, i18n("&Delete")) != KMessageBox::Continue)
    return;

  kug->getGroups().del(group);
  lbgroups->removeItem(group);
  changed = true;
}

#include "mainView.moc"
