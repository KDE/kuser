#ifndef _KU_GROUPVW_H
#define _KU_GROUPVW_H

#include <qwidget.h>

#include <klistview.h>

#include "kgroup.h"

class KGroupView : public KListView
{
    Q_OBJECT

public:
  KGroupView( QWidget* parent = 0, const char* name = 0 );

  virtual ~KGroupView();

  void insertItem(KGroup *aku);
  void removeItem(KGroup *aku);
  KGroup *getCurrentGroup();

private:
  void init();
};

#endif // _KGROUPVW_H
