#include "propdlg.h"
#include "propdlg.moc"
#include "pwddlg.h"

propdlg::propdlg(KUser *auser, QWidget *parent, const char *name, int isprep)
       :QTabDialog(parent, name, FALSE,
       WStyle_Customize | WStyle_DialogBorder | WStyle_SysMenu |
       WStyle_MinMax | WType_Modal) {

  user = auser;
  chquota = -1;

  QObject::connect(this, SIGNAL(applyButtonPressed()), this, SLOT(ok()));
  setCancelButton();
  QObject::connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(cancel()));

  setFont(rufont);
  QWidget *w1 = new QWidget(this, "wd_Password");
  w1->setFont(rufont);

  leuser = addLabel(w1, "leuser", 10, 27, 150, 27, user->p_name);
  leuser->setFont(rufont);
  QToolTip::add(leuser, _("User name"));
  QLabel *l1 = addLabel(w1, "ml1", 10, 10, 50, 20, _("User login"));
  l1->setFont(rufont);
  leid = addLineEdit(w1, "leid", 200, 30, 70, 20, "");
  leid->setFont(rufont);
  QToolTip::add(leid, _("User identificator"));
  QLabel *l2 = addLabel(w1, "ml2", 200, 10, 50, 20, _("User id"));
  l2->setFont(rufont);
  legid = addLineEdit(w1, "legid", 200, 85, 70, 20, "");
  legid->setFont(rufont);
  QToolTip::add(legid, _("Group identificator"));
  QLabel *ld3 = addLabel(w1, "mld3", 200, 60, 50, 20, _("Primary group id"));
  ld3->setFont(rufont);

  QPushButton *pbsetpwd = new QPushButton(w1, "pbsetpwd");
  pbsetpwd->setFont(rufont);
  pbsetpwd->setGeometry(240, 180, 80, 20);
  pbsetpwd->setText("Set password");
  QObject::connect(pbsetpwd, SIGNAL(clicked()), this, SLOT(setpwd()));

  lefname = addLineEdit(w1, "lefname", 10, 80, 160, 20, "");
  lefname->setFont(rufont);
  QToolTip::add(lefname, _("Full name"));
  QLabel *l4 = addLabel(w1, "ml4", 10, 60, 50, 20, _("Full name"));
  l4->setFont(rufont);

  leshell = new QComboBox(TRUE, w1, "leshell");
  leshell->setFont(rufont);
  QToolTip::add(leshell, _("Login shell"));
  leshell->clear();
  leshell->insertItem("<Empty>");

  FILE *f = fopen("/etc/shells","r");
  while (!feof(f)) {
    char s[200];

    fgets(s, 200, f);
    if (feof(f))
      break;
    s[strlen(s)-1]=0;
    if ((s[0])&&(s[0]!='#'))
      leshell->insertItem(s);
  }
  fclose(f);
  leshell->setGeometry(10, 122, 160, 27);
  QObject::connect(leshell, SIGNAL(activated(const char *)), this, SLOT(shactivated(const char *)));

  QLabel *l5 = addLabel(w1, "ml5", 10, 105, 50, 20, _("Login shell"));
  l5->setFont(rufont);
  lehome = addLineEdit(w1, "lehome", 10, 175, 160, 20, "");
  lehome->setFont(rufont);
  QToolTip::add(lehome, _("Home directory"));
  QLabel *l6 = addLabel(w1, "ml6", 10, 155, 50, 20, _("Home directory"));
  l6->setFont(rufont);
  leoffice1 = addLineEdit(w1, "leoffice1", 10, 220, 160, 20, "");
  leoffice1->setFont(rufont);
  QToolTip::add(leoffice1, _("The first office"));
  QLabel *l7 = addLabel(w1, "ml7", 10, 200, 50, 20, _("Office1"));
  l7->setFont(rufont);

  QLabel *ld7 = addLabel(w1, "mld7", 190, 225, 50, 20, _("First office description"));
  ld7->setFont(rufont);

  leoffice2 = addLineEdit(w1, "leoffice2", 10, 265, 160, 20, "");
  leoffice2->setFont(rufont);
  QToolTip::add(leoffice2, _("The second office"));
  QLabel *l8 = addLabel(w1, "ml8", 10, 245, 50, 20, _("Office2"));
  l8->setFont(rufont);
  QLabel *ld8 = addLabel(w1, "mld8", 190, 270, 50, 20, _("Second office description"));
  ld8->setFont(rufont);
  leaddress = addLineEdit(w1, "leaddress", 10, 310, 160, 20, "");
  leaddress->setFont(rufont);
  QToolTip::add(leaddress, _("Postal address"));
  QLabel *l9 = addLabel(w1, "ml9", 10, 290, 50, 20, _("Address"));
  l9->setFont(rufont);
  QLabel *ld9 = addLabel(w1, "mld9", 190, 315, 50, 20, _("Full postal address"));
  ld9->setFont(rufont);

  addTab(w1, _("User info"));

  if (is_shadow != 0) {
    QWidget *w2 = new QWidget(this, "wd_Shadow");
    w2->setFont(rufont);

    leslstchg = addLabel(w2, "leslstchg", 10, 30, 70, 20, "");
    leslstchg->setFont(rufont);
//    QToolTip::add(leslstchg, _("Last password change"));
    QLabel *l16 = addLabel(w2, "ml16", 95, 30, 50, 20, _("Last password change"));
    l16->setFont(rufont);
    lesmin = new KDateCtl(w2, "lesmin", _("Change never allowed"),
               _("Date until change allowed"),
                  user->s_lstchg+user->s_min, 10, 70);
    lesmin->setFont(rufont);
//    QToolTip::add(lesmin, _("Date until change allowed"));

    lesmax = new KDateCtl(w2, "lesmax", _("Change required after first login"),
                          _("Date before change required"),
                          user->s_lstchg+user->s_max, 10, 130);
    lesmax->setFont(rufont);
//    QToolTip::add(lesmax, _("Date before change required"));

    leswarn = new KDateCtl(w2, "leswarn", _("User will never be warned"),
                           _("Date user will be warned about\nexpiration"),
                           user->s_lstchg+user->s_warn, 10, 190);
    leswarn->setFont(rufont);
//    QToolTip::add(leswarn, _("Date user will be warned about expiration"));

    lesinact = new KDateCtl(w2, "lesinact", _("Account is active from the day of creation"),
                            _("Date before account inactive"),
                            user->s_lstchg+user->s_inact, 10, 250);
    lesinact->setFont(rufont);
//    QToolTip::add(lesinact, _("Date before account inactive"));

    lesexpire = new KDateCtl(w2, "lesexpire", _("Account never expires"),
                             _("Date when account expires"),
                             user->s_lstchg+user->s_expire, 10, 310);
    lesexpire->setFont(rufont);
//    QToolTip::add(lesexpire, _("Date when account expires"));

    addTab(w2, _("Extended"));
  }

  if (is_quota != 0) {
    QWidget *w3 = new QWidget(this, "wd_Quota");
    w3->setFont(rufont);

    leqmnt = new QComboBox(TRUE, w3, "leqmnt");
    leqmnt->setFont(rufont);
    QToolTip::add(leqmnt, _("Quota filesystem"));
    leqmnt->clear();

    struct mntent *mnt;
    FILE *fp;

    fp = setmntent(MNTTAB, "r");
    while ((mnt = getmntent(fp)) != (struct mntent *)0)
      if (hasmntopt(mnt, MNTOPT_USRQUOTA) != (char *)0)
        leqmnt->insertItem(mnt->mnt_dir);
    endmntent(fp);

    leqmnt->setGeometry(200, 20, 160, 27);
    QObject::connect(leqmnt, SIGNAL(highlighted(int)), this, SLOT(mntsel(int)));

    leqfs = addLineEdit(w3, "leqfs", 10, 80, 70, 20, "");
    leqfs->setFont(rufont);
    QToolTip::add(leqfs, _("File soft quota"));
    QLabel *l10 = addLabel(w3, "ml10", 95, 80, 50, 20, _("File soft quota"));
    l10->setFont(rufont);
    leqfh = addLineEdit(w3, "leqfh", 10, 125, 70, 20, "");
    leqfh->setFont(rufont);
    QToolTip::add(leqfh, _("File hard quota"));
    QLabel *l11 = addLabel(w3, "ml11", 95, 125, 50, 20, _("File hard quota"));
    l11->setFont(rufont);
    leqfcur = addLabel(w3, "leqfcur", 10, 185, 70, 20, "");
    leqfcur->setFont(rufont);
    QToolTip::add(leqfcur, _("File usage"));
    QLabel *l14 = addLabel(w3, "ml14", 95, 185, 50, 20, _("File usage"));
    l14->setFont(rufont);
    leqis = addLineEdit(w3, "leqis", 10, 225, 70, 20, "");
    leqis->setFont(rufont);
    QToolTip::add(leqis, _("iNode soft quota"));
    QLabel *l12 = addLabel(w3, "ml12", 95, 225, 50, 20, _("iNode soft quota"));
    l12->setFont(rufont);
    leqih = addLineEdit(w3, "leqih", 10, 270, 70, 20, "");
    leqih->setFont(rufont);
    QToolTip::add(leqih, _("iNode hard quota"));
    QLabel *l13 = addLabel(w3, "ml13", 95, 270, 50, 20, _("iNode hard quota"));
    l13->setFont(rufont);
    leqicur = addLabel(w3, "leqicur", 10, 320, 70, 20, "");
    leqicur->setFont(rufont);
    QToolTip::add(leqicur, _("iNode usage"));
    QLabel *l15 = addLabel(w3, "ml15", 95, 320, 50, 20, _("iNode usage"));
    l15->setFont(rufont);
    addTab(w3, _("Quota"));
  }

  selectuser();
  resize(450, 470);
}

int propdlg::change() {
  FILE *passwd;
  char uname[200];
  char *check;

  user->p_fname.setStr(lefname->text());
  user->p_office1.setStr(leoffice1->text());
  user->p_office2.setStr(leoffice2->text());
  user->p_address.setStr(leaddress->text());
  user->p_dir.setStr(lehome->text());
  if (leshell->currentItem() != 0)
    user->p_shell.setStr(leshell->text(leshell->currentItem()));
  else
    user->p_shell.setStr("");

#ifdef _XU_SHADOW
  if (is_shadow) {
    user->s_min = lesmin->getDate()-user->s_lstchg;
    user->s_max = lesmax->getDate()-user->s_lstchg;
    user->s_warn = leswarn->getDate()-user->s_lstchg;
    user->s_inact = lesinact->getDate()-user->s_lstchg;
    user->s_expire = lesexpire->getDate()-user->s_lstchg;
  }
#endif

/*
#ifdef _XU_QUOTA
  if (is_quota != 0) {
    int q = leqmnt->getCurrentItem();
    
    if (getValue(user->quota.at(q).fhard, leqfh->text(), _("Hard block quota must be an integer")))
      return (-1);

    if (getValue(user->quota.at(q).fsoft, leqfs->text(), _("Soft block quota must be an integer")))
      return (-1);

    if (getValue(user->quota.at(q).ihard, leqih->text(), _("Hard inode quota must be an integer")))
      return (-1);

    if (getValue(user->quota.at(q).isoft, leqis->text(), _("Soft inode quota must be an integer")))
      return (-1);
  }
#endif
*/
  return (0);
}

void propdlg::mntsel(int index) {
  chquota = index;
  selectuser();
}

void propdlg::shactivated(const char *text) { 
  int id = leshell->currentItem();
  leshell->changeItem(text, id);
}

void propdlg::save() {
  if (change())
    return;

  changed = TRUE;
}

void propdlg::selectuser() {
  char uname[100];

  leuser->setText(user->p_name);

  sprintf(uname,"%i",user->p_uid);
  leid->setText(uname);

  sprintf(uname,"%i",user->p_gid);
  legid->setText(uname);

  lefname->setText(user->p_fname);

  if (user->p_shell[0] != 0) {
    int tested = 0;
    for (int i=0; i<leshell->count(); i++)
      if (!strcmp(leshell->text(i), user->p_shell)) {
        tested = 1;
        leshell->setCurrentItem(i);
        break;
      }
    if (!tested) {
      leshell->insertItem(user->p_shell);
      leshell->setCurrentItem(leshell->count()-1);
    }
  } else
    leshell->setCurrentItem(0);

  lehome->setText(user->p_dir);
  leoffice1->setText(user->p_office1);
  leoffice2->setText(user->p_office2);
  leaddress->setText(user->p_address);
#ifdef _XU_QUOTA
  if (is_quota != 0) {
    int q = 0;
    if (chquota != -1) {
      q = chquota;
      chquota = -1;
    }
    else  
      q = leqmnt->currentItem();
    sprintf(uname,"%li",user->quota.at(q)->fsoft);
    leqfs->setText(uname);

    sprintf(uname,"%li",user->quota.at(q)->fhard);
    leqfh->setText(uname);

    sprintf(uname,"%li",user->quota.at(q)->isoft);
    leqis->setText(uname);

    sprintf(uname,"%li",user->quota.at(q)->ihard);
    leqih->setText(uname);

    sprintf(uname,"%li",user->quota.at(q)->fcur);
    leqfcur->setText(uname);

    sprintf(uname,"%li",user->quota.at(q)->icur);
    leqicur->setText(uname);
  }
#endif

#ifdef _XU_SHADOW
  if (is_shadow) {
    convertdate(uname, 0, user->s_lstchg);
    leslstchg->setText(uname);

    convertdate(uname, user->s_lstchg, user->s_min);
//    lesmin->setText(uname);

    convertdate(uname, user->s_lstchg, user->s_warn);
//    leswarn->setText(uname);

    convertdate(uname, user->s_lstchg, user->s_max);
//    lesmax->setText(uname);

    convertdate(uname, user->s_lstchg, user->s_inact);
//    lesinact->setText(uname);

    convertdate(uname, user->s_lstchg, user->s_expire);
//    lesexpire->setText(uname);
  }
#endif
}

void propdlg::setpwd() {
  pwddlg *d;

  d = new pwddlg(user, this, "pwddlg");
  d->exec();
}

void propdlg::ok() {
  save();
  accept();
}

void propdlg::cancel() {
  reject();
}
