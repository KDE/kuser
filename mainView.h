#ifndef _KU_MAINDLG_H
#define _KU_MAINDLG_H

#include <stdlib.h>

#include <qevent.h>
#include <qptrlist.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qtabwidget.h>

#include "kuservw.h"
#include "kgroupvw.h"

class mainView : public QTabWidget {
Q_OBJECT
public:
  mainView(QWidget *parent = 0);
  ~mainView();

  void init();

  bool queryClose();

  QPixmap pic_user;

public slots:
  void slotTabChanged();

  void save();
  void properties();
  void userSelected();
  void groupSelected();

  void useradd();
  void useredit();
  void userdel();

  void grpadd();
  void grpedit();
  void grpdel();

  void setpwd();

signals:
  void userSelected(bool);
  void groupSelected(bool);

protected:
  void reloadUsers();
  void reloadGroups();

  KUserView *lbusers;
  KGroupView *lbgroups;
private:
  bool changed;
};

#endif // _KU_MAINDLG_H
