// directory shadow:        functions processing the /etc/shadow file
#ifndef _KU_SHADOW_H_
#define _KU_SHADOW_H_

#ifdef _KU_SHADOW

#include "includes.h"

void shadow_cp(KUser *, spwd const *);      // copy fields of a shadow struct
void sdw_read(void);                            // read the /etc/shadow file
void sdw_write(void);                           // write the /etc/shadow file
spwd *spwstruct(KUser *);                                                       

#endif // _KU_SHADOW

#endif // _KU_SHADOW_H_

