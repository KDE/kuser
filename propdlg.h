#ifndef _KU_PROPDLG_H_
#define _KU_PROPDLG_H_

#include <qlineedit.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qlistbox.h>
#include <qtooltip.h>
#include <qdialog.h>

#include <ktabctl.h>
#include "kdatectl.h"
#include "kuser.h"
#include "quota.h"
#include "globals.h"

class propdlg : public QDialog
{
  Q_OBJECT

public:
#ifdef _KU_QUOTA
  propdlg(KUser &AUser, Quota &AQuota, QWidget *parent = 0, const char *name = 0, int isprep = false);
#else
  propdlg(KUser &AUser, QWidget *parent = 0, const char *name = 0, int isprep = false);
#endif
  ~propdlg();

protected slots:
  virtual void ok();
  void cancel();
  void setpwd();
  void mntsel(int index);
  void qcharchanged(const QString &);
  void shactivated(const QString &);
  void changed();
  void charchanged(const QString &);
  void add();
  void del();
  void setpgroup(const QString &);

protected:
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

  KUser &user;
#ifdef _KU_QUOTA
  Quota &quota;
#endif
  bool ischanged;
  uid_t olduid;

  KTabCtl *tw;
  
  QWidget *w1;
  QWidget *w2;
  QWidget *w3;
  QWidget *w4;
#if defined(__FreeBSD__) || defined(__bsdi__)
  // extra tab page for login class & expiry dates
  QWidget *w5;
#endif

  QListBox* m_Other;
  QListBox* m_Group;
  QPushButton* pbadd;
  QPushButton* pbdel;

  QLabel *l1;
  QLabel *l2;
//  QLabel *ld3;
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
  QLabel *l17;
  QLabel *l18;
#if defined(__FreeBSD__) || defined(__bsdi__)
  // labels for login class
  QLabel *l19;
  QLabel *ld19;
#endif

  QPushButton *pbsetpwd;
  QPushButton *pbok;
  QPushButton *pbcancel;

  QLabel      *leuser;
  QLineEdit   *leid;
//  QLineEdit   *legid;
  QLineEdit   *lefname;
  QComboBox   *leshell;
  QLineEdit   *lehome;
#if defined(__FreeBSD__) || defined(__bsdi__)
  QLineEdit   *leoffice;
  QLineEdit   *leophone;
  QLineEdit   *lehphone;
  QLineEdit   *leclass;
  KDateCtl    *lechange;
  KDateCtl    *leexpire;
#else
  QLineEdit   *leoffice1;
  QLineEdit   *leoffice2;
  QLineEdit   *leaddress;
#endif

  QComboBox   *cbpgrp;
#ifdef _KU_QUOTA
  QComboBox   *leqmnt;
  QLineEdit   *leqfs;
  QLineEdit   *leqfh;
#ifndef BSD
  QLineEdit   *leqft;
#endif
  QLineEdit   *leqis;
  QLineEdit   *leqih;
#ifndef BSD
  QLineEdit   *leqit;
#endif
  QLabel      *leqfcur;
  QLabel      *leqicur;
#endif

#ifdef _KU_SHADOW
  QLabel      *leslstchg;
  KDateCtl    *lesmin;
  KDateCtl    *lesmax;
  KDateCtl    *leswarn;
  KDateCtl    *lesinact;
  KDateCtl    *lesexpire;
#endif
};

#endif // _KU_PROPDLG_H_

