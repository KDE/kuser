#ifndef _KU_KUSERVW_H
#define _KU_KUSERVW_H

#include <qwidget.h>

#include <klistview.h>

#include "kuser.h"

class KUserView : public KListView
{
    Q_OBJECT

public:
  KUserView( QWidget* parent = 0, const char* name = 0 );

  virtual ~KUserView();

  void insertItem(KUser *aku);
  void removeItem(KUser *aku);
  KUser *getCurrentUser();

private:
  void init();
};

#endif // _KU_USERVW_H
