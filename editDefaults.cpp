#include <qpixmap.h>
#include <qlayout.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qtabbar.h>

#include <kapp.h>
#include <ktabctl.h>

#include "editDefaults.h"

editDefaults::editDefaults(QWidget* parent, const char* name)
  : QTabDialog(parent, name, TRUE, 184320) 
{
  page1 = new userDefaultsPage(this, "user_page");
  addTab(page1, i18n("User"));
  resize(330, 300);
  setCancelButton(i18n("Cancel"));
}


editDefaults::~editDefaults() {
}

const char *editDefaults::getShell() const {
  return page1->getShell();
}

const char *editDefaults::getHomeBase() const {
  return page1->getHomeBase();
}

bool editDefaults::getCreateHomeDir() const {
  return page1->getCreateHomeDir();
}

bool editDefaults::getCopySkel() const {
  return page1->getCopySkel();
}

bool editDefaults::getUsePrivateGroup() const {
  return page1->getUsePrivateGroup();
}

void editDefaults::setShell(const char *data) {
  page1->setShell(data);
}

void editDefaults::setHomeBase(const char *data) {
  page1->setHomeBase(data);
}

void editDefaults::setCreateHomeDir(bool data) {
  page1->setCreateHomeDir(data);
}

void editDefaults::setCopySkel(bool data) {
  page1->setCopySkel(data);
}

void editDefaults::setUsePrivateGroup(bool data) {
  page1->setUsePrivateGroup(data);
}

#include "editDefaults.moc"
