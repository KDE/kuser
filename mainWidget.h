#ifndef _KU_MAINWIDGET_H
#define _KU_MAINWIDGET_H

#include <ktopwidget.h>
#include <ktoolbar.h>
#include <kmenubar.h>
#include <qevent.h>
#include <qpixmap.h>

class mainWidget:public KTopLevelWidget
{
Q_OBJECT
public:
  mainWidget(const char *name = 0);
  ~mainWidget();

protected:
  virtual void resizeEvent (QResizeEvent *);

private:
  KMenuBar *menubar;
  KToolBar *toolbar;
};

#endif // _KU_MAINWIDGET_H
