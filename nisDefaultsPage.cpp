#include <kapp.h>
#include <qlayout.h>
#include <qlabel.h>
#include "globals.h"
#include "nisDefaultsPage.h"
#include <klocale.h>
#include <stdio.h>
#include <sys/types.h>

nisDefaultsPage::nisDefaultsPage(QWidget* parent, const char* name)
	: QWidget(parent, name)
{
  KContainerLayout *layout = new KContainerLayout(this,
    NULL,
    KContainerLayout::Vertical,
    TRUE,
    5,
    0,
    TRUE);

  int row = 1;
  addOne(layout,row++,TRUE,FALSE,TRUE);
  addOne(layout,row++,TRUE,FALSE,TRUE);
  addOne(layout,row++,TRUE,FALSE,TRUE);
  addOne(layout,row++,TRUE,FALSE,TRUE);
  addOne(layout,row++,TRUE,FALSE,TRUE);
  addOne(layout,row++,TRUE,FALSE,TRUE);
//  addOne(layout,row++,FALSE,FALSE,FALSE);

  layout->sizeToFit();
}

void nisDefaultsPage::addOne(KContainerLayout *outer, 
       int row,
       bool homogeneous,
       bool expand,
       bool fill)
{

  KContainerLayout *inner = new KContainerLayout(outer,
    NULL,
    KContainerLayout::Horizontal,
    homogeneous,
    5,
    0,
    TRUE);

  QLabel *label;
  char *data1, *data2;

  if( row == 1 ) {
    data1 = NONNISPWD;
    data2 = NONNISGRP;
    label = new QLabel(i18n(data1), inner);
    label->setFixedSize(label->sizeHint());
    inner->packStart(label,expand,fill);
    label = new QLabel(i18n(data2), inner);
    label->setFixedSize(label->sizeHint());
    inner->packStart(label,expand,fill);
  } 
 
  if( row == 2 ) {
    passwdsrc = new QLineEdit(inner);
    passwdsrc->setFixedSize(passwdsrc->sizeHint());
    inner->packStart(passwdsrc,expand,fill);
    groupsrc = new QLineEdit(inner);
    groupsrc->setFixedSize(groupsrc->sizeHint());
    inner->packStart(groupsrc,expand,fill);
  } 
 
  if( row == 3 ) {
    data1 = NISPWD;
    data2 = NISGRP;
    label = new QLabel(i18n(data1), inner);
    label->setFixedSize(label->sizeHint());
    inner->packStart(label,expand,fill);
    label = new QLabel(i18n(data2), inner);
    label->setFixedSize(label->sizeHint());
    inner->packStart(label,expand,fill);
  } 
 
  if( row == 4 ) {
    nispasswdsrc = new QLineEdit(inner);
    nispasswdsrc->setFixedSize(nispasswdsrc->sizeHint());
    inner->packStart(nispasswdsrc,expand,fill);
    nisgroupsrc = new QLineEdit(inner);
    nisgroupsrc->setFixedSize(nisgroupsrc->sizeHint());
    inner->packStart(nisgroupsrc,expand,fill);
  } 
 
  if( row == 5 ) {
    data1 = NISMINUID;
    data2 = NISMINGID;
    label = new QLabel(i18n(data1), inner);
    label->setFixedSize(label->sizeHint());
    inner->packStart(label,expand,fill);
    label = new QLabel(i18n(data2), inner);
    label->setFixedSize(label->sizeHint());
    inner->packStart(label,expand,fill);
  } 
 
  if( row == 6 ) {
    nisminuid = new QLineEdit(inner);
    nisminuid->setFixedSize(nisminuid->sizeHint());
    inner->packStart(nisminuid,expand,fill);
    nismingid = new QLineEdit(inner);
    nismingid->setFixedSize(nismingid->sizeHint());
    inner->packStart(nismingid,expand,fill);
  } 
 
  if( row == 7 ) {
    enableNIS = new QCheckBox(i18n("Enable NIS support"), inner);
    enableNIS->setFixedSize(enableNIS->sizeHint());
    inner->packStart(enableNIS);
  } 

  inner->sizeToFit();
  outer->packStart(inner,TRUE);

}


nisDefaultsPage::~nisDefaultsPage() {
}

QString nisDefaultsPage::getPasswdSrc() const {
  return passwdsrc->text();
}

QString nisDefaultsPage::getNISPasswdSrc() const {
  return nispasswdsrc->text();
}

QString nisDefaultsPage::getGroupSrc() const {
  return groupsrc->text();
}

QString nisDefaultsPage::getNISGroupSrc() const {
  return nisgroupsrc->text();
}

QString nisDefaultsPage::getMINUID() const {
  return nisminuid->text();
}

QString nisDefaultsPage::getMINGID() const {
  return nismingid->text();
}

bool nisDefaultsPage::getEnableNIS() const {
  return enableNIS->isChecked();
}

void nisDefaultsPage::setPasswdSrc(const QString & data) {
  passwdsrc->setText(data);
}

void nisDefaultsPage::setNISPasswdSrc(const QString & data) {
  nispasswdsrc->setText(data);
}

void nisDefaultsPage::setGroupSrc(const QString & data) {
  groupsrc->setText(data);
}

void nisDefaultsPage::setNISGroupSrc(const QString & data) {
  nisgroupsrc->setText(data);
}

void nisDefaultsPage::setMINUID(const QString & data) {
  nisminuid->setText(data);
}

void nisDefaultsPage::setMINGID(const QString & data) {
  nismingid->setText(data);
}

void nisDefaultsPage::setEnableNIS(bool data) {
  enableNIS->setChecked(data);
}

void nisDefaultsPage::sourcesHelp() {
  err->addMsg(i18n("This is the Sources Help text"));
  err->display();
}

#include "nisDefaultsPage.moc"
