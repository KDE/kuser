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



#include <kapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include "misc.h"
#include "globals.h"
#include "userDefaultsPage.h"
#include <klocale.h>
#include <stdio.h>

userDefaultsPage::userDefaultsPage(
	QWidget* parent,
	const char* name
)
	:
	QWidget( parent, name) {
  QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);

  QLabel *l;
  l = new QLabel(i18n("Shell:"), this);
  l->setFixedSize(l->sizeHint());
  tl->addWidget(l, 0, AlignLeft);
  shell = new QComboBox(this);  

  QStringList shells = readShells();
  
  shell->clear();
  shell->insertItem(i18n("<Empty>"));
  shell->insertStringList(shells);

  shell->setMinimumSize(shell->sizeHint());
  tl->addWidget(shell, 0, AlignLeft);

  tl->addStretch(1);

  l = new QLabel(i18n("Base of home folders:"), this);
  l->setFixedSize(l->sizeHint());
  tl->addWidget(l, 0, AlignLeft);
  home=new QLineEdit(this);
  home->setFixedSize(home->sizeHint());
  tl->addWidget(home, 0, AlignLeft);

  tl->addStretch(2);

  createHomeDir = new QCheckBox(i18n("Create home folder"), this);
  createHomeDir->setFixedSize(createHomeDir->sizeHint());
  tl->addWidget(createHomeDir, 0, AlignLeft);

  copySkel = new QCheckBox(i18n("Copy skeleton to home folder"), this);
  copySkel->setFixedSize(copySkel->sizeHint());
  tl->addWidget(copySkel, 0, AlignLeft);

  userPrivateGroup = new QCheckBox(i18n("User private group"), this);
  userPrivateGroup->setFixedSize(userPrivateGroup->sizeHint());
  tl->addWidget(userPrivateGroup, 0, AlignLeft);
  tl->activate();
}


userDefaultsPage::~userDefaultsPage() {
}

QString userDefaultsPage::getShell() const {
  return shell->currentText();
}

QString userDefaultsPage::getHomeBase() const {
  return home->text();
}

bool userDefaultsPage::getCreateHomeDir() const {
  return createHomeDir->isChecked();
}

bool userDefaultsPage::getCopySkel() const {
  return copySkel->isChecked();
}

bool userDefaultsPage::getUserPrivateGroup() const {
  return userPrivateGroup->isChecked();
}

void userDefaultsPage::setShell(const QString & data) {
  for (int i=0; i<shell->count(); i++)
    if (shell->text(i) == data) {
      shell->setCurrentItem(i);
      return;
    }
  
  shell->insertItem(data, 1);
  shell->setCurrentItem(1);
}

void userDefaultsPage::setHomeBase(const QString & data) {
  home->setText(data);
}

void userDefaultsPage::setCreateHomeDir(bool data) {
  createHomeDir->setChecked(data);
}

void userDefaultsPage::setCopySkel(bool data) {
  copySkel->setChecked(data);
}

void userDefaultsPage::setUserPrivateGroup(bool data) {
  userPrivateGroup->setChecked(data);
}
#include "userDefaultsPage.moc"
