#include <kmsgbox.h>
#include "misc.h"
#include "kerror.h"

KErrorMsg::KErrorMsg(QString amsg, KErrorType aerr) {
  msg = amsg;
  err = aerr;
}

KErrorMsg::~KErrorMsg() {
}

QString KErrorMsg::getMsg() {
  return (msg);
}

KErrorType KErrorMsg::getErr() {
  return (err);
}

KError::KError() {
  msgs.setAutoDelete(TRUE);
}

KError::~KError() {
  display();
}

void KError::addMsg(QString amsg, KErrorType aerr) {
  msgs.append(new KErrorMsg(amsg, aerr));
}

void KError::display() {
  for (uint i=0;i<msgs.count();i++)
    KMsgBox::message(0, i18n("Message"), msgs.at(i)->getMsg(), msgs.at(i)->getErr());
    
  msgs.clear();
}

