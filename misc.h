#ifndef _KU_MISC_H
#define _KU_MISC_H

#include <klocale.h>
#include <kapp.h>
#include <qstring.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qlined.h>

#define _(Text) klocale->translate (Text)

class KUser;

QString readentry(const QString &name);
int readnumentry(const QString &name);
void backup(const char *name);
char *convertdate(char *buf, const long int base, const long int days);
QLabel *addLabel(QWidget *parent, const char *name, int x, int y, int w, int h, const char *text);
QLineEdit *addLineEdit(QWidget *parent, const char *name, int x, int y, int w, int h, const char *text);
char *updateString(char *d, const char *t);
int getValue(long int &data, const char *text, const char *msg);
int getValue(int &data, const char *text, const char *msg);
int getValue(unsigned int &data, const char *text, const char *msg);
long today();

#endif // _KU_MISC_H
