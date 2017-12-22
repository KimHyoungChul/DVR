#ifndef _KT_CORE_HANDLER_HEADER
#define _KT_CORE_HANDLER_HEADER

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

int kt_core_fwInit();
int kt_core_fwFini();
	
int kt_core_createDvrHome(char *ipAddr);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_KT_CORE_HANDLER_HEADER*/
