#include "misc.h"
#include "kerror.h"
#include <qmessagebox.h>

KError::KError() {
}

KError::~KError() {
  display();
}

void KError::addMsg(QString amsg) {
  msgs.append(amsg);
}

void KError::display() {
  for (uint i=0;i<msgs.count();i++)
     QMessageBox::information(0, i18n("Message"),
       msgs[i], i18n("OK"));
  msgs.clear();
}

