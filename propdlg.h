#ifndef _XU_PROPDLG_H_
#define _XU_PROPDLG_H_

#include "includes.h"
#include "kdatectl.h"

class propdlg : public QTabDialog
{
  Q_OBJECT

public:
  propdlg(KUser *auser, QWidget *parent = 0, const char *name = 0, int isprep = false);

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

  QPushButton *pbok;
  QPushButton *pbcancel;
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
  QLabel      *lbuser;
  char        *uname;
};

#endif // _XU_PROPDLG_H_

