#include <stdlib.h>
#include <stdio.h>

#include <qvalidator.h>
#include <qdatetm.h>

#include "kdatectl.h"
#include "kdatectl.moc"
#include "misc.h"

KDateCtl::KDateCtl(QWidget *parent, const char *name,
const char *checktitle, const char *title,
long int adate, int ax, int ay) {
  char tmp[200];

  x = ax; y = ay;

  sprintf(tmp, "%s_day", name);
  day = addLineEdit(parent, tmp, x, y+25, 30, 27, "");
  sprintf(tmp,  "va_%s_day", name);
  day->setValidator(new QIntValidator(1, 31, parent, name));

  sprintf(tmp, "%s_month", name);
  month = new KCombo(FALSE, parent, tmp);
  month->clear();
  month->insertItem(_("January"));
  month->insertItem(_("February"));
  month->insertItem(_("March"));
  month->insertItem(_("April"));
  month->insertItem(_("May"));
  month->insertItem(_("June"));
  month->insertItem(_("July"));
  month->insertItem(_("August"));
  month->insertItem(_("September"));
  month->insertItem(_("October"));
  month->insertItem(_("November"));
  month->insertItem(_("December"));

  month->setGeometry(x+40, y+25, 100, 27);

  sprintf(tmp,  "%s_year", name);
  year = addLineEdit(parent, tmp, x+150, y+25, 50, 27, "");
  sprintf(tmp,  "va_%s_year", name);
  year->setValidator(new QIntValidator(1970, 2023, parent, name));

  sprintf(tmp, "%s_isempty", name);
  isempty = new QCheckBox(checktitle, parent, tmp);
  isempty->setGeometry(x, y, 200, 20); 

  sprintf(tmp, "%s_title", name);
  label = addLabel(parent, tmp, x+210, y+25, 50, 27, title);

  QObject::connect(isempty, SIGNAL(toggled(bool)), this,
                            SLOT(isEmptyToggled(bool)));
  QObject::connect(day, SIGNAL(textChanged(const char *)), this,
                            SLOT(dayChanged(const char *)));
  QObject::connect(year, SIGNAL(textChanged(const char *)), this,
                            SLOT(yearChanged(const char *)));
  QObject::connect(month, SIGNAL(activated(int)), this,
                            SLOT(monthChanged(int)));

  setDate(adate);
}

KDateCtl::~KDateCtl() {
  delete day;
  delete month;
  delete year;
  delete isempty;
  delete label;
}

void KDateCtl::setDate(long int adate) {
  QDate *qd = NULL;

  if (adate>24855) {
    isempty->setChecked(TRUE);
    return;
  }

  qd = new QDate(1970, 1, 1);
  *qd = qd->addDays((int)adate);

  iday = qd->day();
  imonth = qd->month();
  iyear = qd->year();

  updateControls();
  delete qd;
}

long int KDateCtl::getDate() {
  long int r = 0;

  QDate *qd = NULL;
  QDate *base = NULL;
  
  base = new QDate(1970, 1, 1);
  qd = new QDate(iyear, imonth, iday);

  r = base->daysTo(*qd);

  delete base;
  delete qd;
  
  return (r);
}

void KDateCtl::updateControls() {
  QString s("");

  if (isempty->isChecked() == TRUE) {
    day->setText("");
    month->setCurrentItem(1);
    year->setText("");
    day->setEnabled(FALSE);
    month->setEnabled(FALSE);
    year->setEnabled(FALSE);
  } else {
    day->setText(s.setNum(iday));
    month->setCurrentItem(imonth-1);
    year->setText(s.setNum(iyear));
    day->setEnabled(TRUE);
    month->setEnabled(TRUE);
    year->setEnabled(TRUE);
  }
}
 
void KDateCtl::isEmptyToggled(bool) { 
  updateControls();
}

void KDateCtl::dayChanged(const char *text) {
  long int tday = 0;

  tday = strtol(text, (char **)NULL, 10);
  QString *tmp = new QString();
  day->setText(tmp->setNum(iday));
  delete tmp;

  textChanged();
}

void KDateCtl::monthChanged(int) {
  imonth = month->currentItem()+1;

  textChanged();
}

void KDateCtl::yearChanged(const char *) {
  QString *tmp = new QString();
  year->setText(tmp->setNum(iday));
  delete tmp;

  textChanged();
}

void KDateCtl::setFont(const QFont &f) {
  day->setFont(f);
  month->setFont(f);
  year->setFont(f);
  label->setFont(f);
  isempty->setFont(f);
}

