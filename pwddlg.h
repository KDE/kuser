#ifndef _KU_PWDDLG
#define _KU_PWDDLG

//#include "includes.h"
#include <qdialog.h>
#include <qpushbt.h>

#include "kuser.h"
#include "mrqpasswdlined.h"


class pwddlg : public QDialog
{
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
  MrQPasswordLineEdit   *leusername1;
  MrQPasswordLineEdit   *leusername2;
};
#endif // _KU_PWDDLG

