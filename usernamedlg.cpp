#include "globals.h"
#include <kmsgbox.h>
#include <kbuttonbox.h>
#include <qlayout.h>
#include "usernamedlg.h"
#include "usernamedlg.moc"
#include "misc.h"
#include "maindlg.h"

usernamedlg::usernamedlg(KUser *auser, QWidget* parent, const char* name)
           :QDialog(parent, name, TRUE)
{
  user = auser;

  setCaption(_("Enter username"));

  QVBoxLayout *layout = new QVBoxLayout(this, 10);
  QGridLayout *grid = new QGridLayout(1, 2);
  layout->addLayout(grid);

  QLabel* lb1 = new QLabel(this, "lb1");
  lb1->setText(_("Username:"));
  lb1->setMinimumSize(lb1->sizeHint());
  lb1->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(lb1, 0, 0, AlignRight);

  leusername = new QLineEdit( this, "LineEdit_1" );

  // ensure it fits at least 20 characters
  leusername->setText( "XXXXXXXXXXXXXXXXXXX" );
  leusername->setMinimumSize( leusername->sizeHint() );

  // clear text
  leusername->setText( "" );
  leusername->setFocus();
  grid->addWidget(leusername, 0, 1);

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

usernamedlg::~usernamedlg() {
  delete leusername;
  delete pbOk;
  delete pbCancel;
}

void usernamedlg::ok()
{
  QString tmp;

  if (users->user_lookup(leusername->text()) != NULL) {
    tmp.sprintf(_("User with name %s already exists."), leusername->text());
    KMsgBox::message(0, _("Error"), tmp, KMsgBox::STOP);
    return;
  }
  
  user->setp_name(leusername->text());
  accept();
}

void usernamedlg::cancel()
{
  reject();
}
