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

#include <QToolTip>
#include <qtimer.h>
#include <kxmlguifactory.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <kaction.h>
#include <klocale.h>
#include <kstdaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
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
  KAction *action;

  action =  new KAction(KIcon("add_user"), i18n("&Add..."), actionCollection(), "add_user");
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(useradd()) );

  action =  new KAction(KIcon("edit_user"), i18n("&Edit..."), actionCollection(), "edit_user");
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(useredit()) );

  action =  new KAction(KIcon("delete_user"), i18n("&Delete..."), actionCollection(), "delete_user");
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(userdel()) );

  action =  new KAction(KIcon("set_password_user"), i18n("&Set Password..."), actionCollection(), "set_password_user");
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(setpwd()) );

  action =  new KAction(KIcon("add_group"), i18n("&Add..."), actionCollection(), "add_group");
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(grpadd()) );

  action =  new KAction(KIcon("edit_group"), i18n("&Edit..."), actionCollection(), "edit_group");
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(grpedit()) );

  action =  new KAction(KIcon("delete_group"), i18n("&Delete..."), actionCollection(), "delete_group");
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(grpdel()) );

  action =  new KAction(KIcon("reload"), i18n("&Reload..."), actionCollection(), "reload");
  connect( action, SIGNAL(triggered(bool)), this, SLOT(reload()) );

  action =  new KAction( i18n("&Select Connection..."), actionCollection(), "select_conn");
  connect( action, SIGNAL(triggered(bool)), this, SLOT(selectconn()) );

  mShowSys = new KToggleAction( i18n("Show System Users/Groups"), actionCollection(), "show_sys" );
  connect( mShowSys, SIGNAL(triggered(bool)), this, SLOT(showSys(bool)) );

  mShowSys->setCheckedState(i18n("Hide System Users/Groups"));
  mShowSys->setChecked( kug->kcfg()->showsys() );
}

void KU_MainWidget::showSys( bool show )
{
  kug->kcfg()->setShowsys( show );
  mv->setShowSys( show );
  mv->reloadUsers();
  mv->reloadGroups();
}

void KU_MainWidget::properties()
{
  KU_ConfigDlg *configdlg = new KU_ConfigDlg( kug->kcfg(), this );
  connect(configdlg, SIGNAL(settingsChanged(const QString&)), this, SLOT(slotApplySettings()));

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
  kDebug() << "Users rw()" << rw << endl;
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
  kDebug() << "Groups rw()" << rw << endl;
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
  kDebug() << "settings changed!" << endl;
  init();
}

void KU_MainWidget::slotApplyConnection()
{
  kDebug() << "slotApplyConnection()" << endl;
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
