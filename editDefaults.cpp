#include <qpixmap.h>
#include <qlayout.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qtabbar.h>

#include <kapp.h>
#include <ktabctl.h>

#include "editDefaults.h"
#include <klocale.h>

editDefaults::editDefaults(QWidget* parent, const QString &name)
  : QTabDialog(parent, name, TRUE, 184320) {
  page1 = new userDefaultsPage(this, "user_page");
  addTab(page1, i18n("User"));
  resize(330, 300);
  setOKButton(i18n("&OK"));
  setCancelButton(i18n("&Cancel"));
}


editDefaults::~editDefaults() {
}

QString editDefaults::getShell() const {
  return page1->getShell();
}

QString editDefaults::getHomeBase() const {
  return page1->getHomeBase();
}

bool editDefaults::getCreateHomeDir() const {
  return page1->getCreateHomeDir();
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

void editDefaults::setHomeBase(const QString &data) {
  page1->setHomeBase(data);
}

void editDefaults::setCreateHomeDir(bool data) {
  page1->setCreateHomeDir(data);
}

void editDefaults::setCopySkel(bool data) {
  page1->setCopySkel(data);
}

void editDefaults::setUserPrivateGroup(bool data) {
  page1->setUserPrivateGroup(data);
}
