#ifndef _KU_PWDDLG
#define _KU_PWDDLG

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>

#include "kuser.h"

class pwddlg : public QDialog {
  Q_OBJECT

public:
  pwddlg(KUser &AUser, QWidget* parent = NULL, const char* name = NULL);
  ~pwddlg();

protected slots:
  void ok();
  void cancel();

private:
  KUser &user;

  QVBoxLayout *layout;
  QPushButton *pbOk;
  QPushButton *pbCancel;
  QLineEdit   *leusername1;
  QLineEdit   *leusername2;
};

#endif // _KU_PWDDLG
