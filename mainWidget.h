#ifndef _KU_MAINWIDGET_H
#define _KU_MAINWIDGET_H

#include <qevent.h>
#include <qpixmap.h>

#include <kmainwindow.h>
#include <kaction.h>
#include <kstatusbar.h>

#include "mainView.h"

class mainWidget : public KMainWindow {
Q_OBJECT
public:
  mainWidget(const char *name = 0);
  ~mainWidget();

  bool queryClose();

protected:
  virtual void resizeEvent (QResizeEvent *event);
  
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
  KToggleAction *mActionToolbar;
  KToggleAction *mActionStatusbar;
  mainView *md;
};

#endif // _KU_MAINWIDGET_H
