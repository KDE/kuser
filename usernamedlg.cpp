#include "globals.h"

#include <qlayout.h>
#include <qlabel.h>

#include <kmessagebox.h>

#include "usernamedlg.h"
#include "misc.h"
#include "kglobal_.h"

usernamedlg::usernamedlg(KUser *auser, QWidget* parent, const char* name)
	: KDialogBase(parent, name, true, i18n("Enter Username"),
		Ok|Cancel, Ok, true),
	user(auser)
{
  QFrame *page = makeMainWidget();
  QGridLayout *layout = new QGridLayout(page, 2, 2, marginHint(), spacingHint());

  QLabel* lb1 = new QLabel(page, "lb1");
  lb1->setText(i18n("Username:"));
  lb1->setMinimumSize(lb1->sizeHint());
  lb1->setAlignment(AlignRight|AlignVCenter);
  layout->addWidget(lb1, 0, 0);
  
  leusername = new QLineEdit(page, "LineEdit_1");
  lb1->setBuddy(leusername);

  // ensure it fits at least 20 characters
  leusername->setText( "XXXXXXXXXXXXXXXXXXX" );
  leusername->setMinimumSize( leusername->sizeHint() );

  // clear text
  leusername->clear();
  leusername->setFocus();
  layout->addWidget(leusername, 0, 1);
}

usernamedlg::~usernamedlg() {
}

void usernamedlg::slotOk() {
  if (kug->getUsers().lookup(leusername->text())) {
    KMessageBox::error( 0, 
      i18n("User with name %1 already exists.")
        .arg(leusername->text()) );
    return;
  }

  if (leusername->text().isEmpty()) {
    KMessageBox::error(0, i18n("You have to enter a user name."));
    return;
  }
  user->setName(leusername->text());
  
  accept();
}
#include "usernamedlg.moc"
