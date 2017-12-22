#ifndef UP_SYS_UTIL_H
#define UP_SYS_UTIL_H

#ifndef __BREW__
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif

#include "../../up_types.h"

#ifdef __cplusplus
extern "C"{
#endif

void up_util_init(void* p);
void up_util_fini(void);
UPUint up_util_getCurrentTick(void);
UPSint up_util_s2i(UPSint8* str);
UPResult util_str_isIPAddress(UPUint8* src, UPSint length);

#ifdef __cplusplus
}
#endif
#endif //UP_SYS_UTIL_H
