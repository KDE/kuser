#ifndef _KU_PROPDLG_H_
#define _KU_PROPDLG_H_

// FreeBSD and BSDI have slightly different GECOS handling
// from Linux systems. Define this to distinguish flavors of
// handling (still not nice, but hey ...)
//
#if defined(__FreeBSD__) || defined(__bsdi__)
#define EXTENDED_GECOS_BSD
#endif


#include <qlineedit.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qlistbox.h>
#include <qtooltip.h>
#include <qcheckbox.h>
#include <qlayout.h>

#include <kdatewidget.h>
#include <knuminput.h>
#include <kdialogbase.h>
#include <klistview.h>

#include "kuser.h"
#include "quota.h"
#include "globals.h"

class propdlg : public KDialogBase
{
  Q_OBJECT

public:
#ifdef _KU_QUOTA
  propdlg(KUser *AUser, Quota *AQuota, QWidget *parent = 0, const char *name = 0, int isprep = false);
#else
  propdlg(KUser *AUser, QWidget *parent = 0, const char *name = 0, int isprep = false);
#endif
  ~propdlg();

protected slots:
  virtual void slotOk();
  void setpwd();
  void mntsel(int index);
  void qchanged(); // Change to quota settings
  void changed(); // Change to misc settings
  void gchanged(); // Change to group settings
  void setpgroup(const QString &); // Change in primary group

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
  KDateWidget *addDateGroup(QWidget  *parent, QGridLayout *layout, int row, const QString &title, int days);
  KIntSpinBox *addDaysGroup(QWidget  *parent, QGridLayout *layout, int row, const QString &title, const QString &title2, int days, bool never=true);

  QFrame *frontpage;
  QGridLayout *frontlayout;
  int frontrow;

  KUser *user;
#ifdef _KU_QUOTA
  Quota *quota;
#endif
  bool ischanged;
  bool isgchanged;
  uid_t olduid;

  KListView *lstgrp;

  QPushButton *pbsetpwd;

  QLabel      *leuser;
  QLineEdit   *leid;
  QLineEdit   *lefname;
  QComboBox   *leshell;
  QLineEdit   *lehome;
#ifdef EXTENDED_GECOS_BSD
  QLineEdit   *leoffice;
  QLineEdit   *leophone;
  QLineEdit   *lehphone;
  QLineEdit   *leclass;
  KDateWidget *lechange;
  KDateWidget *leexpire;
#else
  QLineEdit   *leoffice1;
  QLineEdit   *leoffice2;
  QLineEdit   *leaddress;
#endif

  QComboBox   *cbpgrp;
  QString      primaryGroup;
  bool	       primaryGroupWasOn;
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

#ifdef HAVE_SHADOW
  QLabel *leslstchg;
  KIntSpinBox *lesmin;
  KIntSpinBox *lesmax;
  KIntSpinBox *leswarn;
  KIntSpinBox *lesinact;
  KDateWidget  *lesexpire;
#endif
};

#endif // _KU_PROPDLG_H_

