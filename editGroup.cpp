/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Maintained by Adriaan de Groot <groot@kde.org>
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


#include <qvbox.h>
#include <qwhatsthis.h>

#include <kapplication.h>
#include <klocale.h>

#include "kglobal_.h"
#include "editGroup.h"



editGroup::editGroup(KGroup *akg, QWidget* parent, const char* name)
: KDialogBase(parent, name, true, i18n("Group Properties"),
	Ok | Cancel, Ok, true),
  kg(akg)
{
  QVBox *page = makeVBoxMainWidget();

  m_list = new KListView(page);
  m_list->setFullWidth(true); // Single column, full widget width.
  m_list->addColumn(i18n("Users in Group '%1'").arg(kg->getName()));
  QString whatstr = i18n("Select the users that should be in this group.");
  QWhatsThis::add(m_list, whatstr);
  connect(this,SIGNAL(okClicked(void)),
          this,SLOT(okClicked()));

  for (unsigned int i = 0; i<kug->getUsers().count(); i++) {
    QString userName = kug->getUsers()[i]->getName();
    QCheckListItem *item = new QCheckListItem(m_list, userName, QCheckListItem::CheckBox);
    if (kg->lookup_user(userName))
      item->setOn(true);
  }
}

editGroup::~editGroup() {
}

void editGroup::okClicked() {
  kg->clear();

  QCheckListItem *item = (QCheckListItem *) m_list->firstChild();
  while(item)
  {
     if (item->isOn())
        kg->addUser(item->text());

     item = (QCheckListItem *) item->nextSibling();
  }
}

#include "editGroup.moc"
