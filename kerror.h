#ifndef _KERROR_H
#define _KERROR_H

#include <qobject.h>
#include <qlist.h>
#include <qstring.h>

typedef enum KErrorType {INFORMATION = 1, EXCLAMATION = 2, STOP = 4, QUESTION = 8};

class KErrorMsg {
public:
  KErrorMsg(QString amsg, KErrorType aerr);
  ~KErrorMsg();
  
  QString getMsg();
  KErrorType getErr();
  
private:
  QString msg;
  KErrorType err;
};

class KError: public QObject {
public:

  KError();
  ~KError();
  
  void addMsg(QString amsg, KErrorType aerr);
  void display();
 
private:
  QList<KErrorMsg> msgs;
};

#endif // _KERROR_H
