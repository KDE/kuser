/**********************************************************************

	--- Dlgedit generated file ---

	File: editGroup.h
	Last generated: Tue Nov 25 21:00:34 1997

 *********************************************************************/

#ifndef editGroup_included
#define editGroup_included

#include <qdialog.h>
#include <qlistbox.h>

#include "kgroup.h"
#include "editGroupData.h"

class editGroup : public editGroupData
{
  Q_OBJECT
public:

  editGroup(KGroup *akg,
	     QWidget* parent = NULL,
	     const char* name = NULL);

  virtual ~editGroup();

protected slots:
  virtual void ok();
  virtual void cancel();

  virtual void add();
  virtual void del();

protected:
  KGroup *kg;

  QListBox *m_Users, *m_Group;
};
#endif // editGroup_included
