#include "kdatectl.h"
#include "kdatectl.moc"
#include "misc.h"

KDateCtl::KDateCtl(QWidget *parent, const char *name,
const char *checktitle, const char *title,
long int adate, int ax, int ay) {
  char tmp[200];

  x = ax; y = ay;

  snprintf(tmp, 200, "%s_day", name);
  day = addLineEdit(parent, tmp, x, y+25, 30, 27, "");

  snprintf(tmp, 200, "%s_month", name);
  month = new QComboBox(FALSE, parent, tmp);
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

  snprintf(tmp, 200, "%s_year", name);
  year = addLineEdit(parent, tmp, x+150, y+25, 50, 27, "");

  snprintf(tmp, 200, "%s_isempty", name);
  isempty = new QCheckBox(checktitle, parent, tmp);
  isempty->setGeometry(x, y, 200, 20); 

  snprintf(tmp, 200, "%s_title", name);
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
#ifdef _KU_DEBUG
  printf("%li|%d %d %d|%d\n", adate, iday, imonth, iyear, qd->isValid());
#endif
  delete qd;
}

long int KDateCtl::getDate() {
  long int r = 0;

  QDate *qd = NULL;
  QDate *base = NULL;
  
  base = new QDate(1970, 1, 1);
  qd = new QDate(iyear, imonth, iday);

  r = base->daysTo(*qd);

#ifdef _KU_DEBUG
  printf("KDateCtl::getDate() -> %li\n", r);
#endif

  delete base;
  delete qd;
  
  return (r);
}

void KDateCtl::updateControls() {
  QString s("");

//  day->setText(s.setNum(iday));
//  month->setCurrentItem(imonth-1);
//  year->setText(s.setNum(iyear));

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
 
void KDateCtl::isEmptyToggled(bool state) { 
  updateControls();
#ifdef _KU_DEBUG
  puts("Checked");
#endif
}

int KDateCtl::validate(const char *text) {
  for (unsigned int i=0; i<strlen(text);i++)
    if ((text[i]<'0')||(text[i]>'9'))
      return 0;
      
  return 1;
}

void KDateCtl::dayChanged(const char *text) {
  long int tday = 0;

  if (validate(text) == 1) {
    tday = strtol(text, (char **)NULL, 10);
    if (tday>31) {
      QString *tmp = new QString();
      day->setText(tmp->setNum(iday));
      delete tmp;
    } else
      iday = tday;
  } else {
    QString *tmp = new QString();
    day->setText(tmp->setNum(iday));
    delete tmp;
  }
}

void KDateCtl::monthChanged(int data) {
  imonth = month->currentItem()+1;
}

void KDateCtl::yearChanged(const char *text) {
  long int tyear = 0;

  if (validate(text) == 1) {
    tyear = strtol(text, (char **)NULL, 10);
    if (tyear>2023) {
      QString *tmp = new QString();
      year->setText(tmp->setNum(iday));
      delete tmp;
    } else
      iyear = tyear;
  } else {
    QString *tmp = new QString();
    year->setText(tmp->setNum(iday));
    delete tmp;
  }
}

void KDateCtl::setFont(const QFont &f) {
  day->setFont(f);
  month->setFont(f);
  year->setFont(f);
  label->setFont(f);
  isempty->setFont(f);
}

