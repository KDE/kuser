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
  user->insertItem(_("Edit..."), md, SLOT(edit()) );
  user->insertItem(_("Delete..."), md, SLOT(del()) );
  user->insertItem(_("Add..."), md, SLOT(add()) );
  user->insertItem(_("Set password..."), md, SLOT(setpwd()) );

  QPopupMenu *help = new QPopupMenu;
  CHECK_PTR( help );
  help->insertItem(_("Help"), md, SLOT(help()));
  help->insertSeparator();
  help->insertItem(_("About..."), md, SLOT(about()));

  menubar = new KMenuBar( this );
  CHECK_PTR( menubar );
  menubar->insertItem(_("File"), file );
  menubar->insertItem(_("User"), user );
  menubar->insertSeparator();
  menubar->insertItem(_("Help"), help );

  setMenu(menubar);

  toolbar = new KToolBar(this, "toolbar");
  QPixmap pix;

  pix = kapp->getIconLoader()->loadIcon("useradd.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(add()), TRUE, _("Add user"));
  pix = kapp->getIconLoader()->loadIcon("userdel.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(del()), TRUE, _("Delete user"));
  pix = kapp->getIconLoader()->loadIcon("useredit.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(edit()), TRUE, _("Edit user"));
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

