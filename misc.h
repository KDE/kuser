#ifndef _KU_MISC_H
#define _KU_MISC_H

#include <klocale.h>
#include <kapplication.h>
#include <qstring.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>

class KUser;

void backup(const QString & name);
char *updateString(char *d, const char *t);
int getValue(long int &data, const QString & text, const QString & msg);
int getValue(int &data, const QString & text, const QString & msg);
int getValue(unsigned int &data, const QString & text, const QString & msg);
long today();
int copyFile(const QString & from, const QString & to);
QStringList readShells();
void addShell(const QString &shell);

#endif // _KU_MISC_H
