/**********************************************************************

	--- Dlgedit generated file ---

	File: editGroup.cpp
	Last generated: Tue Nov 25 21:00:34 1997

 *********************************************************************/

#include <qvbox.h>
#include <qwhatsthis.h>

#include <kapp.h>
#include <klocale.h>

#include "kglobal_.h"
#include "editGroup.h"



editGroup::editGroup(KGroup *akg, QWidget* parent, const char* name)
: KDialogBase(parent, name, true, i18n("Group properties"),
	Ok | Cancel, Ok, true),
  kg(akg) 
{
  QVBox *page = makeVBoxMainWidget();
 
  m_list = new KListView(page);
  m_list->setFullWidth(); // Single column, full widget width.
  m_list->addColumn(i18n("Users in Group '%1':").arg(kg->getName()));
  QString whatstr = i18n("Select the users that should be in this group.");
  QWhatsThis::add(m_list, whatstr);
  connect(this,SIGNAL(okClicked(void)),
          this,SLOT(okClicked()));		

  for (int i = 0; i<kug->getUsers().count(); i++) {
    QString userName = kug->getUsers()[i]->getName();
    QCheckListItem *item = new QCheckListItem(m_list, userName, QCheckListItem::CheckBox);
    if (kg->lookup_user(userName))
      item->setOn(true);
  }
}

editGroup::~editGroup() {
}

void editGroup::okClicked() {
  kg->clear();
 
  QCheckListItem *item = (QCheckListItem *) m_list->firstChild();
  while(item)
  {
     if (item->isOn())
        kg->addUser(item->text());

     item = (QCheckListItem *) item->nextSibling();
  }
}

#include "editGroup.moc"
