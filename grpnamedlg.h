#ifndef _KU_GRPNAMEDLG_H
#define _KU_GRPNAMEDLG_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include "kgroup.h"

class grpnamedlg : public QDialog {
  Q_OBJECT
public:
  grpnamedlg(KGroup &AGroup, QWidget* parent = NULL, const char* name = NULL);
  ~grpnamedlg();

protected slots:
  void ok();
  void cancel();

protected:
  KGroup &group;

  QPushButton *pbOk;
  QPushButton *pbCancel;
  QLineEdit   *legrpname;
  QLineEdit   *legid;
};

#endif // _KU_GRPNAMEDLG_H

