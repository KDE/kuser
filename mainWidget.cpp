#include "globals.h"

#include <qtooltip.h>

#include <ktoolbar.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kaction.h>
#include <stdio.h>
#include <kstdaction.h>

#include "mainWidget.h"
#include "misc.h"

mainWidget::mainWidget(const char *name) : KMainWindow(0,name) {
  md = new mainView(this);

  setupActions();
  md->slotTabChanged();

  sbar = new KStatusBar(this);
  sbar->insertItem("Reading config", 0);
  
  setCentralWidget(md);

  resize(500, 400);
  readSettings();
  sbar->changeItem(i18n("Ready"), 0);
}

mainWidget::~mainWidget() 
{
  delete md;
  delete sbar;
}

bool mainWidget::queryClose()
{
  return md->queryClose();
}

void mainWidget::setupActions() {
  KStdAction::save(md, SLOT(save()), actionCollection());
  KStdAction::quit(this, SLOT(close()), actionCollection());

  KStdAction::preferences(md, SLOT(properties()), actionCollection());
  mActionToolbar = KStdAction::showToolbar(this, SLOT(toggleToolBar()), actionCollection());
  mActionStatusbar = KStdAction::showStatusbar(this, SLOT(toggleStatusBar()), actionCollection());
//  KStdAction::saveOptions(md, SLOT(writeSettings()), actionCollection());

  KAction *action;

  (void)new KAction(i18n("&Add..."), QIconSet(BarIcon("add_user")), 0, md,
    SLOT(useradd()), actionCollection(), "add_user");

  action = new KAction(i18n("&Edit..."), QIconSet(BarIcon("edit_user")), 0, md,
    SLOT(useredit()), actionCollection(), "edit_user");
  connect(md, SIGNAL(userSelected(bool)), action, SLOT(setEnabled(bool)));

  action = new KAction(i18n("&Delete..."), QIconSet(BarIcon("delete_user")), 0, md,
    SLOT(userdel()), actionCollection(), "delete_user");
  connect(md, SIGNAL(userSelected(bool)), action, SLOT(setEnabled(bool)));

  (void)new KAction(i18n("&Set Password..."), QIconSet(BarIcon("set_password_user")),
    0, md, SLOT(setpwd()), actionCollection(), "set_password_user");

  (void)new KAction(i18n("&Add..."), QIconSet(BarIcon("add_group")), 0, md,
    SLOT(grpadd()), actionCollection(), "add_group");

  action = new KAction(i18n("&Edit..."), QIconSet(BarIcon("edit_group")), 0, md,
    SLOT(grpedit()), actionCollection(), "edit_group");
  connect(md, SIGNAL(groupSelected(bool)), action, SLOT(setEnabled(bool)));

  action = new KAction(i18n("&Delete..."), QIconSet(BarIcon("delete_group")), 0, md,
    SLOT(grpdel()), actionCollection(), "delete_group");
  connect(md, SIGNAL(groupSelected(bool)), action, SLOT(setEnabled(bool)));

  createGUI("kuserui.rc");
}

void mainWidget::readSettings() {
  // restore geometry settings
  KConfig *config = kapp->config();
  config->setGroup( "Appearance" );
  QString geom = config->readEntry("Geometry");
  if (!geom.isEmpty()) {
    int width, height;
    // #### use readSizeEntry instead
    sscanf(geom.ascii(), "%dx%d", &width, &height);
    resize(width, height);
  }
}

void mainWidget::writeSettings() {
  // save size of the application window
  KConfig *config = kapp->config();
  config->setGroup("Appearance");
  QString geom;
  // #### use readSizeEntry instead
  geom = QString("%1x%2").arg(geometry().width()).arg(geometry().height());
  config->writeEntry("Geometry", geom);
}

void mainWidget::resizeEvent(QResizeEvent *) {
  writeSettings();
}

void mainWidget::toggleToolBar() {
  if (mActionToolbar->isChecked())
    toolBar("mainToolBar")->show();
  else
    toolBar("mainToolBar")->hide();
}

void mainWidget::toggleStatusBar() {
  if (mActionStatusbar->isChecked())
    statusBar()->show();
  else
    statusBar()->hide();
}

#include "mainWidget.moc"
