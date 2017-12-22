#ifndef _UTIL_CIRCLE_LIST_HEADER
#define _UTIL_CIRCLE_LIST_HEADER

#include "up_types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct __utilCircleListStr_
{
	//global
	unsigned int entry_number;
	unsigned int high_index;

	//circle index
	unsigned int circle_push_count;
	unsigned int circle_push_index;
	unsigned int circle_pop_count;
	unsigned int circle_pop_index;

	//resource pool
	unsigned int pool_push_count;
	unsigned int pool_push_index;
	unsigned int pool_pop_count;
	unsigned int pool_pop_index;

	//circle list
	void** list;
}
* putilCircleListStr, utilCircleListStr;

putilCircleListStr util_cl_create(int len);
void util_cl_destroy(putilCircleListStr m_gcs_cycle_list);
void* util_cl_list_pop(putilCircleListStr m_gcs_cycle_list);
void* util_cl_list_push(putilCircleListStr m_gcs_cycle_list, void* pentry);
//int util_cl_list_isFull(putilCircleListStr m_gcs_cycle_list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !___UPTYPES_H */
