/**********************************************************************

	--- Dlgedit generated file ---

	File: editGroup.cpp
	Last generated: Tue Nov 25 21:00:34 1997

 *********************************************************************/

#include "maindlg.h"
#include "editGroup.moc"
#include "editGroupData.moc"
#include <kapp.h>
#include <kbuttonbox.h>
#include <qlayout.h>

#define Inherited editGroupData

editGroup::editGroup(KGroup *akg,
		       QWidget* parent,
		       const char* name)
  : QDialog( parent, name, true)
{
  uint i;

  kg = akg;

  QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);
  QGridLayout *l1 = new QGridLayout(2, 3);
  tl->addLayout(l1);

  QLabel *l = new QLabel(i18n("Users"), this);
  l->setFixedSize(l->sizeHint());
  l1->addWidget(l, 0, 0, AlignLeft);

  l = new QLabel(i18n("Groups"), this);
  l->setFixedSize(l->sizeHint());
  l1->addWidget(l, 0, 2, AlignLeft);

  QPushButton *pb_add = new QPushButton("->", this);
  QPushButton *pb_del = new QPushButton("<-", this);
  pb_del->setFixedSize(pb_del->sizeHint());
  pb_add->setFixedSize(pb_add->sizeHint());
  QVBoxLayout *l2 = new QVBoxLayout;
  l1->addLayout(l2, 1, 1);
  l2->addStretch(1);
  l2->addWidget(pb_add);
  l2->addWidget(pb_del);
  l2->addStretch(1);

  m_Users = new QListBox(this);
  m_Users->setMinimumSize(160, 120);
  m_Group = new QListBox(this);
  m_Group->setMinimumSize(160, 120);

  l1->addWidget(m_Users, 1, 0);
  l1->addWidget(m_Group, 1, 2);

  KButtonBox *bbox = new KButtonBox(this);
  bbox->addStretch(1);
  QPushButton *pbok = bbox->addButton(i18n("OK"));
  QPushButton *pbcancel = bbox->addButton(i18n("Cancel"));
  bbox->layout();
  tl->addWidget(bbox);
  tl->activate();

  for (i = 0; i<kg->getUsersNumber(); i++)
    m_Group->insertItem(kg->getUserName(i));

  for (i = 0; i<users->getNumber(); i++)
    if (kg->lookup_user(users->get(i)->getp_name()) == 0)
      m_Users->insertItem(users->get(i)->getp_name());

  if (m_Users->count() != 0)
    m_Users->setCurrentItem(0);
  if (m_Group->count() != 0)
    m_Group->setCurrentItem(0);

  connect(pbok, SIGNAL(clicked()),
	  this, SLOT(ok()));
  connect(pbcancel, SIGNAL(clicked()),
	  this, SLOT(cancel()));
  connect(pb_add, SIGNAL(clicked()),
	  this, SLOT(add()));
  connect(pb_del, SIGNAL(clicked()),
	  this, SLOT(del()));
}



editGroup::~editGroup()
{
}

void editGroup::ok() {
  kg->clearUsers();

  for (uint i=0; i<m_Group->count(); i++)
    kg->addUser(m_Group->text(i));

  accept();
}

void editGroup::cancel() {
  reject();
}

void editGroup::add() {
  int cur = m_Users->currentItem();

  if (cur == -1)
    return;

  m_Group->insertItem(m_Users->text(cur));
  m_Users->removeItem(cur);

  if (((uint)cur) == m_Users->count())
    m_Users->setCurrentItem(cur-1);
  else
    m_Users->setCurrentItem(cur);

  m_Group->setCurrentItem(m_Group->count()-1);
  m_Group->centerCurrentItem();
}

void editGroup::del() {
  int cur = m_Group->currentItem();

  if (cur == -1)
    return;

  m_Users->insertItem(m_Group->text(cur));
  m_Group->removeItem(cur);

  if (((uint)cur) == m_Group->count())
    m_Group->setCurrentItem(cur-1);
  else
    m_Group->setCurrentItem(cur);
  
  m_Users->setCurrentItem(m_Users->count()-1);
  m_Users->centerCurrentItem();
}

