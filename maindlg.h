#ifndef _KU_MAINDLG_H
#define _KU_MAINDLG_H

#include <ktablistbox.h>
#include "includes.h"

class maindlg:public KTopLevelWidget
{
Q_OBJECT
public:
  maindlg(const char *name = 0);

  KTabListBox *list;
  int prev;
  QPixmap pic_user;
  
public slots:
  void properties();
  void quit();
  void selected(int i,int);
  void edit();
  void del();
  void add();
  void about();
  void help();
  void setpwd();
  void setSort(int col);
protected:
  void reload(int id);
private:
  bool changed;
};

#endif // _KU_MAINDLG_H
