#ifndef _KU_MISC_H
#define _KU_MISC_H

#include <klocale.h>
#include <kapp.h>
#include <qstring.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>

class KUser;

QString readentry(const QString &name, const QString def = QString::null);
int readnumentry(const QString &name);
void backup(const QString & name);
char *convertdate(char *buf, const long int base, const long int days);
QLabel *addLabel(QWidget *parent, const char *name, int x, int y, int w, int h, const QString & text);
QLineEdit *addLineEdit(QWidget *parent, const char *name, int x, int y, int w, int h, const QString & text);
char *updateString(char *d, const char *t);
int getValue(long int &data, const QString & text, const QString & msg);
int getValue(int &data, const QString & text, const QString & msg);
int getValue(unsigned int &data, const QString & text, const QString & msg);
long today();
int copyFile(const QString & from, const QString & to);

#endif // _KU_MISC_H
