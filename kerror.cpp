#include "misc.h"
#include "kerror.h"
#include <kmessagebox.h>

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
     KMessageBox::information(0, msgs[i]);
  msgs.clear();
}

