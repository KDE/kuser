/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#include <qtooltip.h>
#include <qtimer.h>

#include <ktoolbar.h>
#include <kiconloader.h>
#include <kaction.h>
#include <klocale.h>
#include <kstdaction.h>

#include <kdebug.h>
#include <kaction.h>
#include <kstatusbar.h>
#include <kedittoolbar.h>

#include "kglobal_.h"
#include "editDefaults.h"
#include "mainWidget.h"
#include "selectconn.h"
#include "mainView.h"

mainWidget::mainWidget(const char *name) : KMainWindow(0,name)
{
  md = new mainView(this);

  setupActions();
  md->setShowSys( mShowSys->isChecked() );
  init();
  md->slotTabChanged();

  statusBar()->insertItem(i18n("Reading configuration"), 0);

  setCentralWidget(md);

  setupGUI();
	
  statusBar()->changeItem(i18n("Ready"), 0);
}

mainWidget::~mainWidget()
{
  delete md;
}

bool mainWidget::queryClose()
{
  return md->queryClose();
}

void mainWidget::setupActions()
{
  KStdAction::quit(this, SLOT(close()), actionCollection());
  KStdAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());

  KStdAction::preferences(this, SLOT(properties()), actionCollection());

#define BarIconC(x)	BarIcon(QString::fromLatin1(x))

  (void) new KAction(i18n("&Add..."), QIconSet(BarIconC("add_user")), 0, md,
    SLOT(useradd()), actionCollection(), "add_user");

  (void) new KAction(i18n("&Edit..."), QIconSet(BarIconC("edit_user")), 0, md,
    SLOT(useredit()), actionCollection(), "edit_user");

  (void) new KAction(i18n("&Delete..."), QIconSet(BarIconC("delete_user")), 0, md,
    SLOT(userdel()), actionCollection(), "delete_user");

  (void) new KAction(i18n("&Set Password..."),
    0, md, SLOT(setpwd()), actionCollection(), "set_password_user");

  (void) new KAction(i18n("&Add..."), QIconSet(BarIconC("add_group")), 0, md,
    SLOT(grpadd()), actionCollection(), "add_group");

  (void) new KAction(i18n("&Edit..."), QIconSet(BarIconC("edit_group")), 0, md,
    SLOT(grpedit()), actionCollection(), "edit_group");

  (void) new KAction(i18n("&Delete"), QIconSet(BarIconC("delete_group")), 0, md,
    SLOT(grpdel()), actionCollection(), "delete_group");

  (void) new KAction(i18n("&Reload"), QIconSet(BarIconC("reload")), 0, this,
    SLOT(reload()), actionCollection(), "reload");

#undef BarIconC
	
  (void) new KAction(i18n("&Select Connection..."),
    0, this,
    SLOT(selectconn()), actionCollection(), "select_conn");

  mShowSys = new KToggleAction(i18n("Show System Users/Groups"),
    0, 0, this,
    SLOT(showSys()), actionCollection(), "show_sys");
  mShowSys->setCheckedState(i18n("Hide System Users/Groups"));
  mShowSys->setChecked( kug->kcfg()->showsys() );
}

void mainWidget::showSys()
{
  kug->kcfg()->setShowsys( mShowSys->isChecked() );
  md->setShowSys( mShowSys->isChecked() );
  md->reloadUsers();
  md->reloadGroups();
}

void mainWidget::properties()
{
  editDefaults *eddlg = new editDefaults( kug->kcfg(), this );
  connect(eddlg, SIGNAL(settingsChanged()), this, SLOT(slotApplySettings()));

  eddlg->show();
}

void mainWidget::reload()
{
  init();
}

void mainWidget::init()
{
  bool rw;

  md->clearUsers();
  md->clearGroups();
  kug->init();
  rw = ! ( kug->getUsers().getCaps() & KU::KUsers::Cap_ReadOnly );
  kdDebug() << "Users rw()" << rw << endl;
  actionCollection()->action("add_user")->setEnabled( rw );
  actionCollection()->action("edit_user")->setEnabled( rw );
  actionCollection()->action("delete_user")->setEnabled( rw );
  actionCollection()->action("set_password_user")->setEnabled( rw );
  if ( rw ) {
    connect( md, SIGNAL(userSelected(bool)),
      actionCollection()->action("edit_user"), SLOT(setEnabled(bool)) );
    connect( md, SIGNAL(userSelected(bool)),
      actionCollection()->action("delete_user"), SLOT(setEnabled(bool)) );
    connect( md, SIGNAL(userSelected(bool)),
      actionCollection()->action("set_password_user"), SLOT(setEnabled(bool)) );
  } else {
    disconnect( md, SIGNAL(userSelected(bool)), 0, 0 );
  }

  rw = ! ( kug->getGroups().getCaps() & KU::KGroups::Cap_ReadOnly );
  kdDebug() << "Groups rw()" << rw << endl;
  actionCollection()->action("add_group")->setEnabled( rw );
  actionCollection()->action("edit_group")->setEnabled( rw );
  actionCollection()->action("delete_group")->setEnabled( rw );
  if ( rw ) {
    connect( md, SIGNAL(groupSelected(bool)),
      actionCollection()->action("edit_group"), SLOT(setEnabled(bool)) );
    connect( md, SIGNAL(groupSelected(bool)),
      actionCollection()->action("delete_group"), SLOT(setEnabled(bool)) );
  } else {
    disconnect( md, SIGNAL(groupSelected(bool)), 0, 0 );
  }
  md->reloadUsers();
  md->reloadGroups();
  QTimer::singleShot( 0, md, SLOT(slotTabChanged()) );
}

void mainWidget::slotApplySettings()
{
  kdDebug() << "settings changed!" << endl;
  init();
}

void mainWidget::slotApplyConnection()
{
  kdDebug() << "slotApplyConnection()" << endl;
  QString conn = sc->connSelected();
  kug->kcfg()->setConnection( conn );
  kug->initCfg( conn );
  slotApplySettings();
}

void mainWidget::selectconn()
{
  sc = new SelectConn( kug->kcfg()->connection(), this, "selectconn" );
  connect( sc, SIGNAL(applyClicked()), SLOT(slotApplyConnection()) );
  connect( sc, SIGNAL(okClicked()), SLOT(slotApplyConnection()) );
  sc->show();
}

#include "mainWidget.moc"
