#include <qvalidator.h>
#include <kmsgbox.h>

#include <stdio.h>
#include <stdlib.h>
#ifdef __FreeBSD__
#include <qdatetm.h>
#endif
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
       :QDialog(parent, name, FALSE,
       WStyle_Customize | WStyle_DialogBorder | WStyle_SysMenu |
       WStyle_MinMax | WType_Modal) {
  user = auser;
  olduid = user->getp_uid();
#ifdef __FreeBSD__
  QDateTime *epoch = new QDateTime();
  QDateTime *temp_time = new QDateTime();
#endif

#ifdef _KU_QUOTA
  if (aquota == NULL)
    is_quota = 0;
  quota = aquota;
  chquota = 0;
#endif

  tw = new KTabCtl(this, "tw");
  tw->setGeometry(0, 0, 450, 410);

  pbok = new QPushButton(_("Ok"), this, "pbok");
  pbok->setGeometry(200, 425, 100, 30);
  pbok->setDefault(TRUE);
  QObject::connect(pbok, SIGNAL(pressed()), this, SLOT(ok()));
  pbcancel = new QPushButton(_("Cancel"), this, "pbcancel");
  pbcancel->setGeometry(320, 425, 100, 30);
  QObject::connect(pbcancel, SIGNAL(pressed()), this, SLOT(cancel()));

  w1 = new QWidget(this, "wd_Password");

  leuser = addLabel(w1, "leuser", 10, 27, 150, 27, user->getp_name());
  QToolTip::add(leuser, _("User name"));
  l1 = addLabel(w1, "ml1", 10, 10, 50, 20, _("User login"));
  leid = addLineEdit(w1, "leid", 200, 30, 70, 22, "");
  QObject::connect(leid, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leid, _("User identificator"));
  l2 = addLabel(w1, "ml2", 200, 10, 50, 20, _("User id"));

//  legid = addLineEdit(w1, "legid", 200, 85, 70, 22, "");
//  QObject::connect(legid, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
//  QToolTip::add(legid, _("Group identificator"));
//  ld3 = addLabel(w1, "mld3", 200, 60, 50, 20, _("Primary group id"));

  pbsetpwd = new QPushButton(w1, "pbsetpwd");
  pbsetpwd->setGeometry(260, 160, 120, 30);
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

#ifdef __FreeBSD__
  // FreeBSD appears to use the comma separated fields in the GECOS entry
  // differently than Linux.
  leoffice = addLineEdit(w1, "leoffice", 10, 220, 160, 22, "");
  QObject::connect(leoffice, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leoffice, _("Office"));
  l7 = addLabel(w1, "ml7", 10, 200, 50, 20, _("Office"));

  ld7 = addLabel(w1, "mld7", 190, 225, 50, 20, _("Office description"));

  leophone = addLineEdit(w1, "leophone", 10, 265, 160, 22, "");
  QObject::connect(leophone, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leophone, _("Office phone"));
  l8 = addLabel(w1, "ml8", 10, 245, 50, 20, _("Office phone"));

  ld8 = addLabel(w1, "mld8", 190, 270, 50, 20, _("Office phone number"));

  lehphone = addLineEdit(w1, "lehphone", 10, 310, 160, 22, "");
  QObject::connect(lehphone, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(lehphone, _("Home phone"));
  l9 = addLabel(w1, "ml9", 10, 290, 50, 20, _("Home phone"));

  ld9 = addLabel(w1, "mld9", 190, 315, 50, 20, _("Home phone number"));
  tw->addTab(w1, _("User info"));

  // put the new fields on a separate tab page just to avoid rewriting all
  // the positioning code.
  w5 = new QWidget(this, "wd_Extended");

  leclass = addLineEdit(w5, "leclass", 10, 80, 160, 22, "");
  QObject::connect(leclass, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leclass, _("Login class"));
  l19 = addLabel(w5, "ml19", 10, 60, 50, 20, _("Login class"));

  ld19 = addLabel(w5, "mld19", 190, 85, 50, 20, _("Login resource class"));

  // KDateCtl works in `days since the epoch' so we convert from seconds.
  // Set the `base' to 1 so getDate returns 0 (base - 1) if the `never expires'
  // box is checked.
  epoch->setTime_t(0);
  temp_time->setTime_t(user->getp_change());
  lechange = new KDateCtl(w5, "lechange", _("Password never expires"),
                         _("Date when password expires"),
                         epoch->daysTo(*temp_time), 1,
                         10, 150);

  QObject::connect(lechange, SIGNAL(textChanged()), this, SLOT(changed()));

  temp_time->setTime_t(user->getp_expire());

  leexpire = new KDateCtl(w5, "leexpire", _("Account never expires"),
                         _("Date when account expires"),
                         epoch->daysTo(*temp_time), 1,
                         10, 250);

  QObject::connect(leexpire, SIGNAL(textChanged()), this, SLOT(changed()));

  delete epoch;
  delete temp_time;
  tw->addTab(w5, _("Extended"));

#else

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

  tw->addTab(w1, _("User info"));
#endif

#ifdef _KU_SHADOW
  if (is_shadow != 0) {
    w2 = new QWidget(this, "wd_Shadow");

    leslstchg = addLabel(w2, "leslstchg", 10, 30, 70, 20, "");
//    QToolTip::add(leslstchg, _("Last password change"));
    l16 = addLabel(w2, "ml16", 95, 30, 50, 20, _("Last password change"));
    lesmin = new KDateCtl(w2, "lesmin", _("Change never allowed"),
               _("Date until change allowed"),
                  user->gets_lstchg()+user->gets_min(), 
                  user->gets_lstchg(), 10, 70);
    QObject::connect(lesmin, SIGNAL(textChanged()), this, SLOT(changed()));
//    QToolTip::add(lesmin, _("Date until change allowed"));

    lesmax = new KDateCtl(w2, "lesmax", _("Change does not required"),
                          _("Date before change required"),
                          user->gets_lstchg()+user->gets_max(),
                          user->gets_lstchg(), 10, 130);
    QObject::connect(lesmax, SIGNAL(textChanged()), this, SLOT(changed()));
//    QToolTip::add(lesmax, _("Date before change required"));

    leswarn = new KDateCtl(w2, "leswarn", _("User will never be warned"),
                           _("Date user will be warned about\nexpiration"),
                           user->gets_lstchg()+user->gets_warn(),
                           user->gets_lstchg(), 10, 190);
    QObject::connect(leswarn, SIGNAL(textChanged()), this, SLOT(changed()));
//    QToolTip::add(leswarn, _("Date user will be warned about expiration"));

    lesinact = new KDateCtl(w2, "lesinact", _("Account is active from the day of creation"),
                            _("Date before account inactive"),
                            user->gets_lstchg()+user->gets_inact(),
                            user->gets_lstchg(), 10, 250);
    QObject::connect(lesinact, SIGNAL(textChanged()), this, SLOT(changed()));
//    QToolTip::add(lesinact, _("Date before account inactive"));

    lesexpire = new KDateCtl(w2, "lesexpire", _("Account never expires"),
                             _("Date when account expires"),
                             user->gets_lstchg()+user->gets_expire(),
                             user->gets_lstchg(), 10, 310);
    QObject::connect(lesexpire, SIGNAL(textChanged()), this, SLOT(changed()));
//    QToolTip::add(lesexpire, _("Date when account expires"));

    tw->addTab(w2, _("Extended"));
  }
#endif

#ifdef _KU_QUOTA
  if (is_quota != 0) {
    w3 = new QWidget(this, "wd_Quota");

    leqmnt = new QComboBox(TRUE, w3, "leqmnt");
    QToolTip::add(leqmnt, _("Quota filesystem"));
    leqmnt->clear();

    for (uint i = 0; i<mounts->getMountsNumber(); i++)
       leqmnt->insertItem(mounts->getMount(i)->getdir());

    leqmnt->setGeometry(250, 20, 160, 27);
    QObject::connect(leqmnt, SIGNAL(highlighted(int)), this, SLOT(mntsel(int)));

    l10a = addLabel(w3, "ml10a", 20, 28, 230, 20, _("Quota filesystem"));
    l10a->setAlignment(AlignVCenter | AlignRight);

    leqfs = addLineEdit(w3, "leqfs", 10, 80, 70, 22, "");
    leqfs->setValidator(new QIntValidator(w3, "vaqfs"));
    QObject::connect(leqfs, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqfs, _("File soft quota"));
    l10 = addLabel(w3, "ml10", 95, 85, 50, 20, _("File soft quota"));

    leqfh = addLineEdit(w3, "leqfh", 10, 120, 70, 22, "");
    leqfh->setValidator(new QIntValidator(w3, "vaqfh"));
    QObject::connect(leqfh, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqfh, _("File hard quota"));
    l11 = addLabel(w3, "ml11", 95, 125, 50, 20, _("File hard quota"));

    leqfcur = addLabel(w3, "leqfcur", 13, 160, 70, 20, "");
    QToolTip::add(leqfcur, _("File usage"));
    l14 = addLabel(w3, "ml14", 95, 160, 50, 20, _("File usage"));

    leqft = addLineEdit(w3, "leqft", 10, 190, 70, 22, "");
    leqft->setValidator(new QIntValidator(w3, "vaqft"));
    QObject::connect(leqft, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqft, _("File time limit"));
    l18 = addLabel(w3, "ml18", 95, 195, 50, 20, _("File time limit"));

    leqis = addLineEdit(w3, "leqis", 10, 235, 70, 22, "");
    leqis->setValidator(new QIntValidator(w3, "vaqis"));
    QObject::connect(leqis, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqis, _("iNode soft quota"));
    l12 = addLabel(w3, "ml12", 95, 240, 50, 20, _("iNode soft quota"));

    leqih = addLineEdit(w3, "leqih", 10, 275, 70, 22, "");
    leqih->setValidator(new QIntValidator(w3, "vaqih"));
    QObject::connect(leqih, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqih, _("iNode hard quota"));
    l13 = addLabel(w3, "ml13", 95, 280, 50, 20, _("iNode hard quota"));

    leqicur = addLabel(w3, "leqicur", 13, 315, 70, 20, "");
    QToolTip::add(leqicur, _("iNode usage"));
    l15 = addLabel(w3, "ml15", 95, 315, 50, 20, _("iNode usage"));

    leqit = addLineEdit(w3, "leqit", 10, 345, 70, 22, "");
    leqit->setValidator(new QIntValidator(w3, "vaqit"));
    QObject::connect(leqit, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqit, _("iNode time limit"));
    l17 = addLabel(w3, "ml17", 95, 350, 50, 20, _("iNode time limit"));

    tw->addTab(w3, _("Quota"));
  }
#endif

  w4 = new QWidget(this, "wd_Groups");

  m_Other = new QListBox( w4, "m_Other" );
  m_Other->setGeometry( 15, 80, 160, 130 );
  m_Other->setFrameStyle( 51 );
  m_Other->setLineWidth( 2 );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel(w4, "Label_1");
  tmpQLabel->setGeometry(20, 60, 100, 20);
  tmpQLabel->setText(_("Users"));
  tmpQLabel->setAlignment(289);
  tmpQLabel->setMargin(-1);

  m_Group = new QListBox(w4, "m_Group");
  m_Group->setGeometry(250, 80, 160, 130);
  m_Group->setFrameStyle(51);
  m_Group->setLineWidth(2);

  pbadd = new QPushButton(w4, "pbadd");
  pbadd->setGeometry(195, 100, 40, 30);
  connect( pbadd, SIGNAL(clicked()), SLOT(add()) );
  pbadd->setText("->");
  pbadd->setAutoRepeat( FALSE );
  pbadd->setAutoResize( FALSE );

  pbdel = new QPushButton( w4, "pbdel" );
  pbdel->setGeometry( 195, 150, 40, 30 );
  connect( pbdel, SIGNAL(clicked()), SLOT(del()) );
  pbdel->setText("<-");
  pbdel->setAutoRepeat( FALSE );
  pbdel->setAutoResize( FALSE );

  tmpQLabel = new QLabel(w4, "Label_2");
  tmpQLabel->setGeometry(300, 60, 100, 20);
  tmpQLabel->setText(_("Groups"));
  tmpQLabel->setAlignment(289);
  tmpQLabel->setMargin(-1);

  cbpgrp = new QComboBox(FALSE, w4, "cbpgrp");
  cbpgrp->setGeometry(250, 30, 160, 30);
  QObject::connect(cbpgrp, SIGNAL(activated(const char *)), this, SLOT(setpgroup(const char *)));

  tmpQLabel = new QLabel(w4, "Label_3");
  tmpQLabel->setGeometry(100, 35, 140, 20);
  tmpQLabel->setText(_("Primary group"));
  tmpQLabel->setAlignment(AlignVCenter | AlignRight);
  tmpQLabel->setMargin(-1);

  tw->addTab(w4, _("Groups"));

  loadgroups();
  selectuser();
  setFixedSize(450, 470);

  ischanged = FALSE;
#ifdef _KU_QUOTA
  isqchanged = FALSE;
#endif

  setCaption(_("Edit User"));
}

propdlg::~propdlg() {
  delete w1;
  
#ifdef _KU_SHADOW
  if (is_shadow != 0)
    delete w2;
#endif
  
#ifdef _KU_QUOTA
  if (is_quota != 0)
    delete w3;
#endif

  delete w4;
}

void propdlg::loadgroups() {
  uint i;

  for (i = 0; i<groups->getGroupsNumber(); i++) {
    if (groups->getGroup(i)->lookup_user(user->getp_name())!=0)
      m_Group->insertItem(groups->getGroup(i)->getname());
    else
      m_Other->insertItem(groups->getGroup(i)->getname());

    cbpgrp->insertItem(groups->getGroup(i)->getname());
    if (groups->getGroup(i)->getgid() == user->getp_gid())
      cbpgrp->setCurrentItem(i);
  }

  if (m_Other->count() != 0)
    m_Other->setCurrentItem(0);
  if (m_Group->count() != 0)
    m_Group->setCurrentItem(0);
}

void propdlg::setpgroup(const char *) {
  ischanged = TRUE;
}

void propdlg::changed() {
  ischanged = TRUE;
}

void propdlg::add() {
  int cur = m_Other->currentItem();

  if (cur == -1)
    return;

  m_Group->insertItem(m_Other->text(cur));
  m_Other->removeItem(cur);

  if (((uint)cur) == m_Other->count())
    m_Other->setCurrentItem(cur-1);
  else
    m_Other->setCurrentItem(cur);

  m_Group->setCurrentItem(m_Group->count()-1);
  m_Group->centerCurrentItem();

  ischanged = TRUE;
}

void propdlg::del() {
  int cur = m_Group->currentItem();

  if (cur == -1)
    return;

  m_Other->insertItem(m_Group->text(cur));
  m_Group->removeItem(cur);

  if (((uint)cur) == m_Group->count())
    m_Group->setCurrentItem(cur-1);
  else
    m_Group->setCurrentItem(cur);
  
  m_Other->setCurrentItem(m_Other->count()-1);
  m_Other->centerCurrentItem();

  ischanged = TRUE;
}

void propdlg::charchanged(const char *) {
  ischanged = TRUE;
}

void propdlg::save() {
  QString tmp;
  uint newuid;
  tmp.setStr(leid->text());
  newuid = tmp.toInt();
#ifdef __FreeBSD__
  QDateTime *epoch = new QDateTime();
  QDateTime *temp_time = new QDateTime();
#endif
  
  user->setp_uid(newuid);

  user->setp_fname(lefname->text());
#ifdef __FreeBSD__
  user->setp_office(leoffice->text());
  user->setp_ophone(leophone->text());
  user->setp_hphone(lehphone->text());
  user->setp_class(leclass->text());

  // getDate() returns days since the epoch if a date is specified
  // which we convert to seconds since the epoch.
  // getDate() returns 0 if the `never expires' box is checked which
  // ends up being converted to 0 seconds which is what we need.
  epoch->setTime_t(0);

  temp_time->setTime_t(0);
  *temp_time = temp_time->addDays(lechange->getDate());
  user->setp_change(epoch->secsTo(*temp_time));

  temp_time->setTime_t(0);
  *temp_time = temp_time->addDays(leexpire->getDate());
  user->setp_expire(epoch->secsTo(*temp_time));

  delete epoch;
  delete temp_time;
#else
  user->setp_office1(leoffice1->text());
  user->setp_office2(leoffice2->text());
  user->setp_address(leaddress->text());
#endif

  user->setp_dir(lehome->text());
  if (leshell->currentItem() != 0)
    user->setp_shell(leshell->text(leshell->currentItem()));
  else
    user->setp_shell("");

#ifdef _KU_SHADOW
  if (is_shadow) {
    user->sets_min(lesmin->getDate()-user->gets_lstchg());
    user->sets_max(lesmax->getDate()-user->gets_lstchg());
    user->sets_warn(leswarn->getDate()-user->gets_lstchg());
    user->sets_inact(lesinact->getDate()-user->gets_lstchg());
    user->sets_expire(lesexpire->getDate()-user->gets_lstchg());
    user->sets_lstchg(today());
  }
#endif
}

void propdlg::saveg() {
  uint i;

  for (i=0;i<m_Group->count();i++)
    if (groups->group_lookup(m_Group->text(i))->lookup_user(user->getp_name()) == NULL)
      groups->group_lookup(m_Group->text(i))->addUser(user->getp_name());
  
  for (i=0;i<m_Other->count();i++)
    if (groups->group_lookup(m_Other->text(i))->lookup_user(user->getp_name()) != NULL)
      groups->group_lookup(m_Other->text(i))->removeUser(user->getp_name());

  user->setp_gid(groups->group_lookup(cbpgrp->text(cbpgrp->currentItem()))->getgid());
}

#ifdef _KU_QUOTA
void propdlg::mntsel(int index) {
  saveq();

  chquota = index;
  selectuser();
}

void propdlg::qcharchanged(const char *) {
  isqchanged = TRUE;
}

void propdlg::saveq() {
  QuotaMnt *tmpq = quota->getQuotaMnt(chquota);

  tmpq->setfhard(atol(leqfh->text()));
  tmpq->setfsoft(atol(leqfs->text()));
  tmpq->setftime(atol(leqft->text()));
  tmpq->setihard(atol(leqih->text()));
  tmpq->setisoft(atol(leqis->text()));
  tmpq->setitime(atol(leqit->text()));
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
    saveg();
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

  leuser->setText(user->getp_name());

  sprintf(uname,"%i",user->getp_uid());
  leid->setText(uname);

//  sprintf(uname,"%i",user->getp_gid());
//  legid->setText(uname);

  lefname->setText(user->getp_fname());

  if (user->getp_shell().isEmpty() != TRUE) {
    int tested = 0;
    for (int i=0; i<leshell->count(); i++)
      if (!strcmp(leshell->text(i), user->getp_shell())) {
        tested = 1;
        leshell->setCurrentItem(i);
        break;
      }
    if (!tested) {
      leshell->insertItem(user->getp_shell());
      leshell->setCurrentItem(leshell->count()-1);
    }
  } else
    leshell->setCurrentItem(0);

  lehome->setText(user->getp_dir());
#ifdef __FreeBSD__
  leoffice->setText(user->getp_office());
  leophone->setText(user->getp_ophone());
  lehphone->setText(user->getp_hphone());
  leclass->setText(user->getp_class());
  // the date fields get set when the dialogue is built
#else
  leoffice1->setText(user->getp_office1());
  leoffice2->setText(user->getp_office2());
  leaddress->setText(user->getp_address());
#endif

#ifdef _KU_QUOTA
  if (is_quota != 0) {
    int q = 0;
    if (chquota != -1)
      q = chquota;

    sprintf(uname,"%li",quota->getQuotaMnt(q)->getfsoft());
    leqfs->setText(uname);

    sprintf(uname,"%li",quota->getQuotaMnt(q)->getfhard());
    leqfh->setText(uname);

    sprintf(uname,"%li",quota->getQuotaMnt(q)->getisoft());
    leqis->setText(uname);

    sprintf(uname,"%li",quota->getQuotaMnt(q)->getihard());
    leqih->setText(uname);

    sprintf(uname,"%li",quota->getQuotaMnt(q)->getfcur());
    leqfcur->setText(uname);

    sprintf(uname,"%li",quota->getQuotaMnt(q)->geticur());
    leqicur->setText(uname);

    sprintf(uname,"%li",quota->getQuotaMnt(q)->getftime());
    leqft->setText(uname);

    sprintf(uname,"%li",quota->getQuotaMnt(q)->getitime());
    leqit->setText(uname);
  }
#endif
}

void propdlg::setpwd() {
  pwddlg *pd;

  pd = new pwddlg(user, this, "pwddlg");
  pd->exec();
  delete pd;
}

void propdlg::ok() {
  QString tmp;
  uint newuid;
  tmp.setStr(leid->text());
  newuid = tmp.toInt();

printf("propdlg::ok\n");
  
  if (olduid != newuid)
    if (users->user_lookup(newuid) != NULL) {
      tmp.sprintf(_("User with UID %u already exists"), newuid);
      KMsgBox::message(0, _("Message"), tmp, KMsgBox::STOP, _("Ok"));
      return;
    }
    
  if (check() == TRUE)
    accept();
  else
    reject();
}

void propdlg::cancel() {
  reject();
}
