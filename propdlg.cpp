#include <qvalidator.h>

#include "globals.h"
#include "propdlg.h"
#include "propdlg.moc"
#include "pwddlg.h"
#include "maindlg.h"
#include "misc.h"
#include "kdatectl.h"


#ifdef _KU_QUOTA
propdlg::propdlg(KUser *auser, Quota *aquota, QWidget *parent, const char *name, int)
#else
propdlg::propdlg(KUser *auser, QWidget *parent, const char *name, int)
#endif
       :QTabDialog(parent, name, FALSE,
       WStyle_Customize | WStyle_DialogBorder | WStyle_SysMenu |
       WStyle_MinMax | WType_Modal) {
  user = auser;
#ifdef _KU_QUOTA
  if (aquota == NULL)
    is_quota = 0;
  quota = aquota;
  chquota = 0;
#endif

  QObject::connect(this, SIGNAL(applyButtonPressed()), this, SLOT(ok()));
  setCancelButton();
  QObject::connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(cancel()));

  w1 = new QWidget(this, "wd_Password");

  leuser = addLabel(w1, "leuser", 10, 27, 150, 27, user->p_name);
  QToolTip::add(leuser, _("User name"));
  l1 = addLabel(w1, "ml1", 10, 10, 50, 20, _("User login"));
  leid = addLineEdit(w1, "leid", 200, 30, 70, 22, "");
  QObject::connect(leid, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leid, _("User identificator"));
  l2 = addLabel(w1, "ml2", 200, 10, 50, 20, _("User id"));
  legid = addLineEdit(w1, "legid", 200, 85, 70, 22, "");
  QObject::connect(legid, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(legid, _("Group identificator"));
  ld3 = addLabel(w1, "mld3", 200, 60, 50, 20, _("Primary group id"));

  pbsetpwd = new QPushButton(w1, "pbsetpwd");
  pbsetpwd->setGeometry(240, 180, 80, 20);
  pbsetpwd->setText(_("Set password"));
  QObject::connect(pbsetpwd, SIGNAL(clicked()), this, SLOT(setpwd()));

  lefname = addLineEdit(w1, "lefname", 10, 80, 160, 22, "");
  QObject::connect(lefname, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(lefname, _("Full name"));
  l4 = addLabel(w1, "ml4", 10, 60, 50, 20, _("Full name"));

  leshell = new QComboBox(TRUE, w1, "leshell");
  QToolTip::add(leshell, _("Login shell"));
  leshell->clear();
  leshell->insertItem(_("<Empty>"));

  FILE *f = fopen("/etc/shells","r");
  if (f) {
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
  }

  leshell->setGeometry(10, 122, 160, 27);
  QObject::connect(leshell, SIGNAL(activated(const char *)), this, SLOT(shactivated(const char *)));

  l5 = addLabel(w1, "ml5", 10, 105, 50, 20, _("Login shell"));

  lehome = addLineEdit(w1, "lehome", 10, 175, 160, 22, "");
  QObject::connect(lehome, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));

  QToolTip::add(lehome, _("Home directory"));
  l6 = addLabel(w1, "ml6", 10, 155, 50, 20, _("Home directory"));

  leoffice1 = addLineEdit(w1, "leoffice1", 10, 220, 160, 22, "");
  QObject::connect(leoffice1, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leoffice1, _("The first office"));
  l7 = addLabel(w1, "ml7", 10, 200, 50, 20, _("Office1"));

  ld7 = addLabel(w1, "mld7", 190, 225, 50, 20, _("First office description"));

  leoffice2 = addLineEdit(w1, "leoffice2", 10, 265, 160, 22, "");
  QObject::connect(leoffice2, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leoffice2, _("The second office"));
  l8 = addLabel(w1, "ml8", 10, 245, 50, 20, _("Office2"));

  ld8 = addLabel(w1, "mld8", 190, 270, 50, 20, _("Second office description"));
  leaddress = addLineEdit(w1, "leaddress", 10, 310, 160, 22, "");
  QObject::connect(leaddress, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leaddress, _("Postal address"));
  l9 = addLabel(w1, "ml9", 10, 290, 50, 20, _("Address"));
  ld9 = addLabel(w1, "mld9", 190, 315, 50, 20, _("Full postal address"));

  addTab(w1, _("User info"));

#ifdef _KU_SHADOW
  if (is_shadow != 0) {
    w2 = new QWidget(this, "wd_Shadow");

    leslstchg = addLabel(w2, "leslstchg", 10, 30, 70, 20, "");
//    QToolTip::add(leslstchg, _("Last password change"));
    l16 = addLabel(w2, "ml16", 95, 30, 50, 20, _("Last password change"));
    lesmin = new KDateCtl(w2, "lesmin", _("Change never allowed"),
               _("Date until change allowed"),
                  user->s_lstchg+user->s_min, 10, 70);
    QObject::connect(lesmin, SIGNAL(textChanged()), this, SLOT(changed()));
//    QToolTip::add(lesmin, _("Date until change allowed"));

    lesmax = new KDateCtl(w2, "lesmax", _("Change required after first login"),
                          _("Date before change required"),
                          user->s_lstchg+user->s_max, 10, 130);
    QObject::connect(lesmax, SIGNAL(textChanged()), this, SLOT(changed()));
//    QToolTip::add(lesmax, _("Date before change required"));

    leswarn = new KDateCtl(w2, "leswarn", _("User will never be warned"),
                           _("Date user will be warned about\nexpiration"),
                           user->s_lstchg+user->s_warn, 10, 190);
    QObject::connect(leswarn, SIGNAL(textChanged()), this, SLOT(changed()));
//    QToolTip::add(leswarn, _("Date user will be warned about expiration"));

    lesinact = new KDateCtl(w2, "lesinact", _("Account is active from the day of creation"),
                            _("Date before account inactive"),
                            user->s_lstchg+user->s_inact, 10, 250);
    QObject::connect(lesinact, SIGNAL(textChanged()), this, SLOT(changed()));
//    QToolTip::add(lesinact, _("Date before account inactive"));

    lesexpire = new KDateCtl(w2, "lesexpire", _("Account never expires"),
                             _("Date when account expires"),
                             user->s_lstchg+user->s_expire, 10, 310);
    QObject::connect(lesexpire, SIGNAL(textChanged()), this, SLOT(changed()));
//    QToolTip::add(lesexpire, _("Date when account expires"));

    addTab(w2, _("Extended"));
  }
#endif

#ifdef _KU_QUOTA
  if (is_quota != 0) {
    w3 = new QWidget(this, "wd_Quota");

    leqmnt = new QComboBox(TRUE, w3, "leqmnt");
    QToolTip::add(leqmnt, _("Quota filesystem"));
    leqmnt->clear();

    for (uint i = 0; i<mounts->getMountsNumber(); i++)
       leqmnt->insertItem(mounts->getMount(i)->dir);

    leqmnt->setGeometry(200, 20, 160, 27);
    QObject::connect(leqmnt, SIGNAL(highlighted(int)), this, SLOT(mntsel(int)));

    l10a = addLabel(w3, "ml10a", 20, 28, 50, 20, _("Quota filesystem"));

    leqfs = addLineEdit(w3, "leqfs", 10, 80, 70, 22, "");
    leqfs->setValidator(new QIntValidator(w3, "vaqfs"));
    QObject::connect(leqfs, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqfs, _("File soft quota"));
    l10 = addLabel(w3, "ml10", 95, 80, 50, 20, _("File soft quota"));

    leqfh = addLineEdit(w3, "leqfh", 10, 125, 70, 22, "");
    leqfh->setValidator(new QIntValidator(w3, "vaqfh"));
    QObject::connect(leqfh, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqfh, _("File hard quota"));
    l11 = addLabel(w3, "ml11", 95, 125, 50, 20, _("File hard quota"));
    leqfcur = addLabel(w3, "leqfcur", 10, 185, 70, 20, "");
    QToolTip::add(leqfcur, _("File usage"));
    l14 = addLabel(w3, "ml14", 95, 185, 50, 20, _("File usage"));

    leqis = addLineEdit(w3, "leqis", 10, 225, 70, 22, "");
    leqis->setValidator(new QIntValidator(w3, "vaqis"));
    QObject::connect(leqis, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqis, _("iNode soft quota"));
    l12 = addLabel(w3, "ml12", 95, 225, 50, 20, _("iNode soft quota"));

    leqih = addLineEdit(w3, "leqih", 10, 270, 70, 22, "");
    leqih->setValidator(new QIntValidator(w3, "vaqih"));
    QObject::connect(leqih, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqih, _("iNode hard quota"));
    l13 = addLabel(w3, "ml13", 95, 270, 50, 20, _("iNode hard quota"));
    leqicur = addLabel(w3, "leqicur", 10, 320, 70, 20, "");
    QToolTip::add(leqicur, _("iNode usage"));
    l15 = addLabel(w3, "ml15", 95, 320, 50, 20, _("iNode usage"));
    addTab(w3, _("Quota"));
  }
#endif

  selectuser();
  resize(450, 470);

  ischanged = FALSE;
#ifdef _KU_QUOTA
  isqchanged = FALSE;
#endif
}

propdlg::~propdlg() {
}

void propdlg::changed() {
  ischanged = TRUE;
}

void propdlg::charchanged(const char *) {
  ischanged = TRUE;
}

void propdlg::save() {
  user->p_fname.setStr(lefname->text());
  user->p_office1.setStr(leoffice1->text());
  user->p_office2.setStr(leoffice2->text());
  user->p_address.setStr(leaddress->text());
  user->p_dir.setStr(lehome->text());
  if (leshell->currentItem() != 0)
    user->p_shell.setStr(leshell->text(leshell->currentItem()));
  else
    user->p_shell.setStr("");

#ifdef _KU_SHADOW
  if (is_shadow) {
    user->s_min = lesmin->getDate()-user->s_lstchg;
    user->s_max = lesmax->getDate()-user->s_lstchg;
    user->s_warn = leswarn->getDate()-user->s_lstchg;
    user->s_inact = lesinact->getDate()-user->s_lstchg;
    user->s_expire = lesexpire->getDate()-user->s_lstchg;
  }
#endif
}

#ifdef _KU_QUOTA
void propdlg::mntsel(int index) {
  QuotaMnt *tmpq = quota->getQuotaMnt(chquota);

  tmpq->fhard = atol(leqfh->text());
  tmpq->fsoft = atol(leqfs->text());
  tmpq->ihard = atol(leqih->text());
  tmpq->isoft = atol(leqis->text());

  chquota = index;
  selectuser();
}

void propdlg::qcharchanged(const char *) {
  isqchanged = TRUE;
}

void propdlg::saveq() {
  QuotaMnt *tmpq = quota->getQuotaMnt(chquota);

  tmpq->fhard = atol(leqfh->text());
  tmpq->fsoft = atol(leqfs->text());
  tmpq->ihard = atol(leqih->text());
  tmpq->isoft = atol(leqis->text());
}
#else
void propdlg::qcharchanged(const char *) {
}
void propdlg::mntsel(int) {
}
#endif

void propdlg::shactivated(const char *text) { 
  int id = leshell->currentItem();
  leshell->changeItem(text, id);
  changed();
}

bool propdlg::check() {
  bool ret = FALSE;

  if (ischanged == TRUE) {
    save();
    ret = TRUE;
  }

#ifdef _KU_QUOTA
  if (isqchanged == TRUE) {
    saveq();
    ret = TRUE;
  }
#endif

  return (ret);
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
#ifdef _KU_QUOTA
  if (is_quota != 0) {
    int q = 0;
    if (chquota != -1)
      q = chquota;

    sprintf(uname,"%li",quota->getQuotaMnt(q)->fsoft);
    leqfs->setText(uname);

    sprintf(uname,"%li",quota->getQuotaMnt(q)->fhard);
    leqfh->setText(uname);

    sprintf(uname,"%li",quota->getQuotaMnt(q)->isoft);
    leqis->setText(uname);

    sprintf(uname,"%li",quota->getQuotaMnt(q)->ihard);
    leqih->setText(uname);

    sprintf(uname,"%li",quota->getQuotaMnt(q)->fcur);
    leqfcur->setText(uname);

    sprintf(uname,"%li",quota->getQuotaMnt(q)->icur);
    leqicur->setText(uname);
  }
#endif
}

void propdlg::setpwd() {
  pwddlg *d;

  d = new pwddlg(user, this, "pwddlg");
  d->exec();
}

void propdlg::ok() {
  if (check() == TRUE)
    accept();
  else
    reject();
}

void propdlg::cancel() {
  reject();
}
