#ifndef _KERROR_H
#define _KERROR_H

#include <qobject.h>
#include <qlist.h>
#include <qstring.h>

class KError: public QObject {
public:
  KError();
  ~KError();
  
  void addMsg(QString amsg);
  void display();
 
private:
  QStringList msgs;
};

#endif // _KERROR_H
