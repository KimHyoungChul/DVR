#ifndef ucm_API_HEADER
#define ucm_API_HEADER

#ifndef __BREW__
#include <stdlib.h>
#include <string.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
	
#define ucm_malloc(u, x)      malloc(x)
#define ucm_realloc(u, x,a)   realloc(x,a)
#define ucm_free(u, x)        free(x)
#define ucm_calloc(u, n,s)    calloc(n,s)
#define ucm_strdup(u, str)    strdup(str)
	
#ifdef __cplusplus
}
#endif

#endif
