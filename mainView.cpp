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

#define _KU_MAIN
#include "mainView.h"

#include "globals.h"

#include <stdio.h>

#include <unistd.h>
#include <signal.h>

#include <qtooltip.h>
#include <qsplitter.h>
#include <qfile.h>

#include <kinputdialog.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include "misc.h"
#include "kglobal_.h"
#include "grpnamedlg.h"
#include "propdlg.h"
#include "addUser.h"
#include "delUser.h"
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

  if (!user)
    return;

  delUser dlg(user, this);

  if (!dlg.exec())
     return;

#ifdef _KU_QUOTA
  uint uid = user->getUID();

  if (kug->getUsers().lookup(uid) == 0)
    kug->getQuotas().delQuota(uid);
#endif

  kug->getUsers().del(user, dlg.getDeleteHomeDir(), dlg.getDeleteMailBox());
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

  bool ok;
  QString name = KInputDialog::getText( i18n( "Add User" ),
      i18n( "Enter user name:" ), QString::null, &ok, this );

  if (!ok)  {
    delete tk;
#ifdef _KU_QUOTA
    delete tq;
#endif
    return;
  }
  tk->setName(name);

  if (kug->getUsers().lookup(name)) {
    KMessageBox::error( 0, i18n("User with name %1 already exists.").arg(name) );
    delete tk;
#ifdef _KU_QUOTA
    delete tq;
#endif
    return;
  }

  config->setGroup("template");
  tk->setShell(config->readPathEntry("shell", QString::fromLatin1("/bin/sh")));
  tk->setHomeDir(config->readPathEntry("homeBase", QFile::decodeName(KU_HOMEPREFIX))+
  	QString::fromLatin1("/")+tk->getName());
  tk->setGID(config->readNumEntry("gid"));
  tk->setFullName(config->readEntry("p_fname"));
#if defined(__FreeBSD__) || defined(__bsdi__)
  tk->setOffice(config->readEntry("p_office"));
  tk->setWorkPhone(config->readEntry("p_ophone"));
  tk->setHomePhone(config->readEntry("p_hphone"));
  tk->setClass(config->readEntry("p_class"));
  tk->setLastChange(config->readNumEntry("p_change"));
  tk->setExpire(config->readNumEntry("p_expire"));
#else
  tk->setOffice1(config->readEntry("p_office1"));
  tk->setOffice2(config->readEntry("p_office2"));
  tk->setAddress(config->readEntry("p_address"));
#endif

#ifdef HAVE_SHADOW
  tk->setLastChange(today());
  if (config->hasKey("s_min"))
    tk->setMin(config->readNumEntry("s_min"));

  if (config->hasKey("s_max"))
    tk->setMax(config->readNumEntry("s_max"));

  if (config->hasKey("s_warn"))
    tk->setWarn(config->readNumEntry("s_warn"));

  if (config->hasKey("s_inact"))
    tk->setInactive(config->readNumEntry("s_inact"));

  if (config->hasKey("s_expire"))
    tk->setExpire(config->readNumEntry("s_expire"));

  if (config->hasKey("s_flag"))
    tk->setFlag(config->readNumEntry("s_flag"));
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
    
  kug->getUsers().save();
  kug->getGroups().save();
#ifdef _KU_QUOTA
  if (is_quota != 0)
    kug->getQuotas().save();
#endif
  err->display();

  changed = FALSE;
}

bool mainView::queryClose()
{
  if (!changed)
    return true;
    
  int result = KMessageBox::warningYesNoCancel(0, i18n("Would you like to save your changes?"),
                               i18n("Data Was Modified"),
                               i18n("&Save"), i18n("&Discard Changes"));

  if (result == KMessageBox::Cancel)
    return false;
    
  if (result == KMessageBox::Yes)
    save();
    
  return true;
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
  eddlg.setShell(config->readEntry("shell", QString::fromLatin1("/bin/sh")));
  eddlg.setHomeBase(config->readEntry("homeBase", QString::fromLatin1("/home")));	
  eddlg.setCreateHomeDir(config->readBoolEntry("createHomeDir", true));
  eddlg.setCopySkel(config->readBoolEntry("copySkel", true));
  eddlg.setUserPrivateGroup(config->readBoolEntry("userPrivateGroup", KU_USERPRIVATEGROUP));
  config->setGroup("sources");
  eddlg.setPasswdSrc(config->readEntry("passwdsrc", QFile::decodeName(PASSWORD_FILE)));
  eddlg.setGroupSrc(config->readEntry("groupsrc", QFile::decodeName(GROUP_FILE)));
  eddlg.setNISPasswdSrc(config->readEntry("nispasswdsrc"));
  eddlg.setNISGroupSrc(config->readEntry("nisgroupsrc"));
  eddlg.setMINUID(config->readEntry("nisminuid"));
  eddlg.setMINGID(config->readEntry("nismingid"));
//  eddlg.setEnableNIS(config->readBoolEntry("enableNIS", true));


  if (eddlg.exec() != 0)
  {
    config->setGroup("template");
    config->writePathEntry("shell", eddlg.getShell());
    config->writePathEntry("homeBase", eddlg.getHomeBase());
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
    config->sync();
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
