/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
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

#include "ku_pwdlg.h"

#include <QLabel>
#include <QGridLayout>

#include <kmessagebox.h>
#include <klocale.h>

KU_PwDlg::KU_PwDlg( QWidget* parent )
  : KDialog(parent)
{
  setCaption(i18n("Enter Password"));
  setButtons(Ok | Cancel);
  setDefaultButton(Ok);
  QFrame *page = new QFrame( this );
  setMainWidget( page );
  QLabel* lb1 = new QLabel(page);
  lb1->setText(i18n("Password:"));
  lb1->setMinimumSize(lb1->sizeHint());
  lb1->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  lepw1 = new KLineEdit(page);

  // ensure it fits at least 12 characters
  lepw1->setText( QLatin1String( "XXXXXXXXXXXX" ) );
  lepw1->setMinimumSize(lepw1->sizeHint());

  // clear text
  lepw1->clear();
  lepw1->setFocus();
  lepw1->setEchoMode(KLineEdit::Password);

  QLabel* lb2 = new QLabel(page);
  lb2->setText(i18n("Verify:"));
  lb2->setMinimumSize(lb2->sizeHint());
  lb2->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  lepw2 = new KLineEdit(page);

  // ensure it fits at least 12 characters
  lepw2->setText( QLatin1String( "XXXXXXXXXXXX" ) );
  lepw2->setMinimumSize(lepw2->sizeHint());

  // clear text
  lepw2->clear();
  lepw2->setEchoMode(KLineEdit::Password);

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(lb1, 0, 0);
  layout->addWidget(lepw1, 0, 1);
  layout->addWidget(lb2, 1, 0);
  layout->addWidget(lepw2, 1, 1);
  page->setLayout(layout);
}

KU_PwDlg::~KU_PwDlg()
{
  delete lepw1;
  delete lepw2;
}

void KU_PwDlg::accept()
{
  if ( lepw1->text() != lepw2->text() ) {
    KMessageBox::sorry( 0, i18n("Passwords are not identical.\nTry again.") );
    lepw1->clear();
    lepw2->clear();
    lepw1->setFocus();
  } else {
    KDialog::accept();
  }
}

QString KU_PwDlg::getPassword() const
{
  return lepw1->text();
}

#include "ku_pwdlg.moc"
