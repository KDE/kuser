#include "globals.h"

#include <qstring.h>
#include <qvalidator.h>
#include <qgrid.h>
#include <kmessagebox.h>

#include "grpnamedlg.h"
#include "misc.h"
#include "kglobal_.h"

grpnamedlg::grpnamedlg(KGroup &AGroup, QWidget* parent, const char* name)
  : KDialogBase(parent, name, true, i18n("Add Group"), Ok | Cancel, Ok, true), 
    group(AGroup) 
{
  group.setGID(kug->getGroups().first_free());

  QGrid *page = makeGridMainWidget(2, QGrid::Horizontal);

  QLabel* lb1 = new QLabel(page, "lb1");
  lb1->setText(i18n("Group name:"));
  lb1->setMinimumSize(lb1->sizeHint());
  lb1->setAlignment(AlignRight|AlignVCenter);

  legrpname = new QLineEdit( page, "LineEdit_1" );
  // ensure it fits at least 20 characters
  legrpname->setText("XXXXXXXXXXXXXXXXXXX");
  legrpname->setMinimumSize( legrpname->sizeHint() );

  // clear text
  legrpname->clear();
  legrpname->setFocus();
  lb1->setBuddy(legrpname);

  QLabel* lb2 = new QLabel(page, "lb2");
  lb2->setText(i18n("Group number:"));
  lb2->setMinimumSize(lb2->sizeHint());
  lb2->setAlignment(AlignRight|AlignVCenter);

  legid = new QLineEdit(page, "LineEdit_2");
  // ensure it fits at least 20 characters
  legid->setText("XXXXXXXXXXXXXXXXXXX");
  legid->setMinimumSize(legid->sizeHint());
  
  // clear text
  legid->setText(QString("%1").arg(group.getGID()));
  legid->setValidator(new QIntValidator(this, "val1"));
  lb2->setBuddy(legid);
}

grpnamedlg::~grpnamedlg() {
}

void grpnamedlg::slotOk()
{
  QString tmp;
  QString s;
  s = legid->text();

  if (kug->getGroups().lookup(legrpname->text())) {
    tmp = i18n("Group with name %1 already exists.").arg(legrpname->text());
    KMessageBox::sorry(0, tmp);
    return;
  }
  
  if (kug->getGroups().lookup(s.toInt())) {
    tmp = i18n("Group with gid %1 already exists.").arg(s.toInt());
    KMessageBox::sorry(0, tmp);
    return;
  }
  
  group.setName(legrpname->text());
  group.setGID(s.toInt());
  accept();
}

#include "grpnamedlg.moc"
