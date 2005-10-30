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

#include "delUser.h"

#include <qlabel.h>
#include <qvbox.h>
#include <qfile.h>

#include <klocale.h>

delUser::delUser(KU::KUser *AUser, QWidget *parent, const char *name) 
 : KDialogBase( parent, name, true, i18n("Delete User"),
                KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true )
{                   
  QVBox *page = makeVBoxMainWidget();
  new QLabel( i18n("<p>Deleting user <b>%1</b>"
                   "<p>Also perform the following actions:").arg(AUser->getName()),
              page);
  m_deleteHomeDir = new QCheckBox(i18n("Delete &home folder: %1").arg(AUser->getHomeDir()), page);
  QString mailboxpath = QFile::decodeName(MAIL_SPOOL_DIR) + "/" + AUser->getName();
  m_deleteMailBox = new QCheckBox(i18n("Delete &mailbox: %1").arg(mailboxpath), page);
  setButtonGuiItem(KDialogBase::Ok, KStdGuiItem::del());
}

#include "delUser.moc"
