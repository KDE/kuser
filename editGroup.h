/**********************************************************************

	--- Dlgedit generated file ---

	File: editGroup.h
	Last generated: Tue Nov 25 21:00:34 1997

 *********************************************************************/

#ifndef editGroup_included
#define editGroup_included

#include <kdialogbase.h>
#include <klistview.h>

#include "kgroup.h"

class editGroup : public KDialogBase
{
  Q_OBJECT
public:

  editGroup(KGroup *akg,
	     QWidget* parent = NULL,
	     const char* name = NULL);

  virtual ~editGroup();

protected slots:
  virtual void okClicked();

protected:
  KGroup *kg;
  KListView *m_list;
};
#endif // editGroup_included
