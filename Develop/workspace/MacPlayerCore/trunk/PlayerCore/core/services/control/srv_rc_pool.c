#include "up_types.h"
#include "ucm.h"
#include "up_sys_log.h"
#include "util_circle_list.h"

#include "srv_com_handler.h"
#include "srv_rc_pool.h"

void rc_pool_reset(Data_Pool *pool)
{
	PRC_data_entry_str pentry;

	while(UP_NULL != (pentry = (PRC_data_entry_str)util_cl_list_pop(pool->list)))
	{
		util_cl_list_push(pool->pool, (void*)pentry);
	}
}

UPSint rc_pool_init(Data_Pool *pool, UPUint size)
{
	int ret = 0;
	PRC_data_entry_str pentry;
	int count = pool->count;

	pool->list = util_cl_create(count);
	pool->pool = util_cl_create(count);

	while(--count >= 0)
	{
		pentry = (PRC_data_entry_str)memset((void*)ucm_malloc(UP_NULL, sizeof(RC_data_entry_str)), 0, sizeof(RC_data_entry_str));
		pentry->data = (char*)ucm_malloc(UP_NULL, size);
		if(pentry->data == UP_NULL)
		{
			UPLOG(rc_pool_init, ("not enought memeory"));
			rc_pool_fini(pool);
			ret = -1;
			break;
		}
		pentry->size = size;
		util_cl_list_push(pool->pool, (void*)pentry);
	}

	return ret;
}

UPSint rc_pool_fini(Data_Pool* pool)
{
	PRC_data_entry_str pentry;
	int count = pool->count;

	if(pool->pool == UP_NULL || pool->list == UP_NULL)
	{
		UPLOG(rc_pool_fini, ("the pool has already release"));
		return 0;
	}

	UPLOG(rc_pool_fini, ("the pool entry count: %d", pool->count));

	rc_pool_reset(pool);
	while(--count >= 0)
	{
		pentry = (PRC_data_entry_str)pool->pool->list[count];
		if(pentry)
		{
			ucm_free(UP_NULL, pentry->data);
			ucm_free(UP_NULL, pentry);
		}
	}
	util_cl_destroy(pool->pool);
	pool->pool = UP_NULL;
	util_cl_destroy(pool->list);
	pool->list = UP_NULL;

	return 0;
}

PRC_data_entry_str rc_pool_listPop(Data_Pool* pool)
{
	if(!pool)
	{
		return UP_NULL;
	}

	return pool->list ? (PRC_data_entry_str)util_cl_list_pop(pool->list) : UP_NULL;
}

PRC_data_entry_str rc_pool_listPush(Data_Pool* pool, PRC_data_entry_str data)
{
	if(!pool)
	{
		return UP_NULL;
	}

	return (PRC_data_entry_str)util_cl_list_push(pool->list, (void*)data);
}

PRC_data_entry_str rc_pool_poolPop(Data_Pool* pool)
{
	if(!pool)
	{
		return UP_NULL;
	}

	return pool->pool ? (PRC_data_entry_str)util_cl_list_pop(pool->pool) : UP_NULL;
}

PRC_data_entry_str rc_pool_poolPush(Data_Pool* pool, PRC_data_entry_str data)
{
	if(!pool)
	{
		return UP_NULL;
	}

	return (PRC_data_entry_str)util_cl_list_push(pool->pool, (void*)data);
}
