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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include <qpixmap.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qgrid.h>

#include <kapplication.h>
#include <ktabwidget.h>
#include <klocale.h>
#include <kcombobox.h>
#include <kabc/ldapconfigwidget.h>

#include "editDefaults.h"
#include "generalsettings.h"
#include "filessettings.h"
#include "ldapsettings.h"
#include "misc.h"

editDefaults::editDefaults( KConfigSkeleton *config, QWidget *parent, const char *name ) :
  KConfigDialog( parent, name, config, IconList,
  Default|Ok|Apply|Cancel|Help, Ok, true )
{
  GeneralSettings *page1 = new GeneralSettings( this );
  addPage( page1, i18n("General"), "", i18n("General settings") );
  page1->kcfg_shell->insertItem( i18n("<Empty>" ) );
  page1->kcfg_shell->insertStringList( readShells() );
    
  FilesSettings *page2 = new FilesSettings( this );
  addPage( page2, i18n("Files"), "", i18n("File source settings") );
    
  KTabWidget *page3 = new KTabWidget( this );
  page3->setMargin( KDialog::marginHint() );
  KABC::LdapConfigWidget *ldconf = 
    new KABC::LdapConfigWidget( KABC::LdapConfigWidget::W_ALL &
    ~(KABC::LdapConfigWidget::W_FILTER), page3 );
  page3->addTab( ldconf, i18n("Connection") );

  LdapSettings *page3b = new LdapSettings( this );
  page3->addTab( page3b, i18n("Settings") );
  addPage( page3, i18n("LDAP"), "", i18n("LDAP source settings") );
}
#include "editDefaults.moc"
