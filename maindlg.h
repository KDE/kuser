#ifndef _KU_MAINDLG_H
#define _KU_MAINDLG_H

#include <ktablistbox.h>
#include <qevent.h>
#include <qlist.h>
#include <qpushbt.h>
#include <qpixmap.h>

#ifdef _KU_QUOTA
#include "mnt.h"
#include "quota.h"
#endif

#include "kuser.h"
#include "kuservw.h"
#include "kgroupvw.h"
#include "kheader.h"

class mainDlg:public QWidget
{
Q_OBJECT
public:
  mainDlg(QWidget *parent = 0);
  ~mainDlg();

  void init();

  KUserView *lbusers;
  KGroupView *lbgroups;
  int prev;
  QPixmap pic_user;

  KUsers *getUsers();
  KGroups *getGroups();
#ifdef _KU_QUOTA
  Mounts *getMounts();
  Quotas *getQuotas();
#endif
  
public slots:
  void properties();
  void quit();
  void userSelected(int i);
  void groupSelected(int i);
  void edit();
  void del();
  void add();
  void about();
  void help();
  void setpwd();
  void setUsersSort(int col);
  void setGroupsSort(int col);
  
protected:
  KUsers *u;
  KGroups *g;
#ifdef _KU_QUOTA
  Mounts *m;
  Quotas *q;
#endif
  void reloadUsers(int id);
  void reloadGroups(int gid);
  virtual void resizeEvent (QResizeEvent *rse);

private:
  QPushButton *pbquit;
  QPushButton *pbedit;
  QPushButton *pbdel;
  QPushButton *pbadd;

  int usort;
  int gsort;
  bool changed;
};

#ifndef _KU_MAIN
extern mainDlg *md;

#ifdef _KU_QUOTA
#define mounts md->getMounts()
#define quotas md->getQuotas()
#endif

#define users  md->getUsers()
#endif

#endif // _KU_MAINDLG_H
