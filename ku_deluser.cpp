/*
 *  Copyright (c) 2002 Waldo Bastian <bastian@kde.org>
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

#include "globals.h"

#include <QLabel>
#include <QFile>

#include <kvbox.h>
#include <klocale.h>

#include "ku_deluser.h"

KU_DelUser::KU_DelUser(KU_User *AUser, QWidget *parent) 
 : KDialog( parent, i18n("Delete User"),
                KDialog::Ok|KDialog::Cancel )
{                   
  KVBox *page = new KVBox( this );
  setMainWidget( page );
  new QLabel( i18n("<p>Deleting user <b>%1</b>"
                   "<p>Also perform the following actions:").arg(AUser->getName()),
              page);
  m_deleteHomeDir = new QCheckBox(i18n("Delete &home folder: %1").arg(AUser->getHomeDir()), page);
  QString mailboxpath = QFile::decodeName(MAIL_SPOOL_DIR) + "/" + AUser->getName();
  m_deleteMailBox = new QCheckBox(i18n("Delete &mailbox: %1").arg(mailboxpath), page);
  setButtonGuiItem(KDialog::Ok, KStdGuiItem::del());
}

#include "ku_deluser.moc"
