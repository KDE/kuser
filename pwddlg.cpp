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

#include <qgrid.h>

#include <kmessagebox.h>

#include "pwddlg.h"
#include "misc.h"

pwddlg::pwddlg( QWidget* parent, const char* name )
  : KDialogBase(parent, name, true, i18n("Enter Password"), Ok | Cancel, Ok, true)
{
  QGrid *page = makeGridMainWidget(2, QGrid::Horizontal);

  QLabel* lb1 = new QLabel(page, "lb1");
  lb1->setText(i18n("Password:"));
  lb1->setMinimumSize(lb1->sizeHint());
  lb1->setAlignment(AlignRight|AlignVCenter);

  lepw1 = new KLineEdit(page, "LineEdit_1");

  // ensure it fits at least 12 characters
  lepw1->setText( "XXXXXXXXXXXX" );
  lepw1->setMinimumSize(lepw1->sizeHint());

  // clear text
  lepw1->clear();
  lepw1->setFocus();
  lepw1->setEchoMode(KLineEdit::Password);

  QLabel* lb2 = new QLabel(page, "lb2");
  lb2->setText(i18n("Verify:"));
  lb2->setMinimumSize(lb2->sizeHint());
  lb2->setAlignment(AlignRight|AlignVCenter);

  lepw2 = new KLineEdit(page, "LineEdit_2");

  // ensure it fits at least 12 characters
  lepw2->setText( "XXXXXXXXXXXX" );
  lepw2->setMinimumSize(lepw2->sizeHint());

  // clear text
  lepw2->clear();
  lepw2->setEchoMode(KLineEdit::Password);
}

pwddlg::~pwddlg()
{
  delete lepw1;
  delete lepw2;
}

void pwddlg::slotOk()
{
  if ( lepw1->text() != lepw2->text() ) {
    KMessageBox::sorry( 0, i18n("Passwords are not identical.\nTry again.") );
    lepw1->clear();
    lepw2->clear();
    lepw1->setFocus();
  } else {
    accept();
  }
}

QString pwddlg::getPassword() const
{
  return lepw1->text();
}

#include "pwddlg.moc"
