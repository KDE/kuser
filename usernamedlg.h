#ifndef _KU_USERNAMEDLG_H
#define _KU_USERNAMEDLG_H

#include <qlineedit.h>
#include <qcheckbox.h>

#include <kdialogbase.h>

#include "kuser.h"

class usernamedlg : public KDialogBase {
  Q_OBJECT
public:
  usernamedlg(KUser*auser, QWidget* parent = NULL, const char* name = NULL);
  ~usernamedlg();

protected slots:
  void slotOk();

protected:
  KUser *user;
  QLineEdit   *leusername;
};

#endif // _KU_USERNAMEDLG_H

