#ifndef UP_SYS_LOG_H
#define UP_SYS_LOG_H

#include "up_types.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \brief debug level definition
 */
#define UPLOG_LEVEL_CURRENT       0
#define UPLOG_LEVEL_NORMAL        1
#define UPLOG_LEVEL_ERROR         2

/**
 * \brief enable the file log
 */
void uplog_printout(char* filename, int line, char* func);
void uplog_printtext(char* text,...);
#define UPLOG_print(level,premsg,text)                          \
    {                                                           \
	    if(level >= UPLOG_LEVEL_CURRENT)                        \
	    {                                                       \
			uplog_printout(__FILE__,__LINE__,#premsg);          \
			uplog_printtext text;                               \
	    }                                                       \
    }

/* for performance tuning */
typedef enum _UPLOG_PER_LEVEL_
{
	UPLOG_PER_LEVEL_NONE,
	UPLOG_PER_LEVEL_RECEIVE,
	UPLOG_PER_LEVEL_PRE_DECODE,
	UPLOG_PER_LEVEL_DECODER,
	UPLOG_PER_LEVEL_PRESENT,
	UPLOG_PER_LEVEL_ALL,
	UPLOG_PER_LEVEL_COUNT
}UPLOG_PER_LEVEL;

#define UPLOG(premsg,text)       \
	UPLOG_print(UPLOG_LEVEL_NORMAL,premsg, text)

#define UBLOG_DEBUG
#ifndef UBLOG_DEBUG
    #undef  UPLOG
    #define UPLOG(function,parameters)
#endif

#ifdef __cplusplus
}
#endif
#endif //UP_SYS_LOG_H

