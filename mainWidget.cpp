#include "maindlg.h"

#include "globals.h"
#include <qtooltip.h>
#include <ktoolbar.h>
#include <kglobal.h>
#include <kiconloader.h>

#include "misc.h"

#include "mainWidget.h"

mainWidget::mainWidget(const char *name) : KTMainWindow(name) {
  md = new mainDlg(this);
  md->init();

  QPopupMenu *file = new QPopupMenu;
  CHECK_PTR( file );
  file->insertItem(i18n("&Save"),  md, SLOT(save()) );
  file->insertSeparator();
  file->insertItem(i18n("&Preferences..."),  md, SLOT(properties()) );
  file->insertSeparator();
  file->insertItem(i18n("&Quit"),  md, SLOT(quit()) );

  QPopupMenu *user = new QPopupMenu;
  CHECK_PTR(user);
  user->insertItem(i18n("&Edit..."), md, SLOT(useredit()) );
  user->insertItem(i18n("&Delete..."), md, SLOT(userdel()) );
  user->insertItem(i18n("&Add..."), md, SLOT(useradd()) );
  user->insertItem(i18n("&Set password..."), md, SLOT(setpwd()) );

  QPopupMenu *group = new QPopupMenu;
  CHECK_PTR(group);
  group->insertItem(i18n("&Edit..."), md, SLOT(grpedit()) );
  group->insertItem(i18n("&Delete..."), md, SLOT(grpdel()) );
  group->insertItem(i18n("&Add..."), md, SLOT(grpadd()) );

  QString tmp;
  tmp = i18n("KUser version %1\n"
		"KDE project\n"
		"This program was created by\n"
		"Denis Pershin\n"
		"dyp@inetlab.com\n"
		"Copyright 1997, 1998, 1999 (c)").arg(_KU_VERSION);
  QPopupMenu *help = kapp->getHelpMenu(true, tmp);

  menubar = new KMenuBar( this );
  CHECK_PTR( menubar );
  menubar->insertItem(i18n("&File"), file );
  menubar->insertItem(i18n("&User"), user );
  menubar->insertItem(i18n("&Group"), group );
  menubar->insertSeparator();
  menubar->insertItem(i18n("&Help"), help );

  setMenu(menubar);

  toolbar = new KToolBar(this, "toolbar");

  toolbar->insertButton(BarIcon("useradd.xpm"), 0, SIGNAL(clicked()), md, SLOT(useradd()), TRUE, i18n("Add user"));
  toolbar->insertButton(BarIcon("userdel.xpm"), 0, SIGNAL(clicked()), md, SLOT(userdel()), TRUE, i18n("Delete user"));
  toolbar->insertButton(BarIcon("useredit.xpm"), 0, SIGNAL(clicked()), md, SLOT(useredit()), TRUE, i18n("Edit user"));
  
  toolbar->insertSeparator();
  
  toolbar->insertButton(BarIcon("grpadd.xpm"), 0, SIGNAL(clicked()), md, SLOT(grpadd()), TRUE, i18n("Add group"));
  toolbar->insertButton(BarIcon("grpdel.xpm"), 0, SIGNAL(clicked()), md, SLOT(grpdel()), TRUE, i18n("Delete group"));
  toolbar->insertButton(BarIcon("grpedit.xpm"), 0, SIGNAL(clicked()), md, SLOT(grpedit()), TRUE, i18n("Edit group"));
  toolbar->setBarPos(KToolBar::Top);

  addToolBar(toolbar);
  
  sbar = new KStatusBar(this);
  sbar->insertItem("Reading config", 0);
  
  setStatusBar(sbar);

  setView(md);

  resize(500, 400);

  // restore geometry settings
  KConfig *config = kapp->getConfig();
  config->setGroup( "Appearance" );
  QString geom = config->readEntry( "Geometry" );
  if (!geom.isEmpty()) {
    int width, height;
    sscanf( geom, "%dx%d", &width, &height );
    resize( width, height );
  }
  sbar->changeItem(i18n("Ready"), 0);
}

mainWidget::~mainWidget() {
  if (md)
    delete md;

  if (sbar)
    delete sbar;

  if (toolbar)
    delete toolbar;

  if (menubar)
    delete menubar;
}

void mainWidget::resizeEvent (QResizeEvent *) {
  // save size of the application window
  KConfig *config = kapp->getConfig();
  config->setGroup( "Appearance" );
  QString geom;
  geom = QString( "%1x%2").arg(geometry().width()).arg(geometry().height());
  config->writeEntry( "Geometry", geom );
  updateRects();
}

void mainWidget::closeEvent (QCloseEvent *) {
  md->quit();
}
