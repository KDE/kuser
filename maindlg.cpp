#define _KU_MAIN
#include "maindlg.h"
#include "maindlg.moc"

#include "globals.h"
#include <qtooltip.h>
#include <kmsgbox.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <unistd.h>

#ifdef _KU_QUOTA
#include "quota.h"
#include "mnt.h"
#endif

#include "kuser.h"
#include "misc.h"
#include "usernamedlg.h"
#include "propdlg.h"
#include "pwddlg.h"

mainDlg::mainDlg(const char *name) :
KTopLevelWidget(name)
{
#ifdef _KU_DEBUG
printf("mainDlg::mainDlg\n");
#endif

  changed = FALSE;
  prev = 0;

  setCaption(name);
}

void mainDlg::init() {
printf("mainDlg::init()\n");
#ifdef _KU_QUOTA
  m = new Mounts();
  q = new Quotas();
#endif

  u = new KUsers();

  list = new KUserView(this, "list");
  list->setGeometry(10,80,380,208);

//  QObject::connect(list, SIGNAL(headerClicked(int)), this, SLOT(setSort(int)));
  QObject::connect(list, SIGNAL(selected(int)), this, SLOT(selected(int)));

  QLabel *lb1 = addLabel(this, "lb1", 55, 40, 50, 20,_("User name"));
  lb1->setFont(rufont);
  QLabel *lb2 = addLabel(this, "lb2", 165, 40, 250, 20, _("Full name"));
  lb2->setFont(rufont);

  reload(0);

  QPushButton* pbquit;
  pbquit = new QPushButton( this, "pbquit" );
  pbquit->setFont(rufont);
  QToolTip::add(pbquit, _("Quit KUser"));

  pbquit->setGeometry( 50, 350, 100, 30 );
  pbquit->setText(_("Quit"));
  QObject::connect(pbquit, SIGNAL(clicked()), this, SLOT(quit()));

  QPushButton* pbedit;
  pbedit = new QPushButton( this, "pbedit" );
  pbedit->setFont(rufont);
  QToolTip::add(pbedit, _("Edit user profile"));
  pbedit->setGeometry( 250, 350, 100, 30 );
  pbedit->setText(_("Edit"));
  QObject::connect(pbedit, SIGNAL(clicked()), this, SLOT(edit()));

  QPushButton* pbdel;
  pbdel = new QPushButton( this, "pbdel" );
  pbdel->setFont(rufont);
  QToolTip::add(pbdel, _("Delete user"));
  pbdel->setGeometry( 50, 300, 100, 30 );
  pbdel->setText(_("Delete"));
  QObject::connect(pbdel, SIGNAL(clicked()), this, SLOT(del()));

  QPushButton* pbadd;
  pbadd = new QPushButton(this, "pbadd");
  pbadd->setFont(rufont);
  QToolTip::add(pbadd, _("Add user"));
  pbadd->setGeometry(250, 300, 100, 30);
  pbadd->setText(_("Add"));
  QObject::connect(pbadd, SIGNAL(clicked()), this, SLOT(add()));

  QPopupMenu *file = new QPopupMenu;
  CHECK_PTR( file );
  file->setFont(rufont);
  file->insertItem(_("Properties"),  this, SLOT(properties()) );
  file->insertSeparator();
  file->insertItem(_("Quit"),  this, SLOT(quit()) );

  QPopupMenu *user = new QPopupMenu;
  CHECK_PTR(user);
  user->setFont(rufont);
  user->insertItem(_("Edit"), this, SLOT(edit()) );
  user->insertItem(_("Delete"), this, SLOT(del()) );
  user->insertItem(_("Add"), this, SLOT(add()) );
  user->insertItem(_("Set password"), this, SLOT(setpwd()) );

  QPopupMenu *help = new QPopupMenu;
  CHECK_PTR( help );
  help->setFont(rufont);
  help->insertItem(_("About"), this, SLOT(about()));
  help->insertSeparator();
  help->insertItem(_("Help"), this, SLOT(help()));

  KMenuBar *menu = new KMenuBar( this );
  CHECK_PTR( menu );
  menu->setFont(rufont);
  menu->insertItem(_("File"), file );
  menu->insertItem(_("User"), user );
  menu->insertItem(_("Help"), help );

  setMenu(menu);

  KToolBar *toolbar;

  toolbar = new KToolBar(this, "toolbar");
  QPixmap pixmap;

  pixmap = kapp->getIconLoader()->loadIcon("profile_bw.xpm");
  toolbar->insertButton(pixmap, 0, SIGNAL(clicked()), this, SLOT(edit()), TRUE, _("Edit user"));
  toolbar->setBarPos(KToolBar::Top);

  addToolBar(toolbar);

  setFixedSize(400, 400);
}

mainDlg::~mainDlg() {
  delete u;

#ifdef _KU_QUOTA
  delete q;
  delete m;
#endif
}

void mainDlg::setSort(int col) {
  printf("Sort by %d\n", col);
}

void mainDlg::reload(int id) {
  KUser *ku;

  list->setAutoUpdate(FALSE);
  list->clear();

  for (uint i = 0; i<u->getUsersNumber(); i++) {
    ku = u->getUser(i);
    list->insertItem(ku);
  }

  list->setAutoUpdate(TRUE);
  list->setCurrentItem(id);
}

void mainDlg::edit() {
printf("mainDlg::edit()\n currentItem = %d\n", list->currentItem());
  selected(list->currentItem());
}

void mainDlg::del() {
  uint i = 0;
  bool islast = FALSE;

  if (KMsgBox::yesNo(0, _("WARNING"),
                     _("Do you really want to delete user ?"),
                     KMsgBox::STOP,
                     _("Delete"), _("Cancel")) == 1) {

    i = list->currentItem();
    if (i == u->getUsersNumber()-1)
      islast = TRUE;

    unsigned int uid = u->getUser(i)->p_uid;

    u->delUser(i);

#ifdef _KU_QUOTA
    if (u->user_lookup(uid) == NULL)
      q->delQuota(uid);
#endif

    prev = -1;

    if (!islast)
      reload(i);
    else
      reload(i-1);
  }
}

void mainDlg::add() {
  usernamedlg *ud;
  propdlg *editUser;

  KUser *tk;
  Quota *tq;

  tk = new KUser();

  tk->p_uid = u->first_free();
  tq = new Quota(tk->p_uid, FALSE);

  ud = new usernamedlg(tk, this);
  if (ud->exec() == 0)
    return;

  delete ud;

  config->setGroup("template");
  tk->p_shell = readentry("shell");
  tk->p_dir   = readentry("homeprefix")+"/"+tk->p_name;
  tk->p_gid   = readnumentry("gid");
  tk->p_fname = readentry("p_fname");
  tk->p_office1 = readentry("p_office1");
  tk->p_office2 = readentry("p_office2");
  tk->p_address = readentry("p_address");

#ifdef _KU_SHADOW
  tk->s_lstchg = readnumentry("s_lstchg");
  tk->s_min = readnumentry("s_min");
  tk->s_max = readnumentry("s_max");
  tk->s_warn = readnumentry("s_warn");
  tk->s_inact = readnumentry("s_inact");
  tk->s_expire = readnumentry("s_expire");
  tk->s_flag = readnumentry("s_flag");
#endif

#ifdef _KU_QUOTA
  if (is_quota)
    editUser = new propdlg(tk, tq, this, "userin");
  else
    editUser = new propdlg(tk, tq, this, "userin");
#else
  editUser = new propdlg(tk, this, "userin");
#endif

  if (editUser->exec() != 0) {
    u->addUser(tk);
    q->addQuota(tq);
    changed = TRUE;
  }
  else {
    delete tk;
    delete tq;
  }

  reload(u->getUsersNumber()-1);

  delete editUser;
}

void mainDlg::quit() {
 if (changed == TRUE)
   if (KMsgBox::yesNo(0, _("Data was modified"),
                      _("Would you like to save changes ?"),
  		      KMsgBox::INFORMATION,
		      _("Save"), _("Discard changes")) == 1) {
      u->save();
#ifdef _KU_QUOTA
      if (is_quota)
        q->save();
#endif
    }

  qApp->quit();
}

void mainDlg::about() {
    QString tmp;
    tmp.sprintf(_("KUser version %s\nKDE project\nThis program was created by\nDenis Y. Pershin\ndyp@isis.nsu.ru\nCopyright 1997(c)"), _KU_VERSION);
    KMsgBox::message(0, _("Message"), tmp, KMsgBox::INFORMATION);
}

void mainDlg::setpwd() {
  pwddlg *d;
  
  d = new pwddlg(u->getUser(list->currentItem()), this, "pwddlg");
  d->exec();
  delete d;
}

void mainDlg::help() {
  int id = 0;

  id = fork();

  if (id == 0) {
    char path[1024];

    strncpy(path, getenv("KDEDIR"), 1024);
    strncat(path, "/bin/kdehelp file:/", 1024);
    strncat(path, getenv("KDEDIR"), 1024);
    strncat(path, "/doc/kuser/index.html", 1024);

    system(path);
    exit(0);
  }
}

void mainDlg::properties() {
  propdlg *editUser;
  KUser *tk;

  tk = new KUser();
#ifdef _KU_QUOTA
  Quota *tq = new Quota(0, FALSE);
#endif

  config->setGroup("template");
  tk->p_shell = readentry("shell");
  tk->p_dir   = readentry("homeprefix");
  tk->p_gid   = readnumentry("gid");
  tk->p_fname = readentry("p_fname");
  tk->p_office1 = readentry("p_office1");
  tk->p_office2 = readentry("p_office2");
  tk->p_address = readentry("p_address");

#ifdef _KU_SHADOW
  tk->s_lstchg = readnumentry("s_lstchg");
  tk->s_min = readnumentry("s_min");
  tk->s_max = readnumentry("s_max");
  tk->s_warn = readnumentry("s_warn");
  tk->s_inact = readnumentry("s_inact");
  tk->s_expire = readnumentry("s_expire");
  tk->s_flag = readnumentry("s_flag");
#endif

#ifdef _KU_QUOTA
  if (is_quota) {
    /*
    tk->quota.at(0)->fsoft = readnumentry("quota.fsoft");
    tk->quota.at(0)->fhard = readnumentry("quota.fhard");
    tk->quota.at(0)->isoft = readnumentry("quota.isoft");
    tk->quota.at(0)->ihard = readnumentry("quota.ihard");
    */
  }
#endif

#ifdef _KU_QUOTA
  editUser = new propdlg(tk, tq, this, "userin");
#else
  editUser = new propdlg(tk, this, "userin");
#endif
  if (editUser->exec() != 0) {
    config->writeEntry("shell", tk->p_shell);
    config->writeEntry("homeprefix", tk->p_dir);
    config->writeEntry("gid", tk->p_gid);
    config->writeEntry("p_fname", tk->p_fname);
    config->writeEntry("p_office1", tk->p_office1);
    config->writeEntry("p_office2", tk->p_office2);
    config->writeEntry("p_address", tk->p_address);

#ifdef _KU_SHADOW
    config->writeEntry("s_lstchg", tk->s_lstchg);
    config->writeEntry("s_min", tk->s_min);
    config->writeEntry("s_max", tk->s_max);
    config->writeEntry("s_warn", tk->s_warn);
    config->writeEntry("s_inact", tk->s_inact);
    config->writeEntry("s_expire", tk->s_expire);
    config->writeEntry("s_flag", tk->s_flag);
#endif

#ifdef _KU_QUOTA
    if (is_quota) {
      /*
      config->writeEntry("quota.fsoft", tk->quota.at(0)->fsoft);
      config->writeEntry("quota.fhard", tk->quota.at(0)->fhard);
      config->writeEntry("quota.isoft", tk->quota.at(0)->isoft);
      config->writeEntry("quota.ihard", tk->quota.at(0)->ihard);
      */
    }
#endif
  }

  delete tk;
#ifdef _KU_QUOTA
  delete tq;
#endif
  delete editUser;
}

void mainDlg::selected(int i) {
  propdlg *editUser;
  KUser *tmpKU;

  tmpKU =  u->getUser(i);
  if (tmpKU == NULL) {
    printf("Null pointer tmpKU in mainDlg::selected(%d)\n", i);
    return;
  }

#ifdef _KU_QUOTA
  Quota *tmpQ;

  tmpQ = q->getQuota(tmpKU->p_uid);
  if (tmpQ == NULL) {
    printf("Null pointer tmpQ in mainDlg::selected(%d)\n", i);
    return;
  }

  editUser = new propdlg(tmpKU, tmpQ, this, "userin");
#else
  editUser = new propdlg(tmpKU, this, "userin");
#endif

  if (editUser->exec() != 0) {
    reload(list->currentItem());
    changed = TRUE;
  }

  delete editUser;
}

KUsers *mainDlg::getUsers() {
  return (u);
}

#ifdef _KU_QUOTA
Mounts *mainDlg::getMounts() {
  return (m);
}

Quotas *mainDlg::getQuotas() {
  return (q);
}
#endif
