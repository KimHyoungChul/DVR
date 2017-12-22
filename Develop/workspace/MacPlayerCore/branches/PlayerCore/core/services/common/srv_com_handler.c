#include "up_types.h"
#include "ucm.h"
#include "up_sys_log.h"

#include <pthread.h>

#include "srv_com_handler.h"
#include "srv_rc_pool.h"
#include "srv_vs_vmsdvr.h"
#include "srv_vs_file.h"
#include "srv_vs_vms.h"

Data_Pool *pool_create(UPSint count)
{
	Data_Pool *pool = (Data_Pool*)ucm_malloc(UP_NULL, sizeof(Data_Pool));
	if(pool == UP_NULL)
	{
		UPLOG(pool_create, ("create pool error, not enought memory"));
		return UP_NULL;
	}

	pool->count = count;

	return pool;
}

void pool_release(Data_Pool *pool)
{
	rc_pool_fini(pool);

	ucm_free(UP_NULL, pool);
}

SRV_Instance *srv_com_create(SRV_Identify type, const UPSint8* name)
{
	int ret = 0;
	SRV_Instance *instance = (SRV_Instance*)ucm_malloc(UP_NULL, sizeof(SRV_Instance));
	if(instance == UP_NULL)
	{
		UPLOG(srv_com_create, ("create SRV_Instance error not enought memory"));
		return UP_NULL;
	}

	memset((void*)instance, 0x0, sizeof(SRV_Instance));

	instance->type = type;
	instance->name = (UPUint8*)ucm_strdup(UP_NULL, (const char*)name);

	instance->videoStream = UP_INVALID_HANDLE;
	instance->videoStreamStatus = SRV_Thread_status_notCreate;
	instance->videoStreamMutex = (UpMutexHandle)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((pthread_mutex_t*)instance->videoStreamMutex, NULL);
	instance->videoStreamEvent = (UpEventHandle)malloc(sizeof(pthread_cond_t));
	pthread_cond_init((pthread_cond_t*)instance->videoStreamEvent, NULL);

	instance->netHandle = UP_NULL;
	instance->fileHandle = UP_NULL;
	instance->netStatus = SRV_NETWORK_NOTCREATE;

	/*video queue*/
	instance->videoPool = pool_create(24);
	if(instance->videoPool == UP_NULL)
	{
		/*must handle this error*/
		goto create_fail;
	}
	/*pool data size: 100K*/
	ret = rc_pool_init(instance->videoPool, 128*1024);
	if(ret < 0)
	{
		/*error, maybe not enought memory*/
		goto create_fail;
	}
	
	/*audio queue*/
	instance->audioPool = pool_create(64);
	if(instance->audioPool == UP_NULL)
	{
		/*must handle this error*/
		goto create_fail;
	}
	ret = rc_pool_init(instance->audioPool, 1024);
	if(ret < 0)
	{
		/*error, maybe not enought memory*/
		goto create_fail;
	}
	
	instance->partner = UP_NULL;
	
	instance->recFp = UP_NULL;
	instance->recFlag = 0;
	
	instance->dstFileSize = 0;

	return instance;

create_fail:
	if(instance->name)
	{
		ucm_free(UP_NULL, instance->name);
		instance->name = UP_NULL;
	}
	if(instance->videoStreamMutex)
	{
		pthread_mutex_destroy((pthread_mutex_t*)instance->videoStreamMutex);
		free((void*)instance->videoStreamMutex);
		instance->videoStreamMutex = UP_NULL;
	}
	if(instance->videoStreamEvent)
	{
		pthread_cond_destroy((pthread_cond_t*)instance->videoStreamEvent);
		free((void*)instance->videoStreamEvent);
		instance->videoStreamEvent = UP_NULL;
	}
	if(instance->videoPool)
	{
		pool_release(instance->videoPool);
		instance->videoPool = UP_NULL;
	}
	if(instance->audioPool)
	{
		pool_release(instance->audioPool);
		instance->audioPool = UP_NULL;
	}
	if(instance)
	{
		ucm_free(UP_NULL, instance);
		instance = UP_NULL;
	}

	return UP_NULL;
}

void srv_com_destroy(SRV_Instance *instance)
{
	if(instance->name)
	{
		ucm_free(UP_NULL, instance->name);
		instance->name = UP_NULL;
	}
	if(instance->videoStreamMutex)
	{
		pthread_mutex_destroy((pthread_mutex_t*)instance->videoStreamMutex);
		free((void*)instance->videoStreamMutex);
		instance->videoStreamMutex = UP_NULL;
	}
	if(instance->videoStreamEvent)
	{
		pthread_cond_destroy((pthread_cond_t*)instance->videoStreamEvent);
		free((void*)instance->videoStreamEvent);
		instance->videoStreamEvent = UP_NULL;
	}
	if(instance->videoPool)
	{
		pool_release(instance->videoPool);
		instance->videoPool = UP_NULL;
	}
	if(instance->audioPool)
	{
		pool_release(instance->audioPool);
		instance->audioPool = UP_NULL;
	}
	if(instance->recFp)
	{
		fclose(instance->recFp);
		instance->recFp = UP_NULL;
	}
	if(instance)
	{
		ucm_free(UP_NULL, instance);
		instance = UP_NULL;
	}
}

void srv_com_setChannel(SRV_Instance *instance, UPUint channel)
{
	if(instance != UP_NULL && instance->netHandle != NULL)
	{
		instance->netHandle->videoChannel = channel;
	}
	else {
		UPLOG(srv_com_setChannel, ("please init SRV_Instance first"));
	}

}

UPSint srv_com_createVmsInstance(SRV_Instance *instance, UPSint8* addr, UPSint32 port, UPSint8* name, UPSint8* password)
{
	if(instance == UP_NULL)
	{
		return -1;
	}
	
	instance->netHandle = (NET_Instance*)ucm_malloc(UP_NULL, sizeof(NET_Instance));
	if(instance->netHandle == UP_NULL)
	{
		return -1;
	}
	memset((void*)instance->netHandle, 0x0, sizeof(NET_Instance));
	instance->netHandle->type = SRV_ID_SZVMS;
	instance->netHandle->videoChannel = 0;
	instance->netHandle->networkType = 0;
	
	instance->netHandle->username = (UPSint8*)ucm_strdup(UP_NULL, (const UPSint8*)name);
	instance->netHandle->password = (UPSint8*)ucm_strdup(UP_NULL, (const UPSint8*)password);
	instance->netHandle->addr = (UPSint8*)ucm_strdup(UP_NULL, (const UPSint8*)addr);
	instance->netHandle->port = port;
	
	instance->netHandle->mutex = (UpMutexHandle)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((pthread_mutex_t*)instance->netHandle->mutex, NULL);
	instance->netHandle->event = (UpEventHandle)malloc(sizeof(pthread_cond_t));
	pthread_cond_init((pthread_cond_t*)instance->netHandle->event, NULL);
	
	instance->netHandle->sendQBuf = (UPSint8*)ucm_malloc(UP_NULL, 1024*1024);
	if(instance->netHandle->sendQBuf == UP_NULL)
	{
		goto create_fail;
	}
	instance->netHandle->sendQBufSize = 1024*1024;
	instance->netHandle->sendQBufLen = 0;
	
	instance->netHandle->recvQBuf = (UPSint8*)ucm_malloc(UP_NULL, 1024*1024);
	if(instance->netHandle->recvQBuf == UP_NULL)
	{
		goto create_fail;
	}
	instance->netHandle->recvQBufSize = 1024*1024;
	instance->netHandle->recvQBufLen = 0;

	instance->netHandle->recvLastTick = 0;
	instance->netHandle->dueTick = 0;
	
	instance->netHandle->next = UP_NULL;
	
	return 0;
	
create_fail:
	if(instance->netHandle->username)
	{
		ucm_free(UP_NULL, instance->netHandle->username);
		instance->netHandle->username = UP_NULL;
	}
	if(instance->netHandle->password)
	{
		ucm_free(UP_NULL, instance->netHandle->password);
		instance->netHandle->password = UP_NULL;
	}
	if(instance->netHandle->addr)
	{
		ucm_free(UP_NULL, instance->netHandle->addr);
		instance->netHandle->addr = UP_NULL;
	}
	
	if(instance->netHandle->mutex)
	{
		pthread_mutex_destroy((pthread_mutex_t*)instance->netHandle->mutex);
		free((void*)instance->netHandle->mutex);
		instance->netHandle->mutex = UP_NULL;
	}
	if(instance->netHandle->event)
	{
		pthread_cond_destroy((pthread_cond_t*)instance->netHandle->event);
		free((void*)instance->netHandle->event);
		instance->netHandle->event = UP_NULL;
	}
	
	if(instance->netHandle->sendQBuf)
	{
		ucm_free(UP_NULL, instance->netHandle->sendQBuf);
		instance->netHandle->sendQBuf = UP_NULL;
	}
	if(instance->netHandle->recvQBuf)
	{
		ucm_free(UP_NULL, instance->netHandle->recvQBuf);
		instance->netHandle->recvQBuf = UP_NULL;
	}
	
	ucm_free(UP_NULL, instance->netHandle);
	instance->netHandle = UP_NULL;
	
	return -1;
}

void srv_com_destroyVmsInstance(SRV_Instance *instance)
{
	if(instance->netHandle == UP_NULL)
	{
		return;
	}
	
	if(instance->netHandle->username)
	{
		ucm_free(UP_NULL, instance->netHandle->username);
		instance->netHandle->username = UP_NULL;
	}
	if(instance->netHandle->password)
	{
		ucm_free(UP_NULL, instance->netHandle->password);
		instance->netHandle->password = UP_NULL;
	}
	if(instance->netHandle->addr)
	{
		ucm_free(UP_NULL, instance->netHandle->addr);
		instance->netHandle->addr = UP_NULL;
	}
	
	if(instance->netHandle->mutex)
	{
		pthread_mutex_destroy((pthread_mutex_t*)instance->netHandle->mutex);
		free((void*)instance->netHandle->mutex);
		instance->netHandle->mutex = UP_NULL;
	}
	if(instance->netHandle->event)
	{
		pthread_cond_destroy((pthread_cond_t*)instance->netHandle->event);
		free((void*)instance->netHandle->event);
		instance->netHandle->event = UP_NULL;
	}
	
	if(instance->netHandle->sendQBuf)
	{
		ucm_free(UP_NULL, instance->netHandle->sendQBuf);
		instance->netHandle->sendQBuf = UP_NULL;
	}
	if(instance->netHandle->recvQBuf)
	{
		ucm_free(UP_NULL, instance->netHandle->recvQBuf);
		instance->netHandle->recvQBuf = UP_NULL;
	}
	
	ucm_free(UP_NULL, instance->netHandle);
	instance->netHandle = UP_NULL;	
}

UPSint srv_com_setUsrInfo(SRV_Instance *instance, UPSint8* name, UPSint8* password)
{
	if(instance == UP_NULL || instance->netHandle == UP_NULL)
	{
		UPLOG(srv_com_setUsrInfo, ("instance is NULL"));
		return -1;
	}
	
	if(name == UP_NULL || password == UP_NULL)
	{
		UPLOG(srv_com_setUsrInfo, ("FIXME: name or password is NULL"));
	}
	
	UPLOG(srv_com_setUsrInfo, ("set user information"));
	if(instance->netHandle->username)
	{
		ucm_free(UP_NULL, instance->netHandle->username);
		instance->netHandle->username = UP_NULL;
	}
	instance->netHandle->username = (UPSint8*)ucm_strdup(UP_NULL, name);
	
	if(instance->netHandle->password)
	{
		ucm_free(UP_NULL, instance->netHandle->password);
		instance->netHandle->password = UP_NULL;
	}
	instance->netHandle->password = (UPSint8*)ucm_strdup(UP_NULL, password);
	
	return 0;
}

UPSint srv_com_setNetInfo(SRV_Instance *instance, UPSint8* addr, UPSint32 port, UPUint type)
{
	if(instance == UP_NULL || instance->netHandle == UP_NULL)
	{
		UPLOG(srv_com_setNetInfo, ("instance is NULL"));
		return -1;
	}
	
	if(addr == UP_NULL)
	{
		UPLOG(srv_com_setNetInfo, ("FIXME: ip address is NULL"));
	}
	
	UPLOG(srv_com_setNetInfo, ("set net work information"));
	if(instance->netHandle->addr)
	{
		ucm_free(UP_NULL, instance->netHandle->addr);
		instance->netHandle->addr = UP_NULL;
	}
	
	instance->netHandle->addr = (UPSint8*)ucm_strdup(UP_NULL, addr);
	instance->netHandle->port = port;
	instance->netHandle->networkType = type;
	
	return 0;
}

UPSint srv_com_createFileInstance(SRV_Instance *instance, const UPSint8* path)
{
	instance->fileHandle = (FILE_Instance*)ucm_malloc(UP_NULL, sizeof(FILE_Instance));
	if(instance->fileHandle == UP_NULL)
	{
		return -1;
	}
	memset((void*)instance->fileHandle, 0x0, sizeof(FILE_Instance));
	
	instance->fileHandle->type = instance->type;

	instance->fileHandle->path = (UPSint8*)ucm_strdup(UP_NULL, (const UPSint8*)path);

	instance->fileHandle->size = 0;
	instance->fileHandle->readPos = 0;
	instance->fileHandle->basePos = 0;
	instance->fileHandle->foundIFrame = 0;

	instance->fileHandle->frameBuf = (UPSint8*)ucm_malloc(UP_NULL, 1024*1024);
	if(instance->fileHandle->frameBuf == UP_NULL)
	{
		fclose(instance->fileHandle->fp);
		instance->fileHandle->fp = UP_NULL;
		ucm_free(UP_NULL, instance->fileHandle->path);
		instance->fileHandle->path = UP_NULL;
		ucm_free(UP_NULL, instance->fileHandle);
		instance->fileHandle = UP_NULL;

		return -1;
	}
	instance->fileHandle->frameBufLen = 0;
	instance->fileHandle->frameBufSize = 1024*1024;

	return 0;
}

void srv_com_destroyFileInstance(SRV_Instance *instance)
{
	if((instance == UP_NULL) || (instance->fileHandle == UP_NULL))
	{
		return;
	}

	if(instance->fileHandle->fp)
	{
		fclose(instance->fileHandle->fp);
		instance->fileHandle->fp = UP_NULL;
	}
	if(instance->fileHandle->path)
	{
		ucm_free(UP_NULL, instance->fileHandle->path);
		instance->fileHandle->path = UP_NULL;
	}
	if(instance->fileHandle->frameBuf)
	{
		ucm_free(UP_NULL, instance->fileHandle->frameBuf);
		instance->fileHandle->frameBuf = UP_NULL;
	}
	if(instance->fileHandle)
	{
		ucm_free(UP_NULL, instance->fileHandle);
		instance->fileHandle = UP_NULL;
	}
}

UPSint srv_com_fileInstanceSetPath(SRV_Instance *instance, const UPSint8* path)
{
	if(instance->fileHandle == UP_NULL)
	{
		return -1;
	}
	if(path == UP_NULL)
	{
		UPLOG(srv_com_fileInstanceSetPath, ("invailable path"));
		return -1;
	}
	
	if(instance->fileHandle->path)
	{
		ucm_free(UP_NULL, instance->fileHandle->path);
		instance->fileHandle->path = UP_NULL;
	}
	
	instance->fileHandle->path = (UPSint8*)ucm_strdup(UP_NULL, (const UPSint8*)path);
	
	return 0 ;
}

UPSint srv_com_fileInstanceGetPos(SRV_Instance *instance)
{
	if(instance->fileHandle == UP_NULL)
	{
		return -1;
	}
	if(instance->fileHandle->size == 0)
	{
		return -1;
	}
	
	return ((((UPUint64)(instance->fileHandle->readPos))*1000)/instance->fileHandle->size);
}

UPSint srv_com_fileInstanceSetPos(SRV_Instance *instance, UPSint pos)
{
	if(pos < 0 || instance->fileHandle == UP_NULL)
	{
		return -1;
	}
	
	instance->fileHandle->basePos = pos;

	return 0;
}

UPSint srv_com_fileInstanceSetSize(SRV_Instance *instance, UPUint size)
{
	if(instance == UP_NULL)
	{
		return -1;
	}
	
	if(instance->type != SRV_ID_SZPBONLINE)
	{
		return 0;
	}
	
	UPLOG(srv_com_fileInstanceSetSize, ("size: %d", size));
	
	instance->dstFileSize = size;

	return 0;
}

UPSint srv_com_audioPoolReset(SRV_Instance *instance)
{
	if(instance == UP_NULL)
	{
		return -1;
	}
	
	if(instance->audioPool == UP_NULL)
	{
		return -1;
	}
	
	rc_pool_reset(instance->audioPool);
	
	return 0;
	
}

UPSint srv_com_poolReset(SRV_Instance *instance)
{
	if(instance == UP_NULL)
	{
		return -1;
	}

	if(instance->videoPool == UP_NULL)
	{
		return -1;
	}

	rc_pool_reset(instance->videoPool);

	return 0;
}

PRC_data_entry_str srv_com_audioPoolPop(SRV_Instance *instance)
{
	PRC_data_entry_str pEntry = UP_NULL;
	
	if(instance == UP_NULL)
	{
		return UP_NULL;
	}
	
	pEntry = (PRC_data_entry_str)rc_pool_poolPop(instance->audioPool);
	
	return pEntry;
}

PRC_data_entry_str srv_com_poolPop(SRV_Instance *instance)
{
	PRC_data_entry_str pEntry = UP_NULL;

	pEntry = (PRC_data_entry_str)rc_pool_poolPop(instance->videoPool);

	return pEntry;
}

PRC_data_entry_str srv_com_audioPoolPush(SRV_Instance *instance, PRC_data_entry_str data)
{
	if(instance == UP_NULL)
	{
		return UP_NULL;
	}
	
	return rc_pool_poolPush(instance->audioPool, data);
}

PRC_data_entry_str srv_com_poolPush(SRV_Instance *instance, PRC_data_entry_str data)
{
	return rc_pool_poolPush(instance->videoPool, data);
}

PRC_data_entry_str srv_com_audioListPop(SRV_Instance *instance)
{
	PRC_data_entry_str pEntry = UP_NULL;
	
	if(instance == UP_NULL)
	{
		return UP_NULL;
	}
	
	pEntry = (PRC_data_entry_str)rc_pool_listPop(instance->audioPool);
	
	return pEntry;
}

PRC_data_entry_str srv_com_listPop(SRV_Instance *instance)
{
	PRC_data_entry_str pEntry = UP_NULL;

	pEntry = (PRC_data_entry_str)rc_pool_listPop(instance->videoPool);

	return pEntry;
}

PRC_data_entry_str srv_com_audioListPush(SRV_Instance *instance, PRC_data_entry_str data)
{
	if(instance == UP_NULL)
	{
		return UP_NULL;
	}
	
	return rc_pool_listPush(instance->audioPool, data);
}

PRC_data_entry_str srv_com_listPush(SRV_Instance *instance, PRC_data_entry_str data)
{
	return rc_pool_listPush(instance->videoPool, data);
}

#include <pthread.h>
UPSint srv_com_start(SRV_Instance *instance)
{
	int ret = 0;
	pthread_t th_id;
	if(instance == UP_NULL)
	{
		UPLOG(srv_com_start, ("must create SRV_Instance first"));
		return -1;
	}

	instance->videoStreamStatus = SRV_Thread_status_running;
	instance->netStatus = SRV_NETWORK_NOTCREATE;

	if(instance->type == SRV_ID_SZRAWDATA || instance->type == SRV_ID_SZPBONLINE)
	{
		ret = pthread_create(&th_id, NULL, vs_file_main, (void*)instance);
		if(ret != 0)
		{
			UPLOG(srv_com_start, ("create videostream thread error"));
			instance->videoStreamStatus = SRV_Thread_status_exited;
			return -1;
		}
		instance->videoStream = th_id;
	}
	else if(instance->type == SRV_ID_SZVMS)
	{
		ret = pthread_create(&th_id, NULL, vs_vms_main, instance);
		if(ret != 0)
		{
			UPLOG(srv_com_start, ("create videostream thread error"));
			instance->videoStreamStatus = SRV_Thread_status_exited;
			return -1;
		}
		instance->videoStream = th_id;
	}

	UPLOG(srv_com_start, ("create videostream thread success"));

	return 0;
}

UPSint srv_com_stop(SRV_Instance *instance)
{
	if(instance == UP_NULL)
	{
		return 0;
	}

	pthread_mutex_lock((pthread_mutex_t*)instance->videoStreamMutex);
	if(instance->videoStreamStatus == SRV_Thread_status_exited || instance->videoStreamStatus == SRV_Thread_status_notCreate)
	{
		pthread_mutex_unlock((pthread_mutex_t*)instance->videoStreamMutex);
		return 0;
	}
	if(instance->videoStreamStatus == SRV_Thread_status_idle)
	{
		pthread_cond_signal((pthread_cond_t*)instance->videoStreamEvent);
		instance->videoStreamStatus = SRV_Thread_status_exitReq;
	}
	else
	{
		instance->videoStreamStatus = SRV_Thread_status_exitReq;
	}
	pthread_mutex_unlock((pthread_mutex_t*)instance->videoStreamMutex);

	while(instance->videoStreamStatus == SRV_Thread_status_exitReq)
	{
		pthread_mutex_lock((pthread_mutex_t*)instance->videoStreamMutex);
		up_event_wait(instance->videoStreamEvent, instance->videoStreamMutex, 10);
		pthread_mutex_unlock((pthread_mutex_t*)instance->videoStreamMutex);
	}

	UPLOG(srv_com_stop, ("videostream thread stop finish"));

	return 0;
}

UPSint srv_com_restart(SRV_Instance *instance)
{
	UPSint ret;
	ret = srv_com_stop(instance);
	ret = srv_com_start(instance);
	
	if(ret != 0)
		UPLOG(srv_com_restart, ("srv com restart fail"));
	
	return 0;
}

UPSint srv_com_pause(SRV_Instance *instance)
{
	
	return 0;
}

UPSint srv_com_continue(SRV_Instance *instance)
{
	
	return 0;
}

UPSint srv_com_Record(SRV_Instance *instance)
{
	instance->recFlag = 1;
	return 0;
}

UPSint srv_com_StopRecord(SRV_Instance *instance)
{
	instance->recFlag = 0;
	return 0;
}

UPSint srv_com_ptzControl(SRV_Instance *instance, PTZControlAction action, UPUint para)
{
	UPSint ret;
	
	ret = vs_vms_ptzControl(instance->netHandle->addr, instance->netHandle->port, instance->netHandle->videoChannel, action, para);
	
	return ret;
}

UPUint srv_com_vmsGetCalenda(UPSint8 *addr, UPUint32 port, UPUint8 year, UPUint8 month, UPUint8 day)
{
	UPSint ret;
	UPUint calendar;
	
	ret = vs_vms_getCalenda(addr, port, 0, year, month, day, 0, &calendar);
	if(ret != 0)
	{
		UPLOG(srv_com_vms_Calenda, ("get calenda error"));
		calendar = 0xffff;
	}
	
	return calendar;
}

UPSint srv_com_vmsGetFileNum(UPSint8 *addr, UPSint port, UPUint channel, UPUint8 year, 
							 UPUint8 month, UPUint8 day, UPSint type)
{
	UPSint ret;
	UPUint number = 0;
	UPLOG(srv_com_vmsGetFileNum, ("Start"));
	
	ret = vs_vms_searchGetNum(addr, port, channel, year, month, day, type, &number);
	UPLOG(srv_com_vmsGetFileNum, ("File Number: %d", number));
	if(ret != 0)
	{
		return -1;
	}
	UPLOG(srv_com_vmsGetFileNum, ("file number: %d", number));
	
	return number;
}

UPSint8* srv_com_vmsGetFilePage(UPSint8 *addr, UPSint port, UPUint channel, UPUint8 year, 
							  UPUint8 month, UPUint8 day, UPSint type, UPUint page, UPUint *fileNum)
{
	UPSint ret;
	UPSint8 *result = NULL;
	UPLOG(srv_com_vmsGetFilePage, ("Start"));
	
	ret = vs_vms_searchGetPage(addr, port, channel, year, month, day, type, page, &result, fileNum);

	return result;
}

UPSint8* srv_com_vmsGetFileList(UPSint8 *addr, UPSint port, UPUint channel, UPUint8 year, UPUint8 month, UPUint8 day, UPSint type, UPUint *fileNum)
{
	UPSint ret;
	UPSint8 *result;
	
	ret = vs_vms_searchFile(addr, port, channel, year, month, day, type, &result, fileNum);
	if(ret != 0)
	{
		return UP_NULL;
	}
	
	return result;
}

UPSint8* srv_com_vmsGetFileList2(UPSint8 *addr, UPSint port, UPUint channel, UPUint8 year, UPUint8 month, UPUint8 day, UPSint type, UPUint *fileNum, UPSint *retNum)
{
	UPSint ret;
	UPSint8 *result;
	
	ret = vs_vms_searchFile(addr, port, channel, year, month, day, type, &result, fileNum);
	if(ret != 0)
	{
		*retNum = ret;
		return UP_NULL;
	}
	
	*retNum = ret;
	return result;
}

UPSint srv_com_vmsGetChannelNum(UPSint8 *addr, UPSint port)
{
	if(addr == UP_NULL)
	{
		UPLOG(srv_com_vmsGetChannelNum, ("invailable ip address"));
		return -1;
	}
	
	return vs_vms_getChnNumber(addr, port);
}

UPSint srv_com_vmsLogin(UPSint8 *addr, UPSint port, UPUint8 *userName, UPUint8 *password)
{
	return vs_vms_login(addr, port, userName, password);
}

UPSint srv_com_notify(SRV_Instance *instance)
{
	if(instance->videoStreamStatus == SRV_Thread_status_idle)
	{
		pthread_mutex_lock((pthread_mutex_t*)instance->videoStreamMutex);
		pthread_cond_signal((pthread_cond_t*)instance->videoStreamEvent);
		instance->videoStreamStatus = SRV_Thread_status_running;
		pthread_mutex_unlock((pthread_mutex_t*)instance->videoStreamMutex);
	}

	return 0;
}

UPSocketHandle srv_com_connect(UPSint8* addr, UPSint32 port)
{
	UPSint32 ret;
	UPSint needDNS = 0;
	UPSocketHandle socket = NULL;
	
	socket = up_sock_open();
	if(socket == NULL)
	{
		return UP_NULL;
	}
	
	UPLOG(srv_com_connect, ("srv_com_connect add DNS parser"));
	UPLOG(srv_com_connect, ("addr: %s", addr));
	if(util_str_isIPAddress((unsigned char*)addr, strlen((const char*)addr)) == 1)
	{
		needDNS = 0;
	}
	else
	{
		needDNS = 1;
	}
	
	ret = up_sock_connect(socket, addr, strlen((const char*)addr), port, 5000, needDNS);
	if(ret != UP_SOCKET_SUCCESS)
	{
		up_sock_close(socket);
		return UP_NULL;
	}
	
	return socket;
}

SRV_Network_Status srv_com_getNetworkStatus(SRV_Instance *instance)
{
	SRV_Network_Status status;
	
	if(instance == UP_NULL)
	{
		return SRV_NETWORK_NOTCREATE;
	}

	status = instance->netStatus;
	
	return status;
}

SRV_Download *srv_com_downloadCreate()
{
	SRV_Download *instance = UP_NULL;
	
	instance = (SRV_Download*)ucm_malloc(UP_NULL, sizeof(SRV_Download));
	if(instance == UP_NULL)
	{
		UPLOG(srv_com_downloadCreate, ("SRV_Download not enought memory"));
		return UP_NULL;
	}
	memset((void*)instance, 0x0, sizeof(SRV_Download));
	
	instance->thMutex = (UpMutexHandle)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((pthread_mutex_t*)instance->thMutex, NULL);
	instance->thEvent = (UpEventHandle)malloc(sizeof(pthread_cond_t));
	pthread_cond_init((pthread_cond_t*)instance->thEvent, NULL);
	
	instance->thStatus = SRV_Thread_status_notCreate;
	instance->downloadStatus = SRV_DOWNLOAD_NOTSTART;
	
	instance->recvQBuf = (UPSint8*)ucm_malloc(UP_NULL, 256*1024);
	instance->recvQBufSize = 256*1024;
	instance->recvQBufLen = 0;
	
	instance->filePath = UP_NULL;
	memset(instance->savePath, 0x0, sizeof(instance->savePath));
	memset(instance->saveName, 0x0, sizeof(instance->saveName));
	instance->saveFp = UP_NULL;

	return instance;
}

void srv_com_downloadDestroy(SRV_Download *instance)
{
	if(instance == UP_NULL)
	{
		UPLOG(srv_com_downloadDestroy, ("SRV_Download invailable parameter"));
		return ;
	}
	
	/*check the thread status, stop it if need*/
	srv_com_downloadStop(instance);
	
	if(instance->addr)
	{
		ucm_free(UP_NULL, instance->addr);
		instance->addr = UP_NULL;
	}
	
	if(instance->filePath)
	{
		ucm_free(UP_NULL, instance->filePath);
		instance->filePath;
	}
	
	if(instance->recvQBuf)
	{
		ucm_free(UP_NULL, instance->recvQBuf);
		instance->recvQBuf = UP_NULL;
	}
	
	if(instance->saveFp)
	{
		fclose(instance->saveFp);
		instance->saveFp = UP_NULL;
	}
	
	ucm_free(UP_NULL, instance);
	instance = UP_NULL;
	
	return ;
	
}

UPSint srv_com_downloadSetup(SRV_Download *instance, UPSint8 *addr, UPSint32 port)
{
	if(instance == UP_NULL)
	{
		UPLOG(srv_com_downloadSetup, ("SRV_Download invailable parameter"));
		return -1;
	}
	
	UPLOG(srv_com_downloadSetup, ("server address: %s, port: %d", addr, port));
	
	if(addr)
	{
		if(instance->addr)
		{
			ucm_free(UP_NULL, instance->addr);
			instance->addr = UP_NULL;
		}
		instance->addr = (UPSint8*)ucm_strdup(UP_NULL, (const char*)addr);
	}
	
	instance->port = port;
	
	return 0;
}

UPSint srv_com_downloadSetFileInfo(SRV_Download *instance, UPSint8 *path, UPUint fileSize, UPUint offset)
{
	if(instance == UP_NULL)
	{
		UPLOG(srv_com_downloadSetFileInfo, ("SRV_Download invailable parameter"));
		return -1;
	}
	
	if(path)
	{
		if(instance->filePath)
		{
			ucm_free(UP_NULL, instance->filePath);
			instance->filePath = UP_NULL;
		}
		instance->filePath = (UPSint8*)ucm_strdup(UP_NULL, (const char*)path);
	}
	
	instance->fileSize = fileSize;
	instance->startOffset = offset;
	instance->recvSize = 0;
	
	return 0;
}

UPSint srv_com_downloadSetSavePath(SRV_Download *instance, UPSint8 *savePath)
{
	if(instance == UP_NULL)
	{
		UPLOG(srv_com_downloadSetSavePath, ("SRV_Download invailable parameter"));
		return -1;
	}
	
	if(savePath)
	{
		UPLOG(srv_com_downloadSetSavePath, ("SRV_Download save path: %s", savePath));
		memset((void*)instance->savePath, 0x0, sizeof(instance->savePath));
		/*maybe overflow*/
		strcpy((char*)instance->savePath, (char*)savePath);
	}
	
	return 0;
}

UPSint srv_com_downloadSetSaveName(SRV_Download *instance, UPSint8 *saveName)
{
	if(instance == UP_NULL)
	{
		UPLOG(srv_com_downloadSetSaveName, ("SRV_Download invailable parameter"));
		return -1;
	}
	
	if(saveName)
	{
		UPLOG(srv_com_downloadSetSaveName, ("SRV_Download save name: %s", saveName));
		memset((void*)instance->saveName, 0x0, sizeof(instance->saveName));
		/*maybe overflow*/
		strcpy((char*)instance->saveName, (char*)saveName);
	}
	
	return 0;
}

UPSint srv_com_downloadGetPrecent(SRV_Download *instance)
{
	UPSint ret = 0;
	
	if(instance == UP_NULL)
	{
		UPLOG(srv_com_downloadGetPrecent, ("SRV_Download invailable parameter"));
		return -1;
	}
	
	if(instance->fileSize == 0)
	{
		return 0;
	}
	
	ret = (((UPUint64)(instance->recvSize + instance->startOffset))*1000)/instance->fileSize;
	
	return ret;
}

UPSint srv_com_downloadStart(SRV_Download *instance)
{
	int ret;
	pthread_t th_id;
	if(instance == UP_NULL)
	{
		UPLOG(srv_com_downloadStart, ("SRV_Download invailable parameter"));
		return -1;
	}
	instance->thStatus = SRV_Thread_status_running;
	
	ret = pthread_create(&th_id, NULL, vs_vms_downloadMain, (void*)instance);
	if(ret != 0)
	{
		UPLOG(srv_com_downloadStart, ("SRV_Download create thread error"));
		instance->thStatus = SRV_Thread_status_exited;
		return -1;
	}
	instance->thHandler = th_id;
	
	UPLOG(srv_com_downloadStart, ("SRV_Download start success"));
	
	return 0;
}

UPSint srv_com_downloadStop(SRV_Download *instance)
{
	if(instance == UP_NULL)
	{
		UPLOG(srv_com_downloadStop, ("SRV_Download invailable parameter"));
		return -1;
	}
	
	/*tell server to stop*/
	if(instance->downloadStatus == SRV_DOWNLOAD_ONGOING)
	{
		//vs_vms_downloadStop(instance->addr, instance->port);
	}
	
	pthread_mutex_lock((pthread_mutex_t*)instance->thMutex);
	if(instance->thStatus == SRV_Thread_status_exited || instance->thStatus == SRV_Thread_status_notCreate)
	{
		pthread_mutex_unlock((pthread_mutex_t*)instance->thMutex);
		return 0;
	}
	if(instance->thStatus == SRV_Thread_status_idle)
	{
		pthread_cond_signal((pthread_cond_t*)instance->thEvent);
		instance->thStatus = SRV_Thread_status_exitReq;
	}
	else
	{
		instance->thStatus = SRV_Thread_status_exitReq;
	}
	pthread_mutex_unlock((pthread_mutex_t*)instance->thMutex);
	
	while(instance->thStatus == SRV_Thread_status_exitReq)
	{
		pthread_mutex_lock((pthread_mutex_t*)instance->thMutex);
		up_event_wait(instance->thEvent, instance->thMutex, 10);
		pthread_mutex_unlock((pthread_mutex_t*)instance->thMutex);
	}
	
	UPLOG(srv_com_downloadStop, ("SRV_Download stop success"));
	
	return 0;
}

#include <sys/stat.h>
UPSint up_fs_fileExist(const char *fileName)
{
	int ret;
	struct stat st;
	
	ret = lstat(fileName, &st);
	if(ret == 0)
	{
		return 1;
	}
	
	return 0;
}

#include <errno.h>
UPSint up_event_wait(UpEventHandle cond, UpMutexHandle mux, UPSint duration)
{
	int rc = 0;
	struct timespec timeout;
	
    if(cond == UP_NULL || mux == UP_NULL)
	{
        return UP_EVENT_WAIT_FAILURE;
    }
	
	if(duration == UP_EVENT_WAIT_INFINITE || duration < 0)
	{
		//wait forever
        rc = pthread_cond_wait((pthread_cond_t *)(cond), (pthread_mutex_t *)(mux));
	}
	else
	{
		//get the start time
		gettimeofday(&timeout, NULL);
        
		//get the due time
		timeout.tv_sec  +=  (duration / 1000);
		timeout.tv_nsec +=  (duration % 1000) * 1000000;
	  	if (timeout.tv_nsec >= 1000000000) 
		{
			++timeout.tv_sec;
	  		timeout.tv_nsec -= 1000000000;
	  	}
		//wait begining
		rc = pthread_cond_timedwait((pthread_cond_t *)cond,(pthread_mutex_t *)mux, &timeout);
	}
	
	switch(rc)
	{
		case 0:
			return UP_EVENT_WAIT_OK;
		case ETIMEDOUT:
			return UP_EVENT_WAIT_TIMEOUT;
		case EINVAL:
			return UP_EVENT_WAIT_FAILURE;
		default:
			return UP_EVENT_WAIT_FAILURE;
	}
}