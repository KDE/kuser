#include "maindlg.h"
#include "propdlg.h"
#include "pwddlg.h"
#include "pwdtool.h"
#include "sdwtool.h"
#include "misc.h"
#include "maindlg.moc"
#include "usernamedlg.h"

maindlg::maindlg( QWidget *parent, const char *name) :
KTopLevelWidget(name)
//, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | 
//                       WStyle_SysMenu | WStyle_MinMax)
{
  changed = FALSE;

  setCaption(name);
  QLabel *lb1 = addLabel(this, "lb1", 55, 40, 50, 20,_("User name"));
  lb1->setFont(rufont);
  QLabel *lb2 = addLabel(this, "lb2", 165, 40, 250, 20, _("Full name"));
  lb2->setFont(rufont);
  list = new QListBox( this, "ListBox_1" );
  list->setFont(rufont);
  QToolTip::add(list, _("Double click to select user"));
  list->setGeometry( 10, 60, 380, 208 );
  list->setAutoScrollBar(TRUE);
  list->setAutoUpdate(TRUE);
  list->setSmoothScrolling(FALSE);
  QObject::connect(list, SIGNAL(highlighted(int)), this, SLOT(highlighted(int)));
  QObject::connect(list, SIGNAL(selected(int)), this, SLOT(selected(int)));

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
  int editID = user->insertItem(_("Edit"), this, SLOT(edit()) );
  int delID = user->insertItem(_("Delete"), this, SLOT(del()) );
  int addID = user->insertItem(_("Add"), this, SLOT(add()) );
  int setpwdID = user->insertItem(_("Set password"), this, SLOT(setpwd()) );

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

  setFixedSize(400, 400);
}

void maindlg::reload(int id) {
  QPainter *p = new QPainter;
  QPixmap pix(353, 20);
  list->clear();
  for (KUser *curuser=users.first(); curuser!=NULL; curuser=users.next()) {
    pix.fill(white);
    p->begin(&pix);
    p->setFont(rufont);
    p->drawPixmap(3, 2, *pic_user);
    p->drawText(40, 14, curuser->p_name);
    p->drawText(150, 14, curuser->p_fname);
    p->end();
    list->insertItem(pix);
  }

  list->setCurrentItem(id);
}

static int prev = 0;

void maindlg::highlighted(int i) {
  QPainter *p = new QPainter;
  QPixmap pix(353, 20);

  if (prev != -1) {
    pix.fill(white);
    p->begin(&pix);
    p->setFont(rufont);
    p->drawPixmap(3, 2, *pic_user);
    p->drawText(40, 14, users.at(prev)->p_name);
    p->drawText(150, 14, users.at(prev)->p_fname);
    p->end();
    list->changeItem(pix, prev);
  }

  pix.fill(black);
  p->begin(&pix);
  p->setFont(rufont);
  p->drawPixmap(3, 2, *pic_user);
  p->setPen(white);
  p->drawText(40, 14, users.at(i)->p_name);
  p->drawText(150, 14, users.at(i)->p_fname);
  p->drawRect(1, 1, 352, 18);
  p->end();
  list->changeItem(pix, i);
  prev = i;
}

void maindlg::edit() {
  selected(list->currentItem());
}

void maindlg::del() {
  uint i = 0;
  bool islast = FALSE;
  
  if (QMessageBox::query(_("WARNING"),
                         _("Do you really want to delete user ?"),
                         _("Delete"), _("Cancel"))) {

    i = list->currentItem();
    if (i == users.count()-1)
      islast = TRUE;

    users.remove(i);

    list->removeItem(i);

    prev = -1;

    if (!islast) {
      list->setCurrentItem(i);
      highlighted(i);
    }
    else {
      list->setCurrentItem(i-1);
      highlighted(i-1);
    }
  }
}

void maindlg::add() {
  usernamedlg *u;
  propdlg *editUser;

  KUser *tk;
  tk = new KUser();
  u = new usernamedlg(tk, this);
  if (u->exec() == 0)
    return;

  delete u;

  config->setGroup("template");
  tk->p_shell = readentry("shell");
  tk->p_dir   = readentry("homeprefix")+"/"+tk->p_name;
  tk->p_gid   = readnumentry("gid");
  tk->p_fname = readentry("p_fname");
  tk->p_office1 = readentry("p_office1");
  tk->p_office2 = readentry("p_office2");
  tk->p_address = readentry("p_address");

  tk->s_lstchg = readnumentry("s_lstchg");
  tk->s_min = readnumentry("s_min");
  tk->s_max = readnumentry("s_max");
  tk->s_warn = readnumentry("s_warn");
  tk->s_inact = readnumentry("s_inact");
  tk->s_expire = readnumentry("s_expire");
  tk->s_flag = readnumentry("s_flag");

  tk->quota.at(0)->fsoft = readnumentry("quota.fsoft");
  tk->quota.at(0)->fhard = readnumentry("quota.fhard");
  tk->quota.at(0)->isoft = readnumentry("quota.isoft");
  tk->quota.at(0)->ihard = readnumentry("quota.ihard");

  editUser = new propdlg(tk, this, "userin");
  if (editUser->exec() != 0)
    users.append(tk);
  else
    delete tk;

  reload(users.count()-1);

  delete editUser;
}

void maindlg::quit() {
  if (changed == TRUE)
    if (QMessageBox::query(_("Data was modified"),
                           _("Would you like to save changes ?"),
                           _("Save"), _("Discard changes"))) {

      pwd_write();                                                                 
                                                                                
#ifdef _KU_SHADOW
      if (is_shadow)                                                               
        sdw_write();                                                                
#endif                                                                         
                                                                               
#ifdef _KU_QUOTA
#ifdef _KU_DEBUG
printf("%d\n", is_quota);
#endif

      if (is_quota)
        quota_write();
#endif
    }

  qApp->quit();
}

void maindlg::about() {
    sprintf(tmp, _("KUser version %s\nKDE project\nThis program was created by\nDenis Y. Pershin\ndyp@isis.nsu.ru\nCopyright 1997(c)"), _KU_VERSION);
    QMessageBox::message(_("Message"), tmp, "Ok");
}

void maindlg::setpwd() {
  pwddlg *d;

  d = new pwddlg(users.at(list->currentItem()), this, "pwddlg");
  d->exec();
  delete d;
}

void maindlg::help() {
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

void maindlg::properties() {
  propdlg *editUser;
  KUser *tk;

  tk = new KUser();
  config->setGroup("template");
  tk->p_shell = readentry("shell");
  tk->p_dir   = readentry("homeprefix");
  tk->p_gid   = readnumentry("gid");
  tk->p_fname = readentry("p_fname");
  tk->p_office1 = readentry("p_office1");
  tk->p_office2 = readentry("p_office2");
  tk->p_address = readentry("p_address");

  tk->s_lstchg = readnumentry("s_lstchg");
  tk->s_min = readnumentry("s_min");
  tk->s_max = readnumentry("s_max");
  tk->s_warn = readnumentry("s_warn");
  tk->s_inact = readnumentry("s_inact");
  tk->s_expire = readnumentry("s_expire");
  tk->s_flag = readnumentry("s_flag");

  tk->quota.at(0)->fsoft = readnumentry("quota.fsoft");
  tk->quota.at(0)->fhard = readnumentry("quota.fhard");
  tk->quota.at(0)->isoft = readnumentry("quota.isoft");
  tk->quota.at(0)->ihard = readnumentry("quota.ihard");

  editUser = new propdlg(tk, this, "userin");
  if (editUser->exec() != 0) {
    config->writeEntry("shell", tk->p_shell);
    config->writeEntry("homeprefix", tk->p_dir);
    config->writeEntry("gid", tk->p_gid);
    config->writeEntry("p_fname", tk->p_fname);
    config->writeEntry("p_office1", tk->p_office1);
    config->writeEntry("p_office2", tk->p_office2);
    config->writeEntry("p_address", tk->p_address);

    config->writeEntry("s_lstchg", tk->s_lstchg);
    config->writeEntry("s_min", tk->s_min);
    config->writeEntry("s_max", tk->s_max);
    config->writeEntry("s_warn", tk->s_warn);
    config->writeEntry("s_inact", tk->s_inact);
    config->writeEntry("s_expire", tk->s_expire);
    config->writeEntry("s_flag", tk->s_flag);

    config->writeEntry("quota.fsoft", tk->quota.at(0)->fsoft);
    config->writeEntry("quota.fhard", tk->quota.at(0)->fhard);
    config->writeEntry("quota.isoft", tk->quota.at(0)->isoft);
    config->writeEntry("quota.ihard", tk->quota.at(0)->ihard);
  }

  delete tk;
  delete editUser;
}

void maindlg::selected(int i) {
  propdlg *editUser;

  editUser = new propdlg(users.at(i), this, "userin");
  if (editUser->exec() != 0) {
    highlighted(list->currentItem());
    changed = TRUE;
#ifdef _KU_DEBUG
printf("Accepted\n");
#endif
  }

  delete editUser;
}
