#ifndef _KU_GRPNAMEDLG_H
#define _KU_GRPNAMEDLG_H

#include <qlineedit.h>

#include <kdialogbase.h>

#include "kgroup.h"

class grpnamedlg : public KDialogBase {
  Q_OBJECT
public:
  grpnamedlg(KGroup &AGroup, QWidget* parent = NULL, const char* name = NULL);
  ~grpnamedlg();

protected slots:
  void slotOk();

protected:
  KGroup &group;

  QLineEdit   *legrpname;
  QLineEdit   *legid;
};

#endif // _KU_GRPNAMEDLG_H

