#ifndef _KU_ADDUSER_H_
#define _KU_ADDUSER_H_

#include <qchkbox.h>
#include "propdlg.h"

class addUser: public propdlg {
  Q_OBJECT
public:
#ifdef _KU_QUOTA
  addUser(KUser *AUser, Quota *AQuota, QWidget *parent = 0, const char *name = 0, int isprep = false);
#else
  addUser(KUser *AUser, QWidget *parent = 0, const char *name = 0, int isprep = false);
#endif

  void setCreateHomeDir(bool b) 
     { createhome->setChecked(b); }

  void setCopySkel(bool b)
     { copyskel->setChecked(b); }

  void setUsePrivateGroup(bool b)
     { usePrivateGroup->setChecked(b); }

  bool getUsePrivateGroup()
     { return usePrivateGroup->isChecked(); }


protected slots:
  virtual void slotOk();
protected:
  bool checkHome();
  bool checkMailBox();

  QCheckBox *createhome;
  QCheckBox *copyskel;
  QCheckBox *usePrivateGroup;
};

#endif // _KU_ADDUSER_H


