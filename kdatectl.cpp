#include <stdlib.h>
#include <stdio.h>

#include <qvalidator.h>
#include <qdatetm.h>

#include "kdatectl.h"
#include "misc.h"

KDateCtl::KDateCtl(QWidget *parent, const char *name,
                   const char *checktitle, const char *title,
                   long adate, long abase, int ax, int ay) {
  x = ax; y = ay;

  day = addLineEdit(parent, QString("%1_day").arg(name), x, y+25, 30, 27, "");
  day->setValidator(new QIntValidator(1, 31, parent, QString("va_%1_day").arg(name)));

  month = new QComboBox(FALSE, parent, QString("%1_month").arg(name));
  month->clear();
  month->insertItem(i18n("January"));
  month->insertItem(i18n("February"));
  month->insertItem(i18n("March"));
  month->insertItem(i18n("April"));
  month->insertItem(i18n("May"));
  month->insertItem(i18n("June"));
  month->insertItem(i18n("July"));
  month->insertItem(i18n("August"));
  month->insertItem(i18n("September"));
  month->insertItem(i18n("October"));
  month->insertItem(i18n("November"));
  month->insertItem(i18n("December"));

  month->setGeometry(x+40, y+25, 100, 27);

  year = addLineEdit(parent, QString("%1_year").arg(name), x+150, y+25, 50, 27, "");
  year->setValidator(new QIntValidator(1970, 2023, parent, QString("va_%1_year").arg(name)));

  isempty = new QCheckBox(checktitle, parent, QString("%1_isempty").arg(name));
  isempty->setGeometry(x, y, 200, 20); 
  isempty->setAutoResize(true);

  label = addLabel(parent, QString("%1_title").arg(name), x+210, y+25, 50, 27, title);

  QObject::connect(isempty, SIGNAL(toggled(bool)), this,
                            SLOT(isEmptyToggled(bool)));
  QObject::connect(day, SIGNAL(textChanged(const QString &)), this,
                            SLOT(dayChanged(const QString &)));
  QObject::connect(year, SIGNAL(textChanged(const QString &)), this,
                            SLOT(yearChanged(const QString &)));
  QObject::connect(month, SIGNAL(activated(int)), this,
                            SLOT(monthChanged(int)));

  setDate(adate, abase);
}

KDateCtl::~KDateCtl() {
  delete day;
  delete month;
  delete year;
  delete isempty;
  delete label;
}

void KDateCtl::setDate(long int adate, long int abase) {
  QDate *qd = NULL;

  base = abase;

  if ((adate>24855) || (adate<abase)) {
    iday = 0;
    imonth = 0;
    iyear = 0;
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

long KDateCtl::getDate() {
  long int r = 0;

  QDate *qd = NULL;
  QDate *b = NULL;
  
  if (isempty->isChecked() == TRUE)
    return (base-1);
  
  b = new QDate(1970, 1, 1);
  qd = new QDate(iyear, imonth, iday);

  r = b->daysTo(*qd);

  delete b;
  delete qd;
  
  return (r);
}

long KDateCtl::getBase() {
  return (base);
}

void KDateCtl::updateControls() {
  QString s("");

  if (isempty->isChecked() == TRUE) {
    iday = 1;
    imonth = 1;
    iyear = 1970;
    day->setText("1");
    month->setCurrentItem(0);
    year->setText("1970");
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
  emit textChanged();
}

void KDateCtl::dayChanged(const QString &text) {
  iday = text.toInt();
  emit textChanged();
}

void KDateCtl::monthChanged(int) {
  imonth = month->currentItem()+1;
  emit textChanged();
}

void KDateCtl::yearChanged(const QString &text) {
  iyear = text.toInt();
  emit textChanged();
}

void KDateCtl::setFont(const QFont &f) {
  day->setFont(f);
  month->setFont(f);
  year->setFont(f);
  label->setFont(f);
  isempty->setFont(f);
}
