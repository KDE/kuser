#include <kmsgbox.h>
#include "usernamedlg.h"
#include "usernamedlg.moc"
#include "misc.h"
#include "maindlg.h"

usernamedlg::usernamedlg(KUser *auser, QWidget* parent, const char* name)
           :QDialog(parent, name, TRUE)
{
  user = auser;

  setCaption(_("Enter username"));
  pbOk = new QPushButton( this, "PushButton_1" );            
  pbOk->setGeometry( 20, 90, 100, 30 );                       
  pbOk->setText(_("Ok"));
  pbOk->setDefault(TRUE);
  QObject::connect(pbOk, SIGNAL(clicked()), this, SLOT(ok()));
                                                                               
  pbCancel = new QPushButton( this, "PushButton_2" );            
  pbCancel->setGeometry( 170, 90, 100, 30 );                      
  pbCancel->setText(_("Cancel"));                                   
  pbCancel->setDefault(FALSE);
  QObject::connect(pbCancel, SIGNAL(clicked()), this, SLOT(cancel()));                  
                                                                               
  QLabel* lb1 = new QLabel(this, "lb1");
  lb1->setGeometry(40, 10, 200, 30);
  lb1->setText(_("Enter username"));

  leusername = new QLineEdit( this, "LineEdit_1" );                  
  leusername->setGeometry( 40, 40, 200, 30 );                         
  leusername->setText( "" );                                           
  leusername->setFocus();
                        
  resize( 300, 130 );
}

void usernamedlg::ok()
{
  QString tmp;

  if (users->user_lookup(leusername->text()) != NULL) {
    tmp.sprintf(_("User with name %s already exists."), leusername->text());
    KMsgBox::message(0, _("Error"), tmp, KMsgBox::STOP);
  }
  else {
    user->p_name.setStr(leusername->text());
    accept();
  }
}

void usernamedlg::cancel()
{
  reject();
}
