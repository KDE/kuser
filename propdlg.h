#ifndef _KU_PROPDLG_H_
#define _KU_PROPDLG_H_

#include <qlined.h>
#include <qlabel.h>
#include <qcombo.h>
#include <qpushbt.h>
#include <qwidget.h>
#include <qlistbox.h>
#include <qtooltip.h>
#include <qtabdlg.h>

#include "kdatectl.h"
#include "kuser.h"
#include "quota.h"

class propdlg : public QTabDialog
{
  Q_OBJECT

public:
#ifdef _KU_QUOTA
  propdlg(KUser *auser, Quota *aquota, QWidget *parent = 0, const char *name = 0, int isprep = false);
#else
  propdlg(KUser *auser, QWidget *parent = 0, const char *name = 0, int isprep = false);
#endif
  ~propdlg();

protected slots:
  void ok();
  void cancel();
  void setpwd();
  void mntsel(int index);
  void qcharchanged(const char *);
  void shactivated(const char *text);
  void changed();
  void charchanged(const char *);
  void add();
  void del();

private:
  void selectuser();
  void save();
  void saveg();
  bool check();
  void loadgroups();

#ifdef _KU_QUOTA
  void saveq();
  int chquota;
  bool isqchanged;
#endif

  KUser       *user;
#ifdef _KU_QUOTA
  Quota *quota;
#endif
  bool ischanged;

  QWidget *w1;
  QWidget *w2;
  QWidget *w3;
  QWidget *w4;

  QListBox* m_Other;
  QListBox* m_Group;
  QPushButton* pbadd;
  QPushButton* pbdel;

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
  QLabel *l10a;
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
  QLineEdit   *lehome;
  QLineEdit   *leoffice1;
  QLineEdit   *leoffice2;
  QLineEdit   *leaddress;

#ifdef _KU_QUOTA
  QComboBox   *leqmnt;
  QLineEdit   *leqfs;
  QLineEdit   *leqfh;
  QLineEdit   *leqis;
  QLineEdit   *leqih;
  QLabel      *leqfcur;
  QLabel      *leqicur;
#endif

  QLabel      *leslstchg;
  KDateCtl    *lesmin;
  KDateCtl    *lesmax;
  KDateCtl    *leswarn;
  KDateCtl    *lesinact;
  KDateCtl    *lesexpire;
};

#endif // _KU_PROPDLG_H_

