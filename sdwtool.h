// directory shadow:        functions processing the /etc/shadow file
#ifndef _XU_SHADOW_H_
#define _XU_SHADOW_H_

#ifdef _XU_SHADOW

#include "includes.h"

void shadow_cp(KUser *, spwd const *);      // copy fields of a shadow struct
void sdw_read(void);                            // read the /etc/shadow file
void sdw_write(void);                           // write the /etc/shadow file
spwd *spwstruct(KUser *);                                                       

#endif // _XU_SHADOW

#endif // _XU_SHADOW_H_

