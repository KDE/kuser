#ifndef _XU_MAINDLG_H
#define _XU_MAINDLG_H

#include "includes.h"

class maindlg:public KTopLevelWidget
{
Q_OBJECT
public:
  maindlg( QWidget *parent = 0, const char *name = 0);

  QListBox* list;
  
public slots:
  void properties();
  void quit();
  void highlighted(int i);
  void selected(int i);
  void edit();
  void del();
  void add();
  void about();
  void help();
  void setpwd();
protected:
  void reload(int id);
};

#endif // _XU_MAINDLG_H
