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

#include "globals.h"

#include <qstring.h>
#include <qvalidator.h>
#include <qgrid.h>
#include <kmessagebox.h>

#include "grpnamedlg.h"
#include "misc.h"
#include "kglobal_.h"

grpnamedlg::grpnamedlg(KGroup &AGroup, QWidget* parent, const char* name)
  : KDialogBase(parent, name, true, i18n("Add Group"), Ok | Cancel, Ok, true), 
    group(AGroup) 
{
  group.setGID(kug->getGroups().first_free());

  QGrid *page = makeGridMainWidget(2, QGrid::Horizontal);

  QLabel* lb1 = new QLabel(page, "lb1");
  lb1->setText(i18n("Group name:"));
  lb1->setMinimumSize(lb1->sizeHint());
  lb1->setAlignment(AlignRight|AlignVCenter);

  legrpname = new QLineEdit( page, "LineEdit_1" );
  // ensure it fits at least 20 characters
  legrpname->setText(QString::fromLatin1("XXXXXXXXXXXXXXXXXXX"));
  legrpname->setMinimumSize( legrpname->sizeHint() );

  // clear text
  legrpname->clear();
  legrpname->setFocus();
  lb1->setBuddy(legrpname);

  QLabel* lb2 = new QLabel(page, "lb2");
  lb2->setText(i18n("Group number:"));
  lb2->setMinimumSize(lb2->sizeHint());
  lb2->setAlignment(AlignRight|AlignVCenter);

  legid = new QLineEdit(page, "LineEdit_2");
  // ensure it fits at least 20 characters
  legid->setText(QString::fromLatin1("XXXXXXXXXXXXXXXXXXX"));
  legid->setMinimumSize(legid->sizeHint());
  
  // clear text
  legid->setText(QString::fromLatin1("%1").arg(group.getGID()));
  legid->setValidator(new QIntValidator(this, "val1"));
  lb2->setBuddy(legid);
}

grpnamedlg::~grpnamedlg() {
}

void grpnamedlg::slotOk()
{
  QString tmp;
  QString s;
  s = legid->text();

  if (kug->getGroups().lookup(legrpname->text())) {
    tmp = i18n("Group with name %1 already exists.").arg(legrpname->text());
    KMessageBox::sorry(0, tmp);
    return;
  }
  
  if (kug->getGroups().lookup(s.toInt())) {
    tmp = i18n("Group with gid %1 already exists.").arg(s.toInt());
    KMessageBox::sorry(0, tmp);
    return;
  }
  
  group.setName(legrpname->text());
  group.setGID(s.toInt());
  accept();
}

#include "grpnamedlg.moc"
