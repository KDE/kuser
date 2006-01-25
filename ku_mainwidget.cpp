/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi GyĂśrgy <gyurco@freemail.hu>
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
#include <kxmlguifactory.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <kaction.h>
#include <klocale.h>
#include <kstdaction.h>

#include <kdebug.h>
#include <kaction.h>
#include <kstatusbar.h>
#include <kedittoolbar.h>

#include "ku_global.h"
#include "ku_configdlg.h"
#include "ku_mainwidget.h"
#include "ku_selectconn.h"
#include "ku_mainview.h"

KU_MainWidget::KU_MainWidget(const char *name) : KMainWindow(0,name)
{
  mv = new KU_MainView(this);

  setupActions();
  mv->setShowSys( mShowSys->isChecked() );
  init();
  mv->slotTabChanged();

  statusBar()->insertItem(i18n("Reading configuration"), 0);

  setCentralWidget(mv);

  setupGUI();

  statusBar()->changeItem(i18n("Ready"), 0);
}

KU_MainWidget::~KU_MainWidget()
{
  delete mv;
}

bool KU_MainWidget::queryClose()
{
  return mv->queryClose();
}

void KU_MainWidget::setupActions()
{
  KStdAction::quit(this, SLOT(close()), actionCollection());
  KStdAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());

  KStdAction::preferences(this, SLOT(properties()), actionCollection());

#define BarIconC(x)	BarIcon(QString::fromLatin1(x))

  (void) new KAction(i18n("&Add..."), QIcon(BarIconC("add_user")), 0, mv,
    SLOT(useradd()), actionCollection(), "add_user");

  (void) new KAction(i18n("&Edit..."), QIcon(BarIconC("edit_user")), 0, mv,
    SLOT(useredit()), actionCollection(), "edit_user");

  (void) new KAction(i18n("&Delete..."), QIcon(BarIconC("delete_user")), 0, mv,
    SLOT(userdel()), actionCollection(), "delete_user");

  (void) new KAction(i18n("&Set Password..."),
    0, mv, SLOT(setpwd()), actionCollection(), "set_password_user");

  (void) new KAction(i18n("&Add..."), QIcon(BarIconC("add_group")), 0, mv,
    SLOT(grpadd()), actionCollection(), "add_group");

  (void) new KAction(i18n("&Edit..."), QIcon(BarIconC("edit_group")), 0, mv,
    SLOT(grpedit()), actionCollection(), "edit_group");

  (void) new KAction(i18n("&Delete"), QIcon(BarIconC("delete_group")), 0, mv,
    SLOT(grpdel()), actionCollection(), "delete_group");

  (void) new KAction(i18n("&Reload"), QIcon(BarIconC("reload")), 0, this,
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

void KU_MainWidget::showSys()
{
  kug->kcfg()->setShowsys( mShowSys->isChecked() );
  mv->setShowSys( mShowSys->isChecked() );
  mv->reloadUsers();
  mv->reloadGroups();
}

void KU_MainWidget::properties()
{
  KU_ConfigDlg *configdlg = new KU_ConfigDlg( kug->kcfg(), this );
  connect(configdlg, SIGNAL(settingsChanged()), this, SLOT(slotApplySettings()));

  configdlg->show();
}

void KU_MainWidget::reload()
{
  init();
}

void KU_MainWidget::init()
{
  bool rw;

  mv->clearUsers();
  mv->clearGroups();
  kug->init();
  rw = ! ( kug->getUsers()->getCaps() & KU_Users::Cap_ReadOnly );
  kdDebug() << "Users rw()" << rw << endl;
  actionCollection()->action("add_user")->setEnabled( rw );
  actionCollection()->action("edit_user")->setEnabled( rw );
  actionCollection()->action("delete_user")->setEnabled( rw );
  actionCollection()->action("set_password_user")->setEnabled( rw );
  if ( rw ) {
    connect( mv, SIGNAL(userSelected(bool)),
      actionCollection()->action("edit_user"), SLOT(setEnabled(bool)) );
    connect( mv, SIGNAL(userSelected(bool)),
      actionCollection()->action("delete_user"), SLOT(setEnabled(bool)) );
    connect( mv, SIGNAL(userSelected(bool)),
      actionCollection()->action("set_password_user"), SLOT(setEnabled(bool)) );
  } else {
    disconnect( mv, SIGNAL(userSelected(bool)), 0, 0 );
  }

  rw = ! ( kug->getGroups()->getCaps() & KU_Groups::Cap_ReadOnly );
  kdDebug() << "Groups rw()" << rw << endl;
  actionCollection()->action("add_group")->setEnabled( rw );
  actionCollection()->action("edit_group")->setEnabled( rw );
  actionCollection()->action("delete_group")->setEnabled( rw );
  if ( rw ) {
    connect( mv, SIGNAL(groupSelected(bool)),
      actionCollection()->action("edit_group"), SLOT(setEnabled(bool)) );
    connect( mv, SIGNAL(groupSelected(bool)),
      actionCollection()->action("delete_group"), SLOT(setEnabled(bool)) );
  } else {
    disconnect( mv, SIGNAL(groupSelected(bool)), 0, 0 );
  }
  mv->reloadUsers();
  mv->reloadGroups();
  QTimer::singleShot( 0, mv, SLOT(slotTabChanged()) );
}

void KU_MainWidget::slotApplySettings()
{
  kdDebug() << "settings changed!" << endl;
  init();
}

void KU_MainWidget::slotApplyConnection()
{
  kdDebug() << "slotApplyConnection()" << endl;
  QString conn = sc->connSelected();
  kug->kcfg()->setConnection( conn );
  kug->initCfg( conn );
  slotApplySettings();
}

void KU_MainWidget::selectconn()
{
  sc = new KU_SelectConn( kug->kcfg()->connection(), this );
  connect( sc, SIGNAL(applyClicked()), SLOT(slotApplyConnection()) );
  connect( sc, SIGNAL(okClicked()), SLOT(slotApplyConnection()) );
  sc->show();
}

#include "ku_mainwidget.moc"
