/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi GyĂśrgy <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 or at your option version 3 as published by
 *  the Free Software Foundation.
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

#include <qtimer.h>
#include <kxmlguifactory.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <kaction.h>
#include <klocale.h>
#include <kstandardaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <kdebug.h>
#include <kstatusbar.h>
#include <kedittoolbar.h>
#include <kicon.h>

#include "ku_global.h"
#include "ku_configdlg.h"
#include "ku_mainwidget.h"
#include "ku_selectconn.h"
#include "ku_mainview.h"

KU_MainWidget::KU_MainWidget() : KXmlGuiWindow(0)
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
  KStandardAction::quit(this, SLOT(close()), actionCollection());
  KStandardAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());

  KStandardAction::preferences(this, SLOT(properties()), actionCollection());
  QAction *action;

  action  = new KAction(KIcon( QLatin1String( "list-add-user") ), i18n("&Add..." ), this);
  actionCollection()->addAction( QLatin1String( "add_user" ), action );
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(useradd()) );

  action  = new KAction(KIcon( QLatin1String( "user-properties") ), i18n("&Edit..." ), this);
  actionCollection()->addAction( QLatin1String( "edit_user" ), action );
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(useredit()) );

  action  = new KAction(KIcon( QLatin1String( "list-remove-user") ), i18n("&Delete..." ), this);
  actionCollection()->addAction( QLatin1String( "delete_user" ), action );
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(userdel()) );

  action  = new KAction(KIcon( QLatin1String( "preferences-desktop-user-password") ), i18n("&Set Password..." ), this);
  actionCollection()->addAction( QLatin1String( "set_password_user" ), action );
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(setpwd()) );

  action  = new KAction(KIcon( QLatin1String( "user-group-new") ) , i18n("&Add..." ), this);
  actionCollection()->addAction( QLatin1String( "add_group" ), action );
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(grpadd()) );

  action  = new KAction(KIcon( QLatin1String( "user-group-properties") ), i18n("&Edit..." ), this);
  actionCollection()->addAction( QLatin1String( "edit_group" ), action );
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(grpedit()) );

  action  = new KAction(KIcon( QLatin1String( "user-group-delete") ), i18n("&Delete..." ), this);
  actionCollection()->addAction( QLatin1String( "delete_group" ), action );
  connect( action, SIGNAL(triggered(bool)), mv, SLOT(grpdel()) );

  action  = new KAction(KIcon( QLatin1String( "view-refresh") ), i18n("&Reload..." ), this);
  actionCollection()->addAction( QLatin1String( "reload" ), action );
  connect( action, SIGNAL(triggered(bool)), this, SLOT(reload()) );

  action  = new KAction(i18n("&Select Connection..."), this);
  actionCollection()->addAction( QLatin1String( "select_conn" ), action );
  connect( action, SIGNAL(triggered(bool)), this, SLOT(selectconn()) );

  mShowSys  = new KToggleAction(i18n("Show System Users/Groups"), this);
  actionCollection()->addAction( QLatin1String( "show_sys" ), mShowSys );
  connect( mShowSys, SIGNAL(triggered(bool)), this, SLOT(showSys(bool)) );

  mShowSys->setChecked( KU_Global::kcfg()->showsys() );
}

void KU_MainWidget::showSys( bool show )
{
  KU_Global::kcfg()->setShowsys( show );
  mv->setShowSys( show );
  mv->reloadUsers();
  mv->reloadGroups();
}

void KU_MainWidget::properties()
{
  KU_ConfigDlg *configdlg = new KU_ConfigDlg( KU_Global::kcfg(), this );
  connect(configdlg, SIGNAL(settingsChanged(QString)), this, SLOT(slotApplySettings()));

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
  KU_Global::init();
  rw = ! ( KU_Global::users()->getCaps() & KU_Users::Cap_ReadOnly );
  kDebug() << "Users rw()" << rw;
  actionCollection()->action(QLatin1String( "add_user" ))->setEnabled( rw );
  actionCollection()->action(QLatin1String( "edit_user" ))->setEnabled( rw );
  actionCollection()->action(QLatin1String( "delete_user" ))->setEnabled( rw );
  actionCollection()->action(QLatin1String( "set_password_user" ))->setEnabled( rw );
  if ( rw ) {
    connect( mv, SIGNAL(userSelected(bool)),
      actionCollection()->action(QLatin1String( "edit_user" )), SLOT(setEnabled(bool)) );
    connect( mv, SIGNAL(userSelected(bool)),
      actionCollection()->action(QLatin1String( "delete_user" )), SLOT(setEnabled(bool)) );
    connect( mv, SIGNAL(userSelected(bool)),
      actionCollection()->action(QLatin1String( "set_password_user" )), SLOT(setEnabled(bool)) );
  } else {
    disconnect( mv, SIGNAL(userSelected(bool)), 0, 0 );
  }

  rw = ! ( KU_Global::groups()->getCaps() & KU_Groups::Cap_ReadOnly );
  kDebug() << "Groups rw()" << rw;
  actionCollection()->action(QLatin1String( "add_group" ))->setEnabled( rw );
  actionCollection()->action(QLatin1String( "edit_group" ))->setEnabled( rw );
  actionCollection()->action(QLatin1String( "delete_group" ))->setEnabled( rw );
  if ( rw ) {
    connect( mv, SIGNAL(groupSelected(bool)),
      actionCollection()->action(QLatin1String( "edit_group" )), SLOT(setEnabled(bool)) );
    connect( mv, SIGNAL(groupSelected(bool)),
      actionCollection()->action(QLatin1String( "delete_group" )), SLOT(setEnabled(bool)) );
  } else {
    disconnect( mv, SIGNAL(groupSelected(bool)), 0, 0 );
  }
  mv->reloadUsers();
  mv->reloadGroups();
  QTimer::singleShot( 0, mv, SLOT(slotTabChanged()) );
}

void KU_MainWidget::slotApplySettings()
{
  kDebug() << "settings changed!";
  init();
}

void KU_MainWidget::slotApplyConnection()
{
  kDebug() << "slotApplyConnection()";
  QString conn = sc->connSelected();
  KU_Global::kcfg()->setConnection( conn );
  KU_Global::initCfg( conn );
  slotApplySettings();
}

void KU_MainWidget::selectconn()
{
  sc = new KU_SelectConn( KU_Global::kcfg()->connection(), this );
  connect( sc, SIGNAL(applyClicked()), SLOT(slotApplyConnection()) );
  connect( sc, SIGNAL(okClicked()), SLOT(slotApplyConnection()) );
  sc->show();
}

#include "ku_mainwidget.moc"
