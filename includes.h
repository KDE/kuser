#ifndef _XU_INCLUDES_H_
#define _XU_INCLUDES_H_

#define _XU_VERSION "0.1"

#include <qpainter.h>
#include <qpixmap.h>
#include <qlined.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qcombo.h>
#include <qchkbox.h>
#include <qlabel.h>
#include <qmsgbox.h>
#include <qpopmenu.h>
#include <qmenubar.h>
#include <qapp.h>
#include <qpushbt.h>
#include <qwidget.h>
#include <qdialog.h>
#include <qlist.h>
#include <qlistbox.h>
#include <qwindefs.h>
#include <qtooltip.h>
#include <qtabdlg.h>
#include <kconfig.h>
#include <kapp.h>
#include <kdatepik.h>
#include <kmenubar.h>
#include <kpopmenu.h>
#include <ktopwidget.h>

#ifdef _XU_SHADOW
#include <shadow.h>
#endif

#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>                                                          

#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif

#ifdef HAVE_SYS_MNTENT_H
#include <sys/mntent.h>
#endif

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>
#include "mrqpasswdlined.h"

extern char tmp[1024];

class KUser;

#ifdef _XU_QUOTA
#include "quotatool.h"
#endif
extern int is_quota;

#include "kuser.h"
#include "mnt.h"

#define XU_BACKUP_EXT ".bak"

#define PASSWORD_FILE "./passwd"
#define PASSWORD_FILE_MASK S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH

extern char picspath[200];
extern QPixmap *pic_user;
extern KConfig *config;
extern bool changed;

#ifdef _XU_SHADOW
#define SHADOW_FILE "./shadow"
#define SHADOW_FILE_MASK S_IRUSR | S_IWUSR
#endif

extern int is_shadow;

extern QList<KUser> users;

extern QList<MntEnt> mounts;

extern QFont rufont;

#ifdef AIX
extern "C" int getuid(void);
extern "C" int unlink(const char *);
#endif

#endif // _XU_INCLUDES_H_

