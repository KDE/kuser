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

mainWidget::mainWidget(const char *name) : KTMainWindow(name) {
  md = new mainView(this);

  setupActions();

  sbar = new KStatusBar(this);
  sbar->insertItem("Reading config", 0);
  
  setStatusBar(sbar);

  setView(md);

  resize(500, 400);
  readSettings();
  sbar->changeItem(i18n("Ready"), 0);
}

mainWidget::~mainWidget() {
  if (md)
    delete md;

  if (sbar)
    delete sbar;
}

void mainWidget::setupActions() {
  KStdAction::save(md, SLOT(save()), actionCollection());
  KStdAction::quit(md, SLOT(quit()), actionCollection());

  KStdAction::preferences(md, SLOT(properties()), actionCollection());
  KStdAction::showToolbar(this, SLOT(toggleToolBar()), actionCollection());
  KStdAction::showStatusbar(this, SLOT(toggleStatusBar()), actionCollection());
  KStdAction::saveOptions(md, SLOT(save_options()), actionCollection());

  (void)new KAction(i18n("&Add..."), QIconSet(BarIcon("add_user")), 0, md,
    SLOT(useradd()), actionCollection(), "add_user");
  (void)new KAction(i18n("&Edit..."), QIconSet(BarIcon("edit_user")), 0, md,
    SLOT(useredit()), actionCollection(), "edit_user");
  (void)new KAction(i18n("&Delete..."), QIconSet(BarIcon("delete_user")), 0, md,
    SLOT(userdel()), actionCollection(), "delete_user");
  (void)new KAction(i18n("&Set password..."), QIconSet(BarIcon("set_password_user")),
    0, md, SLOT(setpwd()), actionCollection(), "set_password_user");

  (void)new KAction(i18n("&Add..."), QIconSet(BarIcon("add_user")), 0, md,
    SLOT(grpadd()), actionCollection(), "add_group");
  (void)new KAction(i18n("&Edit..."), QIconSet(BarIcon("edit_group")), 0, md,
    SLOT(grpedit()), actionCollection(), "edit_group");
  (void)new KAction(i18n("&Delete..."), QIconSet(BarIcon("delete_group")), 0, md,
    SLOT(grpdel()), actionCollection(), "delete_group");

  createGUI("kuserui.rc");
}

void mainWidget::readSettings() {
  // restore geometry settings
  KConfig *config = kapp->config();
  config->setGroup( "Appearance" );
  QString geom = config->readEntry("Geometry");
  if (!geom.isEmpty()) {
    int width, height;
    sscanf(geom, "%dx%d", &width, &height);
    resize(width, height);
  }
}

void mainWidget::writeSettings() {
  // save size of the application window
  KConfig *config = kapp->config();
  config->setGroup("Appearance");
  QString geom;
  geom = QString("%1x%2").arg(geometry().width()).arg(geometry().height());
  config->writeEntry("Geometry", geom);
}

void mainWidget::resizeEvent(QResizeEvent *) {
  writeSettings();
  updateRects();
}

void mainWidget::closeEvent(QCloseEvent *) {
  md->quit();
}

void mainWidget::toggleToolBar() {
  if (toolBar()->isVisible())
    toolBar()->hide();
  else
    toolBar()->show();
}

void mainWidget::toggleStatusBar() {
  if (statusBar()->isVisible())
    statusBar()->hide();
  else
    statusBar()->show();
}

