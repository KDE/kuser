#ifndef _KU_MAINDLG_H
#define _KU_MAINDLG_H

#include <stdlib.h>

#include <qevent.h>
#include <qlist.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qsplitter.h>

#include "kuservw.h"
#include "kgroupvw.h"
#include "kheader.h"

class mainView : public QWidget {
Q_OBJECT
public:
  mainView(QWidget *parent = 0);
  ~mainView();

  void init();

  QPixmap pic_user;

public slots:
  void save();
  void properties();
  void quit();
  void userSelected(int i);
  void groupSelected(int i);
  
  void useradd();
  void useredit();
  void userdel();
  
  void grpadd();
  void grpedit();
  void grpdel();
  
  void help();
  void setpwd();
  void setUsersSort(int col);
  void setGroupsSort(int col);
  
protected:
  void reloadUsers(int id);
  void reloadGroups(int gid);
  virtual void resizeEvent (QResizeEvent *rse);

  KUserView *lbusers;
  KGroupView *lbgroups;
  int prev;
  QSplitter *kp;
private:
  int usort;
  int gsort;
  bool changed;
};

#endif // _KU_MAINDLG_H
