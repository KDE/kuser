#ifndef _KU_MISC_H
#define _KU_MISC_H

#include "kuser.h"
#include "includes.h"
#include "../config.h"

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

#ifdef _KU_QUOTA

#ifdef HAVE_SYS_FS_UFS_QUOTA_H
#include <sys/fs/ufs_quota.h>
#define CORRECT_FSTYPE(type) (!strcmp(type,MNTTYPE_UFS))
#define _KU_QUOTAFILENAME "quotas"
#define _KU_UFS_QUOTA
#else
#ifdef HAVE_LINUX_QUOTA_H
#include <linux/quota.h>
#define CORRECT_FSTYPE(type) (!strcmp(type,MNTTYPE_EXT2))
#define _KU_QUOTAFILENAME "quota.user"
#define _KU_EXT2_QUOTA
#else
#error "Your platform is not supported"
#endif  // HAVE_SYS_FS_UFS_QUOTA_H
#endif // HAVE_LINUX_QUOTA_H

#endif // _KU_QUOTA

#endif // _KU_MISC_H

