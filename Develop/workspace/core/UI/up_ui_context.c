#include "up_types.h"
#include "ucm.h"
#include "up_sys_log.h"

#include <pthread.h>

#include "srv_com_handler.h"
#include "up_ui_context.h"

UPUI_VideoCtx * upui_ctx_create(const UPSint8* addr, UPSint32 port)
{
	UPUI_VideoCtx *videoCtx = UP_NULL;

	videoCtx = (UPUI_VideoCtx*)ucm_malloc(UP_NULL, sizeof(UPUI_VideoCtx));
	if(videoCtx == UP_NULL)
	{
		UPLOG(upui_ctx_create, ("Create ctx error, please exit!"));
		return UP_NULL;
	}

	memset((void*)videoCtx, 0x0, sizeof(UPUI_VideoCtx));

	if(addr != UP_NULL)
	{
		videoCtx->addr = (UPSint8*)ucm_strdup(UP_NULL, (const char*)addr);
	}
	videoCtx->port = port;

	videoCtx->username = UP_NULL;
	videoCtx->password = UP_NULL;
	videoCtx->videoChannel = 0;
	videoCtx->videoWidth = 0;
	videoCtx->videoHeight = 0;

	videoCtx->processData = UP_NULL;
	videoCtx->processMutex = (UpMutexHandle)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((pthread_mutex_t*)videoCtx->processMutex, NULL);
	videoCtx->processEvent = (UpEventHandle)malloc(sizeof(pthread_cond_t));
	pthread_cond_init((pthread_cond_t*)videoCtx->processEvent, NULL);
	videoCtx->processStatus = SRV_Thread_status_notCreate;
	videoCtx->dataProcess = UP_NULL;
	videoCtx->paramData = UP_NULL;

	videoCtx->srvInstance = UP_NULL;

	return videoCtx;
}

void upui_ctx_destroy(UPUI_VideoCtx* ctx)
{
	if(ctx == UP_NULL)
	{
		return;
	}

	if(ctx->addr)
	{
		ucm_free(UP_NULL, ctx->addr);
		ctx->addr = UP_NULL;
	}

	if(ctx->username)
	{
		ucm_free(UP_NULL, ctx->username);
		ctx->username = UP_NULL;
	}

	if(ctx->password)
	{
		ucm_free(UP_NULL, ctx->password);
		ctx->password = UP_NULL;
	}

	if(ctx->processMutex)
	{
		pthread_mutex_destroy((pthread_mutex_t*)ctx->processMutex);
		free((void*)ctx->processMutex);
		ctx->processMutex = UP_NULL;
	}

	if(ctx->processEvent)
	{
		pthread_cond_destroy((pthread_cond_t*)ctx->processEvent);
		free((void*)ctx->processEvent);
		ctx->processEvent = UP_NULL;
	}

	if(ctx->srvInstance)
	{
		upui_ctx_finiSrv(ctx);
		ctx->srvInstance = UP_NULL;
	}

	if(ctx)
	{
		ucm_free(UP_NULL, ctx);
		ctx = UP_NULL;
	}

}

UPSint upui_ctx_initSrv(UPUI_VideoCtx* ctx)
{
	UPSint ret = 0;
	if(ctx == UP_NULL)
	{
		return -1;
	}

	switch(ctx->type)
	{
		case SRV_ID_SZRAWDATA:
		case SRV_ID_SZPBONLINE:
			ctx->srvInstance = srv_com_create(ctx->type, (const UPSint8*)"h264rawdata");
			if(ctx->srvInstance == UP_NULL)
			{
				UPLOG(upui_ctx_init, ("create srv instance error"));
				return -1;
			}

			/*for debug*/
			ret = srv_com_createFileInstance(ctx->srvInstance, (const UPSint8*)"/Users/kelvin/Develop/ffmpeg-dvr/test/multiThread/test8.264");
			if(ret != 0)
			{
				UPLOG(upui_ctx_init, ("create srv file instance error"));
				return -1;
			}
			break;
		case SRV_ID_SZVMS:
			ctx->srvInstance = srv_com_create(SRV_ID_SZVMS, (const UPSint8*)"h264szvms");
			if(ctx->srvInstance == UP_NULL)
			{
				UPLOG(upui_ctx_init, ("create srv instance error"));
				return -1;
			}
			ret = srv_com_createVmsInstance(ctx->srvInstance, "192.168.0.135", 9997, "kelvin", "123456");
			if(ret != 0)
			{
				UPLOG(upui_ctx_init, ("create srv vms instance error"));
				return -1;
			}
			break;
	}

	return 0;
}

void upui_ctx_finiSrv(UPUI_VideoCtx* ctx)
{
	if(ctx == UP_NULL)
	{
		return;
	}

	if(ctx->srvInstance)
	{
		srv_com_destroyFileInstance(ctx->srvInstance);
		srv_com_destroyVmsInstance(ctx->srvInstance);
	}

	srv_com_destroy(ctx->srvInstance);

	ctx->srvInstance = UP_NULL;
}

void upui_ctx_setHandleCB(UPUI_VideoCtx* ctx, process_cb handlecb, void* param)
{
	if(ctx == UP_NULL)
	{
		return;
	}

	ctx->dataProcess = handlecb;
	ctx->paramData = param;
}

static void upui_ctx_main(void* data)
{
	PRC_data_entry_str pEntry = UP_NULL;
	UPUI_VideoCtx *ctx = (UPUI_VideoCtx*)data;

	if(ctx == UP_NULL)
	{
		UPLOG(upui_ctx_main, ("please set the ctx"));
		return;
	}

	while(ctx->processStatus == SRV_Thread_status_running || ctx->processStatus == SRV_Thread_status_idle)
	{
		if(UP_NULL == (pEntry = srv_com_listPop(ctx->srvInstance)))
		{
			pthread_mutex_lock((pthread_mutex_t*)ctx->processMutex);
			ctx->processStatus = SRV_Thread_status_idle;
			up_event_wait(ctx->processEvent, ctx->processMutex, UP_EVENT_WAIT_INFINITE);
			pthread_mutex_unlock((pthread_mutex_t*)ctx->processMutex);
			//usleep(20000);
			continue;
		}

		if(pEntry == UP_NULL)
		{
			continue;
		}

		if(ctx->dataProcess != UP_NULL)
		{
			ctx->dataProcess(ctx->paramData, pEntry, pEntry->dataLen, 0, 0);
		}

		srv_com_poolPush(ctx->srvInstance, pEntry);

		srv_com_notify(ctx->srvInstance);
		//usleep(40000);
	}

	pthread_mutex_lock((pthread_mutex_t*)ctx->processMutex);
	ctx->processStatus = SRV_Thread_status_exited;
	pthread_mutex_unlock((pthread_mutex_t*)ctx->processMutex);

	UPLOG(upui_ctx_main, ("processData thread exit!"));
}

#include <pthread.h>
UPSint upui_ctx_start(UPUI_VideoCtx *ctx)
{
	int ret = 0;
	pthread_t th_id;
	if(ctx == UP_NULL)
	{
		UPLOG(upui_ctx_start, ("please create video context first!"));
		return -1;
	}

	ctx->processStatus = SRV_Thread_status_running;
	ret = pthread_create(&th_id, NULL, upui_ctx_main, (void*)ctx);
	if(ret != 0)
	{
		UPLOG(upui_ctx_start, ("create processData thread error"));
		ctx->processStatus = SRV_Thread_status_exited;
		return -1;
	}
	ctx->processData = th_id;

	UPLOG(upui_ctx_start, ("create processData thread success"));

	return 0;
}

UPSint upui_ctx_stop(UPUI_VideoCtx *ctx)
{
	if(ctx == UP_NULL)
	{
		return 0;
	}

	pthread_mutex_lock((pthread_mutex_t*)ctx->processMutex);
	if(ctx->processStatus == SRV_Thread_status_exited || ctx->processStatus == SRV_Thread_status_notCreate)
	{
		pthread_mutex_unlock((pthread_mutex_t*)ctx->processMutex);
		return 0;
	}

	if(ctx->processStatus == SRV_Thread_status_idle)
	{
		pthread_cond_signal((pthread_cond_t*)ctx->processEvent);
		ctx->processStatus = SRV_Thread_status_exitReq;
	}
	else
	{
		ctx->processStatus = SRV_Thread_status_exitReq;
	}
	pthread_mutex_unlock((pthread_mutex_t*)ctx->processMutex);

	while(ctx->processStatus == SRV_Thread_status_exitReq)
	{
		pthread_mutex_lock((pthread_mutex_t*)ctx->processMutex);
		up_event_wait(ctx->processEvent, ctx->processMutex, 10);
		pthread_mutex_unlock((pthread_mutex_t*)ctx->processMutex);
	}

	UPLOG(upui_ctx_stop, ("processData thread stop finish"));

	return 0;
}

UPSint upui_ctx_notify(UPUI_VideoCtx *ctx)
{
	if(ctx->processStatus == SRV_Thread_status_idle)
	{
		pthread_mutex_lock((pthread_mutex_t*)ctx->processMutex);
		pthread_cond_signal((pthread_cond_t*)ctx->processEvent);
		ctx->processStatus = SRV_Thread_status_running;
		pthread_mutex_unlock((pthread_mutex_t*)ctx->processMutex);
	}
	
	return 0;
}
