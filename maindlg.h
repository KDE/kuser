#ifndef _KU_MAINDLG_H
#define _KU_MAINDLG_H

#include <ktablistbox.h>
#include <ktopwidget.h>
#include <ktoolbar.h>
#include <kmenubar.h>
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
#include "kheader.h"

class mainDlg:public KTopLevelWidget
{
Q_OBJECT
public:
  mainDlg(const char *name = 0);
  ~mainDlg();

  void init();

  KUserView *list;
  int prev;
  QPixmap pic_user;

  KUsers *getUsers();
#ifdef _KU_QUOTA
  Mounts *getMounts();
  Quotas *getQuotas();
#endif
  
public slots:
  void properties();
  void quit();
  void selected(int i);
  void edit();
  void del();
  void add();
  void about();
  void help();
  void setpwd();
  void setSort(int col);
  
protected:
  KUsers *u;
#ifdef _KU_QUOTA
  Mounts *m;
  Quotas *q;
#endif
  void reload(int id);
  virtual void resizeEvent (QResizeEvent *rse);

private:
  QPushButton *pbquit;
  QPushButton *pbedit;
  QPushButton *pbdel;
  QPushButton *pbadd;

  KToolBar *toolbar;
  KMenuBar *menubar;
  int sort;
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
