#include "maindlg.h"
#include "mainWidget.moc"

#include "globals.h"
#include <qtooltip.h>
#include <kmsgbox.h>
#include <ktoolbar.h>
#include <kiconloader.h>

#include "misc.h"

mainWidget::mainWidget(const char *name) :
KTopLevelWidget(name)
{
  md = new mainDlg(this);
  md->init();

  QPopupMenu *file = new QPopupMenu;
  CHECK_PTR( file );
  file->insertItem(_("Properties..."),  md, SLOT(properties()) );
  file->insertSeparator();
  file->insertItem(_("Quit"),  md, SLOT(quit()) );

  QPopupMenu *user = new QPopupMenu;
  CHECK_PTR(user);
  user->insertItem(_("Edit..."), md, SLOT(useredit()) );
  user->insertItem(_("Delete..."), md, SLOT(userdel()) );
  user->insertItem(_("Add..."), md, SLOT(useradd()) );
  user->insertItem(_("Set password..."), md, SLOT(setpwd()) );

  QPopupMenu *group = new QPopupMenu;
  CHECK_PTR(group);
  group->insertItem(_("Edit..."), md, SLOT(grpedit()) );
  group->insertItem(_("Delete..."), md, SLOT(grpdel()) );
  group->insertItem(_("Add..."), md, SLOT(grpadd()) );

  QPopupMenu *help = new QPopupMenu;
  CHECK_PTR( help );
  help->insertItem(_("Help"), md, SLOT(help()));
  help->insertSeparator();
  help->insertItem(_("About..."), md, SLOT(about()));

  menubar = new KMenuBar( this );
  CHECK_PTR( menubar );
  menubar->insertItem(_("File"), file );
  menubar->insertItem(_("User"), user );
  menubar->insertItem(_("Group"), group );
  menubar->insertSeparator();
  menubar->insertItem(_("Help"), help );

  setMenu(menubar);

  toolbar = new KToolBar(this, "toolbar");
  QPixmap pix;

  pix = kapp->getIconLoader()->loadIcon("useradd.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(useradd()), TRUE, _("Add user"));
  pix = kapp->getIconLoader()->loadIcon("userdel.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(userdel()), TRUE, _("Delete user"));
  pix = kapp->getIconLoader()->loadIcon("useredit.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(useredit()), TRUE, _("Edit user"));
  
  toolbar->insertSeparator();
  
  pix = kapp->getIconLoader()->loadIcon("grpadd.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(grpadd()), TRUE, _("Add group"));
  pix = kapp->getIconLoader()->loadIcon("grpdel.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(grpdel()), TRUE, _("Delete group"));
  pix = kapp->getIconLoader()->loadIcon("grpedit.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(grpedit()), TRUE, _("Edit group"));
  toolbar->setBarPos(KToolBar::Top);

  addToolBar(toolbar);
  
  sbar = new KStatusBar(this);
  sbar->insertItem("Reading config", 0);
  
  setStatusBar(sbar);

  setView(md);

  resize(500, 400);

  // restore geometry settings
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup( "Appearance" );
  QString geom = config->readEntry( "Geometry" );
  if (!geom.isEmpty()) {
    int width, height;
    sscanf( geom, "%dx%d", &width, &height );
    resize( width, height );
  }
 }

mainWidget::~mainWidget() {
  delete md;
  delete sbar;
  delete toolbar;
  delete menubar;
}

void mainWidget::resizeEvent (QResizeEvent *) {
  // save size of the application window
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup( "Appearance" );
  QString geom;
  geom.sprintf( "%dx%d", geometry().width(), geometry().height() );
  config->writeEntry( "Geometry", geom );
  updateRects();
}

