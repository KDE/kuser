#ifndef username_included
#define username_included

#include "includes.h"

class usernamedlg : public QDialog
{
Q_OBJECT
public slots:
  void ok();
  void cancel();
public:
  usernamedlg(KUser*auser, QWidget* parent = NULL, const char* name = NULL);
private:
  KUser *user;

  QPushButton *pbOk;
  QPushButton *pbCancel;
  QLineEdit   *leusername;
};
#endif // username_included

