/*
 *  Copyright (c) 2002 Waldo Bastian <bastian@kde.org>
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

#include "ku_deluser.h"

#include "globals.h"

#include <QLabel>
#include <QFile>

#include <kvbox.h>
#include <klocale.h>
#include <KStandardGuiItem>

KU_DelUser::KU_DelUser(KU_User *AUser, QWidget *parent) 
 : KDialog( parent)
{ 
  setCaption(i18n("Delete User"));
  setButtons(KDialog::Ok|KDialog::Cancel );
  setDefaultButton(KDialog::Ok);  
  KVBox *page = new KVBox( this );
  setMainWidget( page );
  new QLabel( i18n("<p>Deleting user <b>%1</b>"
                   "<br />Also perform the following actions:</p>", AUser->getName()),
              page);
  m_deleteHomeDir = new QCheckBox(i18n("Delete &home folder: %1", AUser->getHomeDir()), page);
  QString mailboxpath = QFile::decodeName(MAIL_SPOOL_DIR) + '/' + AUser->getName();
  m_deleteMailBox = new QCheckBox(i18n("Delete &mailbox: %1", mailboxpath), page);
  setButtonGuiItem(KDialog::Ok, KStandardGuiItem::del());
}

#include "ku_deluser.moc"
