#include "config.h"

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#include "pwddlg.h"
#include "pwddlg.moc"
#include "misc.h"

pwddlg::pwddlg(KUser *auser, QWidget* parent, const char* name)
           :QDialog(parent, name, TRUE)
{
  user = auser;

  setCaption(_("Enter password"));
  pbOk = new QPushButton( this, "PushButton_1" );
  pbOk->setGeometry( 20, 140, 100, 30 );
  pbOk->setText( "Ok" );
  pbOk->setDefault(TRUE);
  QObject::connect(pbOk, SIGNAL(clicked()), this, SLOT(ok()));

  pbCancel = new QPushButton( this, "PushButton_2" );
  pbCancel->setGeometry( 170, 140, 100, 30 );
  pbCancel->setText(_("Cancel"));
  pbCancel->setDefault(FALSE);
  QObject::connect(pbCancel, SIGNAL(clicked()), this, SLOT(cancel()));

  QLabel* lb1 = new QLabel(this, "lb1");
  lb1->setGeometry(40, 10, 200, 30);
  lb1->setText(_("Enter password"));

  leusername1 = new MrQPasswordLineEdit( this, "LineEdit_1" );                  
  leusername1->setGeometry( 40, 40, 200, 30 );                         
  leusername1->setText( "" );                                           
  leusername1->setFocus();
                        
  leusername2 = new MrQPasswordLineEdit( this, "LineEdit_2" );
  leusername2->setGeometry( 40, 80, 200, 30 );                         
  leusername2->setText( "" );                                           
                        
  resize( 300, 180 );
}

void pwddlg::ok()
{
  char salt[3];
  const char * set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

  if (strcmp(leusername1->text(), leusername2->text())) {
    QMessageBox::message(_("Error"), _("Passwords are not identical.\nTry again"), "Ok");
    leusername1->setText("");
    leusername2->setText("");
    leusername1->setFocus();
  }
  else {
    srand(time(NULL));
    salt[0] = set[getpid() % sizeof(set)];
    salt[1] = set[rand() % sizeof(set)];
    salt[2] = 0;

    strcpy(tmp, crypt(leusername1->text(), salt));

    if (is_shadow != 0) {
      user->s_pwd.setStr(tmp);
      user->p_pwd.setStr("x");
    }
    else
      user->p_pwd.setStr(tmp);
    accept();
  }
}

void pwddlg::cancel()
{
  reject();
}

