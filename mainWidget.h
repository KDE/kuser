#ifndef _KU_MAINWIDGET_H
#define _KU_MAINWIDGET_H

#include <qevent.h>
#include <qpixmap.h>

#include <ktmainwindow.h>
#include <kstatusbar.h>

#include "mainView.h"

class mainWidget : public KTMainWindow {
Q_OBJECT
public:
  mainWidget(const char *name = 0);
  ~mainWidget();

protected:
  virtual void resizeEvent (QResizeEvent *event);
  virtual void closeEvent (QCloseEvent *event);
  
  void setupActions();
  void readSettings();
  void writeSettings();

protected slots:
  void toggleToolBar();
  void toggleStatusBar();

private:
//  KMenuBar   *menubar;
//  KToolBar   *toolbar;
  KStatusBar *sbar;
  mainView *md;
};

#endif // _KU_MAINWIDGET_H
