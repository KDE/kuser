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
  void setUsePrivateGroup(bool data);
  void setCreateHomeDir(bool data);
  void setCopySkel(bool data);

protected slots:
  virtual void ok();

  virtual void usePrivateGroupChecked(bool data);
  virtual void createHomeChecked(bool data);

protected:
  bool checkHome();
  bool checkMailBox();

  QCheckBox *createhome;
  QCheckBox *copyskel;
  QCheckBox *usePrivateGroup;
};

#endif // _KU_ADDUSER_H


