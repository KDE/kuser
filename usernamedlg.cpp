#include "globals.h"

#include <qmessagebox.h>
#include <qlayout.h>

#include <kbuttonbox.h>

#include "usernamedlg.h"
#include "misc.h"
#include "kglobal_.h"

usernamedlg::usernamedlg(KUser *auser, QWidget* parent, const char* name)
           :QDialog(parent, name, TRUE) {
  user = auser;

  setCaption(i18n("Enter username"));

  QVBoxLayout *layout = new QVBoxLayout(this, 10);
  QGridLayout *grid = new QGridLayout(1, 2);
  layout->addLayout(grid);

  QLabel* lb1 = new QLabel(this, "lb1");
  lb1->setText(i18n("Username:"));
  lb1->setMinimumSize(lb1->sizeHint());
  lb1->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(lb1, 0, 0, AlignRight);

  leusername = new QLineEdit( this, "LineEdit_1" );

  // ensure it fits at least 20 characters
  leusername->setText( "XXXXXXXXXXXXXXXXXXX" );
  leusername->setMinimumSize( leusername->sizeHint() );

  // clear text
  leusername->clear();
  leusername->setFocus();
  grid->addWidget(leusername, 0, 1);

  // add a button box
  KButtonBox *bbox = new KButtonBox(this);

  // make buttons right aligned
  bbox->addStretch(1);

  // the default buttons  
  pbOk = bbox->addButton(i18n("OK"));
  pbCancel = bbox->addButton(i18n("Cancel"));
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

usernamedlg::~usernamedlg() {
  delete leusername;
  delete pbOk;
  delete pbCancel;
}

void usernamedlg::ok() {
  if (kug->getUsers().lookup(leusername->text()) != NULL) {
    QMessageBox::information(0, 
      i18n("Error"), 
      i18n("User with name %1 already exists.")
        .arg(leusername->text()), i18n("OK"));
    return;
  }

  if (strlen(leusername->text()) == 0) {
    QMessageBox::warning(0, i18n("Error"), 
			 i18n("You have to enter a user name."), 
			 i18n("OK"));
    return;
  }
  
  user->setFullName(leusername->text());
  accept();
}

void usernamedlg::cancel() {
  reject();
}
