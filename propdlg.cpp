// $Id$

#include "globals.h"

#include <stdio.h>
#include <stdlib.h>

#include <qwhatsthis.h>
#include <qlayout.h>
#include <qgroupbox.h>

#include <kseparator.h>
#include <kglobal.h>

#include <qvalidator.h>


#include "propdlg.h"
#include "pwddlg.h"
#include "kglobal_.h"
#include "misc.h"

#ifdef EXTENDED_GECOS_BSD
#include <qdatetm.h>
#endif

static void addRow(QWidget *parent, QGridLayout *layout, int row, QWidget *widget, 
                   const QString &label, const QString &what, bool two_column=true)
{
   QLabel *lab = new QLabel(widget, label, parent);
   lab->setMinimumSize(lab->sizeHint());
   widget->setMinimumSize(widget->sizeHint());
   layout->addWidget(lab, row, 0);
   if (!what.isEmpty())
   {
      QWhatsThis::add(lab, what);
      QWhatsThis::add(widget, what);
   }
   if (two_column)
      layout->addMultiCellWidget(widget, row, row, 1, 2);
   else
      layout->addWidget(widget, row, 1);
}

KDateWidget *propdlg::addDateGroup(QWidget *parent, QGridLayout *layout, int row, 
	const QString &title, int days)
{
    KDateWidget *date;

    QLabel *label = new QLabel(title, parent);
    layout->addWidget(label, row, 0);

    bool never = false;
    if (days == -1)
    {
       never = true;
       days = 0;
    }
    date = new KDateWidget(QDate(1970,1,1).addDays(days), parent);
    layout->addMultiCellWidget(date, row, row, 1, 2);
    
    QCheckBox *date_disabled = new QCheckBox(parent);
    date_disabled->setText(i18n("Never"));
    layout->addWidget(date_disabled, row, 3);

    QObject::connect(date, SIGNAL(changed(QDate)), this, SLOT(changed()));
    QObject::connect(date_disabled, SIGNAL(toggled(bool)), this, SLOT(changed()));
    QObject::connect(date_disabled, SIGNAL(toggled(bool)), date, SLOT(setDisabled(bool)));
    if (never)
       date_disabled->setChecked(true);
    return date;
}

KIntSpinBox *propdlg::addDaysGroup(QWidget *parent, QGridLayout *layout, int row, 
	const QString &title, const QString &title2, int nr_of_days, bool never)
{
    KIntSpinBox *days;

    QLabel *label = new QLabel(title, parent);
    layout->addWidget(label, row, 0, AlignRight);

    days = new KIntSpinBox(parent);
    days->setSuffix(i18n(" days"));
    days->setMaxValue(32767);
    if (never)
    {
      days->setMinValue(-1);
      days->setSpecialValueText(i18n("Never"));
    }
    else
    {
      days->setMinValue(0);
    }
    days->setValue(nr_of_days);
    layout->addWidget(days, row, 1);
    
    label = new QLabel(title2, parent);
    layout->addMultiCellWidget(label, row, row, 2, 3);

//    layout->addMultiCellWidget(group, row,  row, 0, 1);
    QObject::connect(days, SIGNAL(valueChanged(int)), this, SLOT(changed()));
    return days;
}

#ifdef _KU_QUOTA
propdlg::propdlg(KUser *AUser, Quota *AQuota, QWidget *parent, const char *name, int)
#else
propdlg::propdlg(KUser *AUser, QWidget *parent, const char *name, int)
#endif
 : KDialogBase(Tabbed, i18n("User Properties"), 
	Ok | Cancel, Ok, parent, name, true),
 user(AUser)
#ifdef _KU_QUOTA
 , quota(AQuota) 
#endif
#ifdef EXTENDED_GECOS_BSD
 , leexpire(0L)
#endif

{
  QString whatstr;
#ifdef EXTENDED_GECOS_BSD
  QDateTime epoch ;
  QDateTime temp_time ;
#endif

#ifdef _KU_QUOTA
  if (&AQuota == NULL)
    is_quota = 0;
  else
    quota = AQuota;
  chquota = 0;
#endif


  // Tab 1: User Info
  {
    QFrame *frame = addPage(i18n("User Info"));
    QGridLayout *layout = new QGridLayout(frame, 20, 3, marginHint(), spacingHint());
    int row = 0;

    frontpage = frame;
    frontlayout = layout;

    leuser = new QLabel(frame);
//    whatstr = i18n("WHAT IS THIS: User login");
    addRow(frame, layout, row++, leuser, i18n("User login:"), whatstr, false);

    leid = new QLineEdit(frame);
//    whatstr = i18n("WHAT IS THIS: User Id");
    addRow(frame, layout, row++, leid, i18n("User ID:"), whatstr);

    pbsetpwd = new QPushButton(i18n("Set Password..."), frame);
    layout->addWidget(pbsetpwd, 0, 2);
    QObject::connect(pbsetpwd, SIGNAL(clicked()), this, SLOT(setpwd()));

    lefname = new QLineEdit(frame);
//    whatstr = i18n("WHAT IS THIS: Full Name");
    addRow(frame, layout, row++, lefname, i18n("Full name:"), whatstr);
    QObject::connect(lefname, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));

    leshell = new QComboBox(true, frame);
    leshell->clear();
    leshell->insertItem(i18n("<Empty>"));

    QStringList shells;

    FILE *f = fopen(SHELL_FILE,"r");
    if (f) {
      while (!feof(f)) {
        char s[200];

        fgets(s, 200, f);
        if (feof(f))
          break;

        s[strlen(s)-1]=0;
        if ((s[0])&&(s[0]!='#'))
          shells.append(s);
      }
      fclose(f);
    }
    shells.sort();
    leshell->insertStringList(shells);
    QObject::connect(leshell, SIGNAL(activated(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Login Shell");
    addRow(frame, layout, row++, leshell, i18n("Login shell:"), whatstr);

    lehome = new QLineEdit(frame);
    QObject::connect(lehome, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Home Directory");
    addRow(frame, layout, row++, lehome, i18n("Home directory:"), whatstr);

#ifdef EXTENDED_GECOS_BSD
    // FreeBSD appears to use the comma separated fields in the GECOS entry
    // differently than Linux.
    leoffice = new QLineEdit(frame);  
    QObject::connect(leoffice, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Office");
    addRow(frame, layout, row++, leoffice, i18n("Office:"), whatstr);

    leophone = new QLineEdit(frame);
    QObject::connect(leophone, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Office Phone");
    addRow(frame, layout, row++, leophone, i18n("Office Phone:"), whatstr);

    lehphone = new QLineEdit(frame);
    QObject::connect(lehphone, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Home Phone");
    addRow(frame, layout, row++, lehphone, i18n("Home Phone:"), whatstr);
#else
    leoffice1 = new QLineEdit(frame);
    QObject::connect(leoffice1, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Office1");
    addRow(frame, layout, row++, leoffice1, i18n("Office #1:"), whatstr);

    leoffice2 = new QLineEdit(frame);
    QObject::connect(leoffice2, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Office2");
    addRow(frame, layout, row++, leoffice2, i18n("Office #2:"), whatstr);

    leaddress = new QLineEdit(frame);
    QObject::connect(leaddress, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Address");
    addRow(frame, layout, row++, leaddress, i18n("Address:"), whatstr);
#endif
    frontrow = row;
  }

#ifdef HAVE_SHADOW
  // Tab 2 : Password Management
  if (is_shadow != 0) {
    QFrame *frame = addPage(i18n("Password Management"));
    QGridLayout *layout = new QGridLayout(frame, 20, 4, marginHint(), spacingHint());
    int row = 0;

    QDate lastChange = QDate(1970,1,1).addDays(user->getLastChange());
    leslstchg = new QLabel(frame);
    leslstchg->setText(KGlobal::locale()->formatDate(lastChange));
    addRow(frame, layout, row++, leslstchg, i18n("Last password change:"), QString::null, true);
 
    layout->addMultiCellWidget(new KSeparator(KSeparator::HLine, frame), row, row, 0, 3);
    row++;

    lesmin = addDaysGroup(frame, layout, row++, i18n("Password may not be changed before"), i18n("after last password change."), user->getMin(), false);
    lesmax = addDaysGroup(frame, layout, row++, i18n("Password expires"), i18n("after last password change."), user->getMax());
    leswarn = addDaysGroup(frame, layout, row++, i18n("Issue expire warning"), i18n("before password expires."), user->getWarn());
    lesinact = addDaysGroup(frame, layout, row++, i18n("Account will be disabled"), i18n("after expiration of password"), user->getInactive());

    layout->addMultiCellWidget(new KSeparator(KSeparator::HLine, frame), row, row, 0, 3);
    row++;

    lesexpire = addDateGroup(frame, layout, row++, i18n("Account will expire on:"), user->getExpire()); 
  }
#endif

#ifdef _KU_QUOTA
  // Tab 3: Quotas
  if (is_quota != 0) {
    QFrame *frame = addPage(i18n("Quota"));
    QGridLayout *layout = new QGridLayout(frame, 3, 2, marginHint(), spacingHint());

    leqmnt = new QComboBox(false, frame);
//    whatstr = i18n("WHAT IS THIS: Select filesystem");
    addRow(frame, layout, 0, leqmnt, i18n("Filesystem with quotas:"), whatstr, false);

    leqmnt->clear();

    for (uint i = 0; i<kug->getMounts().getMountsNumber(); i++)
       leqmnt->insertItem(kug->getMounts()[i]->getdir());

    QObject::connect(leqmnt, SIGNAL(highlighted(int)), this, SLOT(mntsel(int)));

    {
      QGroupBox *group = new QGroupBox( frame );
      layout->addMultiCellWidget(group, 1, 1, 0, 1);
      group->setTitle("Disk Space");
      QGridLayout *groupLayout = new QGridLayout(group, 6, 2, marginHint(), spacingHint()); 
      groupLayout->addRowSpacing(0, group->fontMetrics().lineSpacing());
      groupLayout->setColStretch(0,1);
      int row = 1;

      leqfs = new QLineEdit(group);
      leqfs->setValidator(new QIntValidator(group));
      QObject::connect(leqfs, SIGNAL(textChanged(const QString &)), this, SLOT(qchanged()));
//      whatstr = i18n("WHAT IS THIS: Disk space soft quota");
      addRow(group, groupLayout, row++, leqfs, i18n("Disk space soft quota:"), whatstr, false);

      leqfh = new QLineEdit(group);
      leqfh->setValidator(new QIntValidator(group));
      QObject::connect(leqfh, SIGNAL(textChanged(const QString &)), this, SLOT(qchanged()));
//      whatstr = i18n("WHAT IS THIS: Disk space hard quota");
      addRow(group, groupLayout, row++, leqfh, i18n("Disk space hard quota:"), whatstr, false);

      leqfcur = new QLabel(group);
//      whatstr = i18n("WHAT IS THIS: Disk usage");
      addRow(group, groupLayout, row++, leqfcur, i18n("Disk space in use:"), whatstr, false);

#ifndef BSD
      leqft = new QLineEdit(group);
      leqft->setValidator(new QIntValidator(group));
      QObject::connect(leqft, SIGNAL(textChanged(const QString &)), this, SLOT(qchanged()));
//      whatstr = i18n("WHAT IS THIS: Time limit allowed for soft quota");
      addRow(group, groupLayout, row++, leqft, i18n("Grace period:"), whatstr, false);
#endif
    }


    {
      QGroupBox *group = new QGroupBox( frame );
      layout->addMultiCellWidget(group, 2, 2, 0, 1);
      group->setTitle("Number of Files");
      QGridLayout *groupLayout = new QGridLayout(group, 6, 2, marginHint(), spacingHint()); 
      groupLayout->addRowSpacing(0, group->fontMetrics().lineSpacing());
      groupLayout->setColStretch(0,1);
      int row = 1;

      leqis = new QLineEdit(group);
      leqis->setValidator(new QIntValidator(group));
      QObject::connect(leqis, SIGNAL(textChanged(const QString &)), this, SLOT(qchanged()));
//      whatstr = i18n("WHAT IS THIS: File number soft quota");
      addRow(group, groupLayout, row++, leqis, i18n("File number soft quota:"), whatstr, false);

      leqih = new QLineEdit(group);
      leqih->setValidator(new QIntValidator(group));
      QObject::connect(leqih, SIGNAL(textChanged(const QString &)), this, SLOT(qchanged()));
//      whatstr = i18n("WHAT IS THIS: File number hard quota");
      addRow(group, groupLayout, row++, leqih, i18n("File number hard quota:"), whatstr, false);

      leqicur = new QLabel(group);
//      whatstr = i18n("WHAT IS THIS: File number usage");
      addRow(group, groupLayout, row++, leqicur, i18n("Number of files in use:"), whatstr, false);

#ifndef BSD
      leqit = new QLineEdit(group);
      leqit->setValidator(new QIntValidator(group));
      QObject::connect(leqit, SIGNAL(textChanged(const QString &)), this, SLOT(qchanged()));
//      whatstr = i18n("WHAT IS THIS: Time limit allowed for file number soft quota");
      addRow(group, groupLayout, row++, leqit, i18n("Grace period:"), whatstr, false);
#endif
    }
  }
#endif

  // Tab 4: Groups
  {
    QFrame *frame = addPage(i18n("Groups"));
    QGridLayout *layout = new QGridLayout(frame, 20, 3, marginHint(), spacingHint());
    int row = 0;

//    whatstr = i18n("WHATSTHIS: Primary Group");
    cbpgrp = new QComboBox(false, frame, "cbpgrp");
    addRow(frame, layout, row++, cbpgrp, i18n("Primary group:"), whatstr, false);
    QObject::connect(cbpgrp, SIGNAL(activated(const QString &)), this, SLOT(setpgroup(const QString &)));

    lstgrp = new KListView(frame);
    lstgrp->setFullWidth(); // Single column, full widget width.
    lstgrp->addColumn(i18n("Groups user '%1' belongs to:").arg(user->getName()));
//    QString whatstr = i18n("Select the groups that this user belongs to.");
    QWhatsThis::add(lstgrp, whatstr);
    layout->addMultiCellWidget(lstgrp, row, row, 0, 1);
    row++;    
    QObject::connect(lstgrp, SIGNAL(clicked(QListViewItem *)), this, SLOT(gchanged())); 
  }
  
#ifdef EXTENDED_GECOS_BSD
  // Tab5: extended BSD tab
  {
    QFrame *frame = addPage(i18n("Groups"));
    QGridLayout *layout = new QGridLayout(frame, 20, 4, marginHint(), spacingHint());
    int row = 0;

    leclass = new QLineEdit(frame);
    QObject::connect(leclass, SIGNAL(textChanged(const QString &)), this, SLOT(changed()));
//    whatstr = i18n("WHAT IS THIS: Login class");
    addRow(frame, layout, row++, leclass, i18n("Login class:"), whatstr, true);

    leexpire = addDateGroup(frame, layout, row++, i18n("Account will expire after:"), user->getExpire()); 
  }
#endif

  loadgroups();
  selectuser();
//  setFixedSize(450, 470);

  ischanged = false;
  isgchanged = false;
#ifdef _KU_QUOTA
  isqchanged = false;
#endif

}

propdlg::~propdlg() {
}

void propdlg::loadgroups() {
  QStringList groups;
  for (uint i = 0; i<kug->getGroups().count(); i++) {
    KGroup *group = kug->getGroups()[i];
    QString groupName = group->getName();
    QCheckListItem *item = new QCheckListItem(lstgrp, groupName, QCheckListItem::CheckBox);
    groups.append(groupName);
    if (group->getGID() == user->getGID())
    {
      item->setOn(true);
      item->setEnabled(false);
      primaryGroup = groupName;
      primaryGroupWasOn = group->lookup_user(user->getName());
    }
    else if (group->lookup_user(user->getName()))
    {
      item->setOn(true);
    }
  }
  groups.sort();
  cbpgrp->insertStringList(groups);
  cbpgrp->setCurrentItem(groups.findIndex(primaryGroup));
}

void propdlg::setpgroup(const QString &) {
  isgchanged = true;
  if (cbpgrp->currentText() == primaryGroup)
     return;

  QString prevPrimaryGroup = primaryGroup;
  bool prevPrimaryGroupWasOn = primaryGroupWasOn;
  primaryGroup = cbpgrp->currentText();

  QCheckListItem *item = (QCheckListItem *) lstgrp->firstChild();

  QString userName = user->getName();
  while(item)
  {
     QString groupName = item->text();
     if (groupName  == prevPrimaryGroup)
     {
        KGroup *group = kug->getGroups().lookup(groupName);
        item->setEnabled(true);
        item->setOn(prevPrimaryGroupWasOn);
        item->repaint();
     }
     else if (groupName == primaryGroup)
     {
        primaryGroupWasOn = item->isOn();
        item->setEnabled(false);
        item->setOn(true);
        item->repaint();
     }

     item = (QCheckListItem *) item->nextSibling();
  }
}

void propdlg::changed() {
  ischanged = true;
}

void propdlg::gchanged() {
  isgchanged = true;
}

void propdlg::save() {
  
  user->setUID(leid->text().toInt());
  user->setFullName(lefname->text());

#ifdef EXTENDED_GECOS_BSD
  QDateTime epoch ;
  QDateTime temp_time ;

  user->setOffice(leoffice->text());
  user->setWorkPhone(leophone->text());
  user->setHomePhone(lehphone->text());
  user->setClass(leclass->text());

  // date() returns days since the epoch if a date is specified	
  // which we convert to seconds since the epoch.
  // date() returns 0 if the `never expires' box is checked which
  // ends up being converted to 0 seconds which is what we need.
  epoch.setTime_t(0);
  user->setExpire(epoch.secsTo(leexpire->date()));
#else
  user->setOffice1(leoffice1->text());
  user->setOffice2(leoffice2->text());
  user->setAddress(leaddress->text());
#endif

  user->setHomeDir(lehome->text());
  if (leshell->currentItem() != 0)
    user->setShell(leshell->text(leshell->currentItem()));
  else
    user->setShell("");

#ifdef HAVE_SHADOW
  if (is_shadow) {
    QDate epoch(1970,1,1);
    if (lesmin->isEnabled())
       user->setMin(lesmin->value());
    else
       user->setMin(-1);

    if (lesmax->isEnabled())
       user->setMax(lesmax->value());
    else
       user->setMin(-1);

    if (leswarn->isEnabled())
       user->setWarn(leswarn->value());
    else
       user->setMin(-1);

    if (lesinact->isEnabled())
       user->setInactive(lesinact->value());
    else
       user->setMin(-1);

    if (lesexpire->isEnabled())
       user->setExpire(epoch.daysTo(lesexpire->date()));
//       user->setExpire(lesexpire->date().daysTo(epoch));
    else
       user->setExpire(-1);

//    user->setLastChange(today()); // Huh?
  }
#endif
}

void propdlg::saveg() {
  QCheckListItem *item = (QCheckListItem *) lstgrp->firstChild();

  QString userName = user->getName();
  while(item)
  {
     KGroup *group = kug->getGroups().lookup(item->text());
     if (item->isOn())
        group->addUser(userName);
     else
        group->removeUser(userName);

     item = (QCheckListItem *) item->nextSibling();
  }

  user->setGID(kug->getGroups().lookup(cbpgrp->currentText())->getGID());
}

#ifdef _KU_QUOTA
void propdlg::mntsel(int index) {
  saveq();

  chquota = index;
  selectuser();
}

void propdlg::qchanged() {
  isqchanged = true;
}

void propdlg::saveq() {
  QuotaMnt *tmpq = (*quota)[chquota];

  tmpq->setfhard(leqfh->text().toLong());
  tmpq->setfsoft(leqfs->text().toLong());
#ifndef BSD
  tmpq->setftime(leqft->text().toLong());
#endif
  tmpq->setihard(leqih->text().toLong());
  tmpq->setisoft(leqis->text().toLong());
#ifndef BSD
  tmpq->setitime(leqit->text().toLong());
#endif
}

/* These do not seem to be necessary but leaving just in case...  */

#else
void propdlg::qchanged() {
}

void propdlg::mntsel(int) {
}
#endif

bool propdlg::check() {
  bool ret = false;

  if (ischanged) {
    save();
    ret = true;
  }

  if (isgchanged) {
    saveg();
    ret = true;
  }

#ifdef _KU_QUOTA
  if (isqchanged) {
    saveq();
    ret = true;
  }
#endif

  return ret;
}

void propdlg::selectuser() {
  leuser->setText(user->getName());

  leid->setText(QString("%1").arg(user->getUID()));
  olduid = user->getUID();

//  sprintf(uname, "%i", user->getp_gid());
//  legid->setText(uname);

  lefname->setText(user->getFullName());

  if (user->getShell().isEmpty() != TRUE) {
    bool tested = FALSE;
    for (int i=0; i<leshell->count(); i++)
      if (leshell->text(i) == user->getShell()) {
        tested = TRUE;
        leshell->setCurrentItem(i);
        break;
      }
    if (!tested) {
      leshell->insertItem(user->getShell());
      leshell->setCurrentItem(leshell->count()-1);
    }
  } else
    leshell->setCurrentItem(0);

  lehome->setText(user->getHomeDir());
#ifdef EXTENDED_GECOS_BSD
  leoffice->setText(user->getOffice());
  leophone->setText(user->getWorkPhone());
  lehphone->setText(user->getHomePhone());
  leclass->setText(user->getClass());
  // the date fields get set when the dialogue is built
#else
  leoffice1->setText(user->getOffice1());
  leoffice2->setText(user->getOffice2());
  leaddress->setText(user->getAddress());
#endif

#ifdef _KU_QUOTA
  if (is_quota != 0) {
    int q = 0;
    if (chquota != -1) {
      q = chquota;

      QuotaMnt *qm = (*quota)[q];
      leqfs->setText(QString("%1").arg(qm->getfsoft()));
      leqfh->setText(QString("%1").arg(qm->getfhard()));
      leqis->setText(QString("%1").arg(qm->getisoft()));
      leqih->setText(QString("%1").arg(qm->getihard()));
      leqfcur->setText(QString("%1").arg(qm->getfcur()));
      leqicur->setText(QString("%1").arg(qm->geticur()));

#ifndef BSD
      leqft->setText(QString("%1").arg(qm->getftime()));
      leqit->setText(QString("%1").arg(qm->getitime()));
#endif
    }
  }
#endif
}

void propdlg::setpwd() {
  pwddlg *pd;

  pd = new pwddlg(user, this, "pwddlg");
  pd->exec();
  ischanged = true;
  delete pd;
}

void propdlg::slotOk() {
  uid_t newuid = leid->text().toInt();

  if (olduid != newuid)
    if (kug->getUsers().lookup(newuid)) {
      err->addMsg(i18n("User with UID %1 already exists").arg(newuid));
      err->display();
      return;
    }
    
  if (check())
    accept();
  else
    reject();
}

#include "propdlg.moc"
