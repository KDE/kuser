#ifndef _XU_PROPDLG_H_
#define _XU_PROPDLG_H_

#include "includes.h"
#include "kdatectl.h"

class propdlg : public QTabDialog
{
  Q_OBJECT

public:
  propdlg(KUser *auser, QWidget *parent = 0, const char *name = 0, int isprep = false);
  ~propdlg();

protected slots:
  void ok();
  void cancel();
  void save();
  void setpwd();
  void mntsel(int index);
  void shactivated(const char *text);

private:
  int chquota;
  int change();
  void selectuser();

private:
  KUser       *user;

  QWidget *w1;
  QWidget *w2;
  QWidget *w3;

  QLabel *l1;
  QLabel *l2;
  QLabel *ld3;
  QLabel *l4;
  QLabel *l5;
  QLabel *l6;
  QLabel *l7;
  QLabel *ld7;
  QLabel *l8;
  QLabel *ld8;
  QLabel *l9;
  QLabel *ld9;
  QLabel *l10;
  QLabel *l11;
  QLabel *l12;
  QLabel *l13;
  QLabel *l14;
  QLabel *l15;
  QLabel *l16;

  QPushButton *pbsetpwd;

  QLabel      *leuser;
  QLineEdit   *leid;
  QLineEdit   *legid;
  QLineEdit   *lefname;
  QComboBox   *leshell;
  QComboBox   *leqmnt;
  QLineEdit   *lehome;
  QLineEdit   *leoffice1;
  QLineEdit   *leoffice2;
  QLineEdit   *leaddress;
  QLineEdit   *leqfs;
  QLineEdit   *leqfh;
  QLineEdit   *leqis;
  QLineEdit   *leqih;
  QLabel      *leslstchg;
  KDateCtl    *lesmin;
  KDateCtl    *lesmax;
  KDateCtl    *leswarn;
  KDateCtl    *lesinact;
  KDateCtl    *lesexpire;
  QLabel      *leqfcur;
  QLabel      *leqicur;
};

#endif // _XU_PROPDLG_H_

