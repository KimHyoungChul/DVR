#ifndef _UP_SYS_CONFIG_HEADER
#define _UP_SYS_CONFIG_HEADER

//This is a empty file
#define __LINUX_32__ 
#include "up_types.h"
#include "up_sys_version.h"

/*define the thread handler*/
typedef void* UpThreadHandle;
/*mutex*/
typedef UPHandle UpMutexHandle;
/*event*/
typedef UPHandle UpEventHandle;
#define UP_EVENT_WAIT_INFINITE (-1)
#define UP_EVENT_WAIT_TIMEOUT  (-2)
#define UP_EVENT_WAIT_FAILURE  (-3)
#define UP_EVENT_WAIT_OK       0

#endif

