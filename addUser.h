#ifndef _KU_ADDUSER_H_
#define _KU_ADDUSER_H_

#include <qchkbox.h>
#include "propdlg.h"

class addUser: public propdlg {
  Q_OBJECT
public:
#ifdef _KU_QUOTA
  addUser(KUser *auser, Quota *aquota, QWidget *parent = 0, const char *name = 0, int isprep = false);
#else
  addUser(KUser *auser, QWidget *parent = 0, const char *name = 0, int isprep = false);
#endif
protected slots:
  virtual void ok();
protected:
  void createHome();
  void copySkel();

  QCheckBox *createhome;
  QCheckBox *copyskel;
};

#endif // _KU_ADDUSER_H


