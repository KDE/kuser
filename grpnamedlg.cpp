#include "globals.h"
#include <kmsgbox.h>
#include <kbuttonbox.h>
#include <qlayout.h>
#include <qvalidator.h>
#include "grpnamedlg.h"
#include "grpnamedlg.moc"
#include "misc.h"
#include "maindlg.h"

grpnamedlg::grpnamedlg(KGroup *agroup, QWidget* parent, const char* name)
           :QDialog(parent, name, TRUE)
{
  group = agroup;
  group->setgid(groups->first_free());

  setCaption(_("Add group"));

  QVBoxLayout *layout = new QVBoxLayout(this, 10);
  QGridLayout *grid = new QGridLayout(2, 2);
  layout->addLayout(grid);

  QLabel* lb1 = new QLabel(this, "lb1");
  lb1->setText(_("Group name:"));
  lb1->setMinimumSize(lb1->sizeHint());
  lb1->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(lb1, 0, 0, AlignRight);

  QLabel* lb2 = new QLabel(this, "lb2");
  lb2->setText(_("Group number:"));
  lb2->setMinimumSize(lb2->sizeHint());
  lb2->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(lb2, 1, 0, AlignRight);

  legrpname = new QLineEdit( this, "LineEdit_1" );

  // ensure it fits at least 20 characters
  legrpname->setText( "XXXXXXXXXXXXXXXXXXX" );
  legrpname->setMinimumSize( legrpname->sizeHint() );

  // clear text
  legrpname->setText( "" );
  legrpname->setFocus();
  grid->addWidget(legrpname, 0, 1);

  legid = new QLineEdit( this, "LineEdit_2" );

  // ensure it fits at least 20 characters
  legid->setText( "XXXXXXXXXXXXXXXXXXX" );
  legid->setMinimumSize( legid->sizeHint() );
  
  QString s;
  s.setNum(group->getgid());
  
  // clear text
  legid->setText(s);
  legid->setValidator(new QIntValidator(this, "val1"));

  grid->addWidget(legid, 1, 1);
  
  // add a button box
  KButtonBox *bbox = new KButtonBox(this);

  // make buttons right aligned
  bbox->addStretch(1);

  // the default buttons  
  pbOk = bbox->addButton(_("Ok"));
  pbCancel = bbox->addButton(_("Cancel"));
  pbOk->setDefault(TRUE);
  
  // establish callbacks
  QObject::connect(pbOk, SIGNAL(clicked()), 
		   this, SLOT(ok()));
  QObject::connect(pbCancel, SIGNAL(clicked()), 
		   this, SLOT(cancel()));

  bbox->layout();
  bbox->setMinimumSize(bbox->sizeHint());
  
  layout->addWidget(bbox);
  layout->freeze();
}

grpnamedlg::~grpnamedlg() {
  delete legrpname;
  delete pbOk;
  delete pbCancel;
}

void grpnamedlg::ok()
{
  QString tmp;
  QString s;
  s.setStr(legid->text());

  if (groups->group_lookup(legrpname->text()) != NULL) {
    tmp.sprintf(_("Group with name %s already exists."), legrpname->text());
    KMsgBox::message(0, _("Error"), tmp, KMsgBox::STOP);
    return;
  }
  
  if (groups->group_lookup(s.toInt()) != NULL) {
    tmp.sprintf(_("Group with gid %d already exists."), s.toInt());
    KMsgBox::message(0, _("Error"), tmp, KMsgBox::STOP);
    return;
  }
  
  group->setname(legrpname->text());
  group->setgid(s.toInt());
  accept();
}

void grpnamedlg::cancel()
{
  reject();
}
