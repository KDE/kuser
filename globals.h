#ifndef _KU_GLOBALS_H_
#define _KU_GLOBALS_H_

#define _KU_VERSION "0.4"

#include "../config.h"
#include <kconfig.h>
#include "kerror.h"

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

extern int is_quota;
extern int is_shadow;

#define KU_BACKUP_EXT ".bak"

#ifdef __FreeBSD__
#undef _KU_SHADOW
#include <pwd.h>
#include <paths.h>
#define SHELL_FILE _PATH_SHELLS
#define PASSWORD_FILE _PATH_MASTERPASSWD
#define PASSWORD_FILE_MASK S_IRUSR | S_IWUSR
#define PWMKDB _PATH_PWD_MKDB" -p "PASSWORD_FILE
#define SKEL_DIR "/usr/share/skel"
#define SKEL_FILE_PREFIX "dot"
#else
#define SHELL_FILE "/etc/shells"
#define PASSWORD_FILE "/etc/passwd"
#define PASSWORD_FILE_MASK S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define SKEL_DIR "/etc/skel"
#define SKEL_FILE_PREFIX ""
#endif

extern char picspath[200];
extern KConfig *config;
extern KError *err;
//extern bool changed;

#ifdef _KU_SHADOW
#define SHADOW_FILE "/etc/shadow"
#define SHADOW_FILE_MASK S_IRUSR | S_IWUSR
#endif

#ifdef AIX
extern "C" int getuid(void);
extern "C" int unlink(const char *);
#endif

#endif // _KU_GLOBALS_H_

