#ifndef _KU_GLOBALS_H_
#define _KU_GLOBALS_H_

#define _KU_VERSION "0.3"

#ifdef BSD
#undef _KU_SHADOW
#endif

#include "../config.h"
#include <kconfig.h>
#include "kerror.h"

extern int is_quota;
extern int is_shadow;

#define KU_BACKUP_EXT ".bak"

#define PASSWORD_FILE "/etc/passwd"
#define PASSWORD_FILE_MASK S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH

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

