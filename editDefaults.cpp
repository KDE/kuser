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

#include <qpixmap.h>
#include <qlayout.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qtabbar.h>

#include <kapplication.h>
#include <ktabctl.h>

#include "editDefaults.h"
#include <klocale.h>

editDefaults::editDefaults(QWidget* parent, const char * name)
  : QTabDialog(parent, name, TRUE, 184320) {
  page1 = new userDefaultsPage(this, "user_page");
  addTab(page1, i18n("User"));
  resize(330, 300);
  setOKButton(i18n("&OK"));
  setCancelButton(i18n("&Cancel"));
  page2 = new nisDefaultsPage(this, "nis_page");	
  addTab(page2, i18n("Sources"));				
  resize(330, 300);					
  setOKButton(i18n("&OK"));				
  setCancelButton(i18n("&Cancel"));			
}


editDefaults::~editDefaults() {
}

QString editDefaults::getShell() const {
  return page1->getShell();
}

QString editDefaults::getPasswdSrc() const {	
  return page2->getPasswdSrc();			
}						

QString editDefaults::getNISPasswdSrc() const {		
  return page2->getNISPasswdSrc();			
}							

QString editDefaults::getGroupSrc() const {	
  return page2->getGroupSrc();			
}						

QString editDefaults::getNISGroupSrc() const {	
  return page2->getNISGroupSrc();		
}						

QString editDefaults::getMINUID() const {	
  return page2->getMINUID();			
}						

QString editDefaults::getMINGID() const {	
  return page2->getMINGID();			
}						

QString editDefaults::getHomeBase() const {
  return page1->getHomeBase();
}

bool editDefaults::getCreateHomeDir() const {
  return page1->getCreateHomeDir();
}

bool editDefaults::getEnableNIS() const {	
  return page2->getEnableNIS();			
}						

bool editDefaults::getCopySkel() const {
  return page1->getCopySkel();
}

bool editDefaults::getUserPrivateGroup() const {
  return page1->getUserPrivateGroup();
}

void editDefaults::setShell(const QString &data) {
  page1->setShell(data);
}

void editDefaults::setPasswdSrc(const QString &data) {	
  page2->setPasswdSrc(data);				
}							

void editDefaults::setNISPasswdSrc(const QString &data) {	
  page2->setNISPasswdSrc(data);					
}								

void editDefaults::setGroupSrc(const QString &data) {	
  page2->setGroupSrc(data);				
}							

void editDefaults::setNISGroupSrc(const QString &data) {  
  page2->setNISGroupSrc(data);				
}							

void editDefaults::setMINUID(const QString &data) {  	
  page2->setMINUID(data);				
}							

void editDefaults::setMINGID(const QString &data) {  	
  page2->setMINGID(data);				
}							

void editDefaults::setHomeBase(const QString &data) {
  page1->setHomeBase(data);
}

void editDefaults::setCreateHomeDir(bool data) {
  page1->setCreateHomeDir(data);
}

void editDefaults::setEnableNIS(bool data) {	
  page2->setEnableNIS(data);			
}						

void editDefaults::sourcesHelp() {		
}						

void editDefaults::setCopySkel(bool data) {
  page1->setCopySkel(data);
}

void editDefaults::setUserPrivateGroup(bool data) {
  page1->setUserPrivateGroup(data);
}
#include "editDefaults.moc"
