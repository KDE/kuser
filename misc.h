#ifndef _XU_MISC_H
#define _XU_MISC_H

#define _(Text) gettext (Text)

KUser *user_lookup(const char *name);
uint first_free();
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
void getmounts();
void init();

#endif // _XU_MISC_H

