#ifndef _KU_PWDDLG
#define _KU_PWDDLG

#include <qlineedit.h>
#include <kdialogbase.h>


#include "kuser.h"

class pwddlg : public KDialogBase {
  Q_OBJECT

public:
  pwddlg(KUser *AUser, QWidget* parent = NULL, const char* name = NULL);
  ~pwddlg();

protected slots:
  void slotOk();

private:
  KUser *user;

  QLineEdit   *leusername1;
  QLineEdit   *leusername2;
};

#endif // _KU_PWDDLG
