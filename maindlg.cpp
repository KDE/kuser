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

  QObject::connect(list, SIGNAL(headerClicked(int)), this, SLOT(setSort(int)));
  QObject::connect(list, SIGNAL(selected(int)), this, SLOT(selected(int)));

  reload(0);

  pbquit = new QPushButton( this, "pbquit" );
  pbquit->setFont(rufont);
  QToolTip::add(pbquit, _("Quit KUser"));

  pbquit->setGeometry( 50, 350, 100, 30 );
  pbquit->setText(_("Quit"));
  QObject::connect(pbquit, SIGNAL(clicked()), this, SLOT(quit()));

  pbedit = new QPushButton( this, "pbedit" );
  pbedit->setFont(rufont);
  QToolTip::add(pbedit, _("Edit user profile"));
  pbedit->setGeometry( 250, 350, 100, 30 );
  pbedit->setText(_("Edit"));
  QObject::connect(pbedit, SIGNAL(clicked()), this, SLOT(edit()));

  pbdel = new QPushButton( this, "pbdel" );
  pbdel->setFont(rufont);
  QToolTip::add(pbdel, _("Delete user"));
  pbdel->setGeometry( 50, 300, 100, 30 );
  pbdel->setText(_("Delete"));
  QObject::connect(pbdel, SIGNAL(clicked()), this, SLOT(del()));

  pbadd = new QPushButton(this, "pbadd");
  pbadd->setFont(rufont);
  QToolTip::add(pbadd, _("Add user"));
  pbadd->setGeometry(250, 300, 100, 30);
  pbadd->setText(_("Add"));
  QObject::connect(pbadd, SIGNAL(clicked()), this, SLOT(add()));

  QPopupMenu *file = new QPopupMenu;
  CHECK_PTR( file );
  file->setFont(rufont);
  file->insertItem(_("Properties..."),  this, SLOT(properties()) );
  file->insertSeparator();
  file->insertItem(_("Quit"),  this, SLOT(quit()) );

  QPopupMenu *user = new QPopupMenu;
  CHECK_PTR(user);
  user->setFont(rufont);
  user->insertItem(_("Edit..."), this, SLOT(edit()) );
  user->insertItem(_("Delete..."), this, SLOT(del()) );
  user->insertItem(_("Add..."), this, SLOT(add()) );
  user->insertItem(_("Set password..."), this, SLOT(setpwd()) );

  QPopupMenu *help = new QPopupMenu;
  CHECK_PTR( help );
  help->setFont(rufont);
  help->insertItem(_("Help"), this, SLOT(help()));
  help->insertSeparator();
  help->insertItem(_("About..."), this, SLOT(about()));

  menubar = new KMenuBar( this );
  CHECK_PTR( menubar );
  menubar->setFont(rufont);
  menubar->insertItem(_("File"), file );
  menubar->insertItem(_("User"), user );
  menubar->insertSeparator();
  menubar->insertItem(_("Help"), help );

  setMenu(menubar);

  toolbar = new KToolBar(this, "toolbar");
  QPixmap pixmap;

  pixmap = kapp->getIconLoader()->loadIcon("profile_bw.xpm");
  toolbar->insertButton(pixmap, 0, SIGNAL(clicked()), this, SLOT(edit()), TRUE, _("Edit user"));
  toolbar->setBarPos(KToolBar::Top);

  addToolBar(toolbar);

  resize(400, 400);
}

mainDlg::~mainDlg() {
  delete menubar;
  delete toolbar;

  delete u;

#ifdef _KU_QUOTA
  delete q;
  delete m;
#endif
}

void mainDlg::setSort(int col) {
  if (sort == col)
    sort = -1;
  else
    sort = col;

  list->sortBy(sort);

  reload(list->currentItem());
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
  list->repaint();
  list->setCurrentItem(id);
}

void mainDlg::edit() {
  selected(list->currentItem());
}

void mainDlg::del() {
  uint i = 0;
  bool islast = FALSE;

  if (KMsgBox::yesNo(0, _("WARNING"),
                     _("Do you really want to delete user ?"),
                     KMsgBox::STOP,
                     _("Cancel"), _("Delete")) == 2) {

    i = list->currentItem();
    if (i == u->getUsersNumber()-1)
      islast = TRUE;

    unsigned int uid = list->getCurrentUser()->p_uid;

    u->delUser(list->getCurrentUser());

#ifdef _KU_QUOTA
    if (u->user_lookup(uid) == NULL)
      q->delQuota(uid);
#endif

    prev = -1;

    if (!islast)
      reload(i);
    else
      reload(i-1);
    changed = TRUE;
  }
}

void mainDlg::add() {
  usernamedlg *ud;
  propdlg *editUser;

  KUser *tk;
#ifdef _KU_QUOTA
  Quota *tq;
#endif // _KU_QUOTA

  tk = new KUser();

  tk->p_uid = u->first_free();

#ifdef _KU_QUOTA
  tq = new Quota(tk->p_uid, FALSE);
#endif // _KU_QUOTA

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
    editUser = new propdlg(tk, NULL, this, "userin");
#else
  editUser = new propdlg(tk, this, "userin");
#endif

  if (editUser->exec() != 0) {
    u->addUser(tk);
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

  delete this;
  qApp->quit();
}

void mainDlg::about() {
    QString tmp;
    tmp.sprintf(_("KUser version %s\nKDE project\nThis program was created by\nDenis Y. Pershin\ndyp@inetlab.com\nCopyright 1997(c)"), _KU_VERSION);
    KMsgBox::message(0, _("Message"), tmp, KMsgBox::INFORMATION);
}

void mainDlg::setpwd() {
  pwddlg *d;
  
  d = new pwddlg(list->getCurrentUser(), this, "pwddlg");
  d->exec();
  delete d;
}

void mainDlg::help() {
	kapp->invokeHTMLHelp(0,0);
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

  tmpKU =  list->getCurrentUser();
  if (tmpKU == NULL) {
    printf(_("Null pointer tmpKU in mainDlg::selected(%d)\n"), i);
    return;
  }

#ifdef _KU_QUOTA
  Quota *tmpQ;

  tmpQ = q->getQuota(tmpKU->p_uid);
  if (tmpQ == NULL) {
    printf(_("Null pointer tmpQ in mainDlg::selected(%d)\n"), i);
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

void mainDlg::resizeEvent (QResizeEvent *rse) {
  QSize sz;

  sz = rse->size();

  list->setGeometry(10,80,sz.width()-20,sz.height()-192);

  pbquit->move(sz.width()/8, sz.height()-50);
  pbedit->move(sz.width()*7/8-100, sz.height()-50);
  pbdel->move(sz.width()/8, sz.height()-100);
  pbadd->move(sz.width()*7/8-100, sz.height()-100);

  updateRects();
}

