#ifndef _KU_PWDDLG
#define _KU_PWDDLG

//#include "includes.h"
#include <qdialog.h>
#include <qpushbt.h>
#include <qlined.h>
#include <qlayout.h>

#include "kuser.h"

class pwddlg : public QDialog {
  Q_OBJECT
public slots:
  void ok();
  void cancel();

public:
  pwddlg(KUser *auser, QWidget* parent = NULL, const char* name = NULL);

private:
  KUser *user;

  QPushButton *pbOk;
  QPushButton *pbCancel;
  QLineEdit   *leusername1;
  QLineEdit   *leusername2;
};

#endif // _KU_PWDDLG
