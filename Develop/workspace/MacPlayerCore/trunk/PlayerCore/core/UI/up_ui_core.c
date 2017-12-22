#include "up_types.h"
#include "ucm.h"
#include "up_sys_log.h"

#include "srv_com_handler.h"
#include "up_ui_context.h"
#include "up_ui_core.h"

static UPSint m_fw_init = 0;
UPUI_VideoCtx *gVideoCtx = UP_NULL;

static UPSint8 m_Version[32];

UPSint upui_fw_versionPrint()
{
	UPSint version = MODULE_VERSION;
	
	//big endian
	printf("\n\n\nVersion: %d.%d.%d\n\n\n", (version>>16)&0xff, (version>>8)&0xff, version&0xff);
	
	return 0;
}

UPSint8* upui_fw_getVersionStr()
{
	UPSint version = MODULE_VERSION;
	memset(m_Version, 0x0, sizeof(m_Version));
	
	//big endian
	sprintf(m_Version, "%d.%d.%d", (version>>16)&0xff, (version>>8)&0xff, version&0xff);

	return m_Version;
}

UPSint upui_fw_init()
{
	UPLOG(upui_fw_init, ("fw init"));
	upui_fw_versionPrint();
	printf("Version: %s\n", upui_fw_getVersionStr());
	if(m_fw_init == 1)
	{
		UPLOG(upui_fw_init, ("fw already init"));
		return 0;
	}

	UPLOG(upui_fw_init, ("fw init2"));
	up_util_init(UP_NULL);

	m_fw_init = 1;

	return 0;
}

UPSint upui_fw_fini()
{
	if(m_fw_init == 0)
	{
		return 0;
	}

	up_util_fini();

	m_fw_init = 0;

	return 0;
}

UPUI_VideoCtx* upui_fw_initCtx(UPUint channel, UPSint type)
{
	int ret = 0;
	UPUI_VideoCtx *instance;
	instance = upui_ctx_create((const UPSint8*)"192.168.0.188", 15965);
	if(instance == UP_NULL)
	{
		return UP_NULL;
	}
	
	if(type == 1)
	{
		instance->type = SRV_ID_SZRAWDATA;
	}
	else if(type == 2)
	{
		instance->type = SRV_ID_SZPBONLINE;
	}
	else if(type == 0)
	{
		instance->type = SRV_ID_SZVMS;
	}

	instance->videoChannel = channel;
	ret = upui_ctx_initSrv(instance);
	if(ret != 0)
	{
		UPLOG(upui_fw_initCtx, ("init ctx faile"));
		upui_ctx_destroy(instance);
		instance = UP_NULL;
		return UP_NULL;
	}
	
	instance->srvInstance->partner = instance;

	return instance;
}

void upui_fw_finiCtx(UPUI_VideoCtx *ctx)
{
	upui_ctx_destroy(ctx);
}

void upui_fw_setCtxChannel(UPUI_VideoCtx* instance, UPUint channel)
{
	instance->videoChannel = channel;
	srv_com_setChannel(instance->srvInstance, channel);
}

UPUint upui_fw_getCtxChannel(UPUI_VideoCtx* instance)
{
	if(instance == UP_NULL)
	{
		UPLOG(upui_fw_getCtxChannel, ("instance is NULL"));
		return 0;
	}
	
	return instance->videoChannel;
}

UPSint upui_fw_setCtxUsrInfo(UPUI_VideoCtx* instance, UPSint8* name, UPSint8* password)
{
	if(instance == UP_NULL || instance->srvInstance == UP_NULL)
	{
		UPLOG(upui_fw_setCtxUsrInfo, ("UPUI_VideoCtx is NULL"));
		return -1;
	}
	
	if(name == UP_NULL || password == UP_NULL)
	{
		UPLOG(upui_fw_setCtxUsrInfo, ("name/password is NULL"));
	}
	
	if(instance->username)
	{
		ucm_free(UP_NULL, instance->username);
		instance->username = UP_NULL;
	}
	instance->username = (UPSint8*)ucm_strdup(UP_NULL, (UPUint8*)name);
	
	if(instance->password)
	{
		ucm_free(UP_NULL, instance->password);
		instance->password = UP_NULL;
	}
	instance->password = (UPSint8*)ucm_strdup(UP_NULL, (UPUint8*)password);
	
	
	
	return srv_com_setUsrInfo(instance->srvInstance, name, password);
}

UPSint upui_fw_setCtxNetInfo(UPUI_VideoCtx* instance, UPSint8* addr, UPSint32 port, UPUint type)
{
	if(instance == UP_NULL || instance->srvInstance == UP_NULL)
	{
		UPLOG(upui_fw_setCtxNetInfo, ("UPUI_VideoCtx is NULL"));
		return -1;
	}
	
	if(addr == UP_NULL)
	{
		UPLOG(upui_fw_setCtxNetInfo, ("ip address is NULL"));
	}
	
	if(instance->addr)
	{
		ucm_free(UP_NULL, instance->addr);
		instance->addr = UP_NULL;
	}
	instance->addr = (UPSint8*)ucm_strdup(UP_NULL, addr);
	
	instance->port = port;
	
	return srv_com_setNetInfo(instance->srvInstance, addr, port, type);
}

UPSint upui_fw_setCtxFilePath(UPUI_VideoCtx* instance, const UPSint8* path)
{
	UPSint ret;
	if(instance == UP_NULL)
	{
		return -1;
	}
	
	ret = srv_com_fileInstanceSetPath(instance->srvInstance, path);

	return ret;
}

UPSint upui_fw_startSrv(UPUI_VideoCtx *ctx)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}

	return srv_com_start(ctx->srvInstance);
}

UPSint upui_fw_stopSrv(UPUI_VideoCtx *ctx)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}

	return srv_com_stop(ctx->srvInstance);
}

UPSint upui_fw_startSrvRecord(UPUI_VideoCtx *ctx)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_Record(ctx->srvInstance);
}

UPSint upui_fw_stopSrvRecord(UPUI_VideoCtx *ctx)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_StopRecord(ctx->srvInstance);
}

UPSint upui_fw_getSrvFilePos(UPUI_VideoCtx *ctx)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_fileInstanceGetPos(ctx->srvInstance);   
}

UPSint upui_fw_setSrvFilePos(UPUI_VideoCtx *ctx, UPSint pos)
{
	if(ctx == UP_NULL)
	{
		UPLOG(upui_fw_setSrvFilePos, ("parameter error"));
		return -1;
	}
	
	return srv_com_fileInstanceSetPos(ctx->srvInstance, pos);
}

UPSint upui_fw_setSrvFileSize(UPUI_VideoCtx *ctx, UPUint size)
{
	if(ctx == UP_NULL)
	{
		UPLOG(upui_fw_setSrvFileSize, ("parameter error"));
		return -1;
	}
	
	UPLOG(upui_fw_setSrvFileSize, ("size: %d", size));
	return srv_com_fileInstanceSetSize(ctx->srvInstance, size);
}

UPSint upui_fw_startCtx(UPUI_VideoCtx *ctx)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}

	return upui_ctx_start(ctx);
}

UPSint upui_fw_stopCtx(UPUI_VideoCtx *ctx)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}

	return upui_ctx_stop(ctx);
}

UPSint upui_fw_setHandleCBCtx(UPUI_VideoCtx *ctx, process_cb handlecb, void *param)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}

	upui_ctx_setHandleCB(ctx, handlecb, param);

	return 0;
}

UPSint upui_fw_resetFramePool(UPUI_VideoCtx *ctx)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}

	return srv_com_poolReset(ctx->srvInstance);
}

UPSint upui_fw_getNetworkStatus(UPUI_VideoCtx *ctx)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_getNetworkStatus(ctx->srvInstance);
}

UPSint upui_fw_srvPtzUp(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_UP, para);
}

UPSint upui_fw_srvPtzDown(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_DOWN, para);
}

UPSint upui_fw_srvPtzLeft(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_LEFT, para);
}

UPSint upui_fw_srvPtzRight(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_RIGHT, para);
}

UPSint upui_fw_srvPtzZoomin(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_ZOOMIN, para);
}

UPSint upui_fw_srvPtzZoomout(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_ZOOMOUT, para);
}

UPSint upui_fw_srvPtzFocusFar(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_FOCUSFAR, para);
}

UPSint upui_fw_srvPtzFocusNear(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_FOCUSNEAR, para);
}

UPSint upui_fw_srvPtzIrisOpen(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_IRISOPEN, para);
}

UPSint upui_fw_srvPtzIrisClose(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_IRISCLOSE, para);
}

UPSint upui_fw_srvPtzAutoPan(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_AUTOPAN, para);
}

UPSint upui_fw_srvPtzCruisePreSet(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_CRUISE_PRESET, para);
}

UPSint upui_fw_srvPtzCruiseRecall(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_CRUISE_RECALL, para);
}

UPSint upui_fw_srvPtzCruiseStart(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_CRUISE_START, para);
}

UPSint upui_fw_srvPtzCruiseClear(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_CRUISE_CLEARPRESET, para);
}

UPSint upui_fw_srvPtzCruiseLoad(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_CRUISE_LOAD, para);
}

UPSint upui_fw_srvPtzCruiseSave(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_CRUISE_SAVE, para);
}

UPSint upui_fw_srvPtzCruiseStop(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_CRUISE_STOP, para);
}

UPSint upui_fw_srvPtzStop(UPUI_VideoCtx *ctx, UPUint para)
{
	if(ctx == UP_NULL)
	{
		return -1;
	}
	
	return srv_com_ptzControl(ctx->srvInstance, PTZ_ACTION_STOP, para);
}

UPSint upui_fw_srvGetChnNumber(UPSint8 *addr, UPUint32 port)
{
	return srv_com_vmsGetChannelNum(addr, port);
}

UPSint upui_fw_srvLogin(UPSint8 *addr, UPUint32 port, UPUint8 *userName, UPUint8 *password)
{
	return srv_com_vmsLogin(addr, port, userName, password);
}

void upui_fw_srvFileMgrFini(Search_FileMgr *mgr);
UPUint upui_fw_srvGetCalenda(UPSint8 *addr, UPUint32 port, UPUint8 year, UPUint8 month, UPUint8 day, UPUint8 type)
{	
	return srv_com_vmsGetCalenda(addr, port, year, month, day);
}

static void m_mgr_dump(Search_FileMgr *mgr)
{
	UPUint i = 0;
	Search_FileInfo *fileInfo = UP_NULL;
	if(mgr == UP_NULL)
	{
		UPLOG(m_mgr_dump, ("invailable search file mgr"));
		return ;
	}
	
	while(i < mgr->fileNum)
	{
		fileInfo = mgr->list[i];
		UPLOG(m_mgr_dump, ("name: %s", fileInfo->file_name));
		UPLOG(m_mgr_dump, ("file size: %d", fileInfo->file_size));
		UPLOG(m_mgr_dump, ("record type: %d", fileInfo->rec_type));
		UPLOG(m_mgr_dump, ("channel: %d", fileInfo->channel));
		UPLOG(m_mgr_dump, ("start hour: %d", fileInfo->start_hour));
		UPLOG(m_mgr_dump, ("start miniute: %d", fileInfo->start_min));
		UPLOG(m_mgr_dump, ("start second: %d", fileInfo->start_sec));
		UPLOG(m_mgr_dump, ("end hour: %d", fileInfo->end_hour));
		UPLOG(m_mgr_dump, ("end miniute: %d", fileInfo->end_min));
		UPLOG(m_mgr_dump, ("end second: %d", fileInfo->end_sec));
		i++;
	}
	
}

static void m_mgr_dumpFileInfo(Search_FileInfo *fileInfo)
{
	if(fileInfo == UP_NULL)
	{
		UPLOG(m_mgr_dumpFileInfo, ("invailable file information"));
		return;
	}
	
	UPLOG(m_mgr_dump, ("name: %s", fileInfo->file_name));
	UPLOG(m_mgr_dump, ("file size: %d", fileInfo->file_size));
	UPLOG(m_mgr_dump, ("record type: %d", fileInfo->rec_type));
	UPLOG(m_mgr_dump, ("channel: %d", fileInfo->channel));
	UPLOG(m_mgr_dump, ("start hour: %d", fileInfo->start_hour));
	UPLOG(m_mgr_dump, ("start miniute: %d", fileInfo->start_min));
	UPLOG(m_mgr_dump, ("start second: %d", fileInfo->start_sec));
	UPLOG(m_mgr_dump, ("end hour: %d", fileInfo->end_hour));
	UPLOG(m_mgr_dump, ("end miniute: %d", fileInfo->end_min));
	UPLOG(m_mgr_dump, ("end second: %d", fileInfo->end_sec));

	return;
}

Search_FileMgr *upui_fw_srvFileMgrInit()
{
	Search_FileMgr *instance = (Search_FileMgr*)ucm_malloc(UP_NULL, sizeof(Search_FileMgr));
	if(instance == UP_NULL)
	{
		UPLOG(upui_fw_srvFileMgrInit, ("Search_FileMgr not enought memory to create"));
		return UP_NULL;
	}
	
	memset((void*)instance, 0x0, sizeof(Search_FileMgr));
	
	instance->downloadMgr = srv_com_downloadCreate();
	if(instance->downloadMgr == UP_NULL)
	{
		UPLOG(upui_fw_srvFileMgrInit, ("Search_FileMgr not enought memory to create"));
		ucm_free(UP_NULL, instance);
		instance = UP_NULL;
		return instance;
	}
	
	instance->year = 0;
	instance->month = 0;
	instance->day = 0;
	
	instance->dstFile = UP_NULL;
	instance->dstOffset = 0;
	
	return instance;
}

void upui_fw_srvFileMgrFini(Search_FileMgr *instance)
{
	UPUint i = 0;
	
	UPLOG(upui_fw_srvFileMgrFini, ("start"));
	if(instance == UP_NULL)
	{
		return ;
	}
	
	if(instance->downloadMgr)
	{
		srv_com_downloadDestroy(instance->downloadMgr);
		instance->downloadMgr = UP_NULL;
	}
	
	/*release resource*/
	if(instance->fileNum > 0)
	{
		if(instance->list)
		{
			while(i < instance->fileNum)
			{
				ucm_free(UP_NULL, instance->list[i]);
				instance->list[i] = UP_NULL;
				i++;
			}
		}
	}
	if(instance->list)
	{
		ucm_free(UP_NULL, instance->list);
		instance->list = UP_NULL;
	}
	
	ucm_free(UP_NULL, instance);
	
}

void upui_fw_srvFileMgrClear(Search_FileMgr *instance)
{
	UPUint i = 0;
	if(instance == UP_NULL)
	{
		UPLOG(upui_fw_srvFileMgrClear, ("Search File Clear not available instance"));
		return;
	}
	
	if(instance->fileNum > 0)
	{
		if(instance->list)
		{
			while(i < instance->fileNum)
			{
				ucm_free(UP_NULL, instance->list[i]);
				instance->list[i] = UP_NULL;
				i++;
			}
		}
	}
	
	if(instance->list)
	{
		ucm_free(UP_NULL, instance->list);
		instance->list = UP_NULL;
	}
}

UPSint upui_fw_srvSearchFile(Search_FileMgr *instance, UPSint8 *addr, UPUint32 port, UPUint channel, UPUint8 year, UPUint8 month, UPUint8 day, UPUint8 type)
{
	UPSint8 *listTmp = UP_NULL;
	UPUint fileNum = 0, i = 0;
	Search_FileInfo *fileInfo = UP_NULL;
	
	if(instance == UP_NULL)
	{
		UPLOG(upui_fw_srvSearchFile, ("Search File not available instance"));
		return -1;
	}

	instance->year = year;
	instance->month = month;
	instance->day = day;

	listTmp = srv_com_vmsGetFileList(addr, port, channel, year, month, day, type, &fileNum);
	UPLOG(upui_fw_srvSearchFile, ("file number: %d", fileNum));
	
	if(listTmp == UP_NULL)
	{
		UPLOG(upui_fw_srvSearchFile, ("no record file in this day"));
		upui_fw_srvFileMgrClear(instance);
		instance->fileNum = 0;
		return -1;
	}
	
	if(fileNum > 0)
	{
		upui_fw_srvFileMgrClear(instance);
		instance->list = ucm_malloc(UP_NULL, sizeof(Search_FileInfo*)*fileNum);
	}
	else
	{
		upui_fw_srvFileMgrClear(instance);
		instance->fileNum = 0;
	}
	
	while(i < fileNum)
	{
		fileInfo = (Search_FileInfo*)(listTmp + sizeof(Search_FileInfo)*i);
		instance->list[i] = (Search_FileInfo*)ucm_malloc(UP_NULL, sizeof(Search_FileInfo));
		memcpy(instance->list[i], fileInfo, sizeof(Search_FileInfo));
		instance->list[i]->reserve[0] = 1;
		instance->list[i]->reserve[1] = 1;
		i++;
	}
	
	ucm_free(UP_NULL, listTmp);
	instance->fileNum = fileNum;
	m_mgr_dump(instance);

	return 0;
}

UPSint upui_fw_srvSearchFileGetNumber(Search_FileMgr *instance, UPSint8 *addr, UPUint32 port, 
									  UPUint channel, UPUint8 year, UPUint8 month, UPUint8 day, UPUint8 type)
{
	UPSint ret = 0;
	UPUint i = 0;
//	Search_FileInfo *fileInfo = UP_NULL;
	
	ret = srv_com_vmsGetFileNum(addr, port, channel, year, month, day, type);
	UPLOG(upui_fw_srvSearchFileGetNumber, ("ret: %d", ret));
	if(ret > 0)
	{
		upui_fw_srvFileMgrClear(instance);
		instance->list = ucm_malloc(UP_NULL, sizeof(Search_FileInfo*)*ret);
	}
	else
	{
		upui_fw_srvFileMgrClear(instance);
		instance->fileNum = 0;
		return -1;
	}
	
	while(i < ret)
	{
		instance->list[i] = (Search_FileInfo*)ucm_malloc(UP_NULL, sizeof(Search_FileInfo));
		memset(instance->list[i], 0x0, sizeof(Search_FileInfo));
		instance->list[i]->reserve[0] = 0;
		instance->list[i]->reserve[1] = 0;
		i++;
	}

	instance->fileNum = ret;
	
	return ret;
}

UPSint upui_fw_srvSearchFileGetPage(Search_FileMgr *instance, UPSint8 *addr, UPUint32 port, 
									UPUint channel, UPUint8 year, UPUint8 month, UPUint8 day, UPUint8 type, UPUint page)
{
	UPSint ret = 0;
	UPUint i = 0, j = 0;
	UPSint8 *listTmp = UP_NULL;
	UPUint fileNum;
	Search_FileInfo *fileInfo = UP_NULL;
	
	listTmp = srv_com_vmsGetFilePage(addr, port, channel, year, month, day, type, page, &fileNum);
	
	if(listTmp == UP_NULL)
	{
		UPLOG(upui_fw_srvSearchFile, ("no record file at this page: %d", page));
		//upui_fw_srvFileMgrClear(instance);
		//instance->fileNum = 0;
		return -1;
	}
	
	while(i < instance->fileNum)
	{
		fileInfo = (Search_FileInfo*)instance->list[i];
		if(fileInfo->reserve[0] == 1 && fileInfo->reserve[1] == 1)
		{
			i++;
		}
		else
		{
			break;
		}

	}
	
	while(i < instance->fileNum && j < fileNum)
	{
		//UPLOG(upui_fw_srvSearchFileGetPage, ("fileNum: %d, i: %d", instance->fileNum, i));
		fileInfo = (Search_FileInfo*)(listTmp + sizeof(Search_FileInfo)*j);
		fileInfo->reserve[0] = 1;
		fileInfo->reserve[1] = 1;
		memcpy(instance->list[i], fileInfo, sizeof(Search_FileInfo));

		i++;
		j++;
	}
	ucm_free(UP_NULL, listTmp);
	
	return ret;
}

UPSint upui_fw_srvFileMgrDownloadStop(Search_FileMgr *instance)
{
	if(instance == UP_NULL)
	{
		UPLOG(upui_fw_srvFileMgrDownloadStop, ("Search File not available instance"));
		return -1;
	}
	
	srv_com_downloadStop(instance->downloadMgr);
	
	return 0;
}

UPSint upui_fw_srvFileMgrSetDst(Search_FileMgr *instance, UPSint8 *name, UPUint offset)
{
	UPUint i = 0;
	UPSint flag = -1;
	Search_FileInfo *fileInfo = UP_NULL;
	
	if(instance == UP_NULL)
	{
		UPLOG(upui_fw_srvFileMgrSetDst, ("Search File not available instance"));
		return -1;
	}
	
	if(instance->downloadMgr->filePath != UP_NULL 
	   && instance->downloadMgr->downloadStatus == SRV_DOWNLOAD_ONGOING
	   && instance->downloadMgr->thStatus == SRV_Thread_status_running)
	{
		if(memcmp(instance->downloadMgr->filePath, name, strlen(name)) == 0)
		{
			UPLOG(upui_fw_srvFileMgrSetDst, ("Search File download ongoing"));
			return 0;
		}
	}
	
	srv_com_downloadStop(instance->downloadMgr);
	if(instance->downloadMgr->downloadStatus == SRV_DOWNLOAD_ONGOING 
	   && instance->downloadMgr->thStatus == SRV_Thread_status_running)
	{
		UPLOG(upui_fw_srvFileMgrSetDst, ("Search File download ongoing"));
		return 0;
	}
	
	//srv_com_downloadStop(instance->downloadMgr);
	
	while(i < instance->fileNum)
	{
		fileInfo = instance->list[i];
		if(memcmp(fileInfo->file_name, name, strlen(name)) == 0)
		{
			UPLOG(upui_fw_srvFileMgrSetDst, ("Search File find file"));
			flag = i;
			break;
		}
		i++;
	}
	
	if(flag != -1)
	{
		instance->dstFile = instance->list[flag];
		m_mgr_dumpFileInfo(instance->dstFile);
		instance->dstOffset = offset;
		srv_com_downloadSetFileInfo(instance->downloadMgr, (unsigned char*)(instance->dstFile->file_name), instance->dstFile->file_size, instance->dstOffset);
		srv_com_downloadStart(instance->downloadMgr);
	}
	
	return 0;
}

UPSint upui_fw_srvFileMgrGetDstSize(Search_FileMgr *instance)
{
	if(instance == UP_NULL)
	{
		UPLOG(upui_fw_srvFileMgrGetDstSize, ("Search File not available instance"));
		return 0;
	}
	
	return instance->dstFile->file_size;
}

UPSint upui_fw_srvFileMgrSetServer(Search_FileMgr *instance, UPSint8 *addr, UPSint32 port)
{
	if(instance == UP_NULL)
	{
		UPLOG(upui_fw_srvFileMgrSetServer, ("Search File not available instance"));
		return -1;
	}
	
	return srv_com_downloadSetup(instance->downloadMgr, addr, port);
}

UPSint upui_fw_srvFileMgrSetSavePath(Search_FileMgr *instance, UPSint8 *savePath)
{
	if(instance == UP_NULL)
	{
		UPLOG(upui_fw_srvFileMgrSetSavePath, ("Search File not available instance"));
		return -1;
	}
	
	return srv_com_downloadSetSavePath(instance->downloadMgr, savePath);
}

UPSint upui_fw_srvFileMgrSetSaveName(Search_FileMgr *instance, UPSint8 *saveName)
{
	if(instance == UP_NULL)
	{
		UPLOG(upui_fw_srvFileMgrSetSaveName, ("Search File not available instance"));
		return -1;
	}
	
	return srv_com_downloadSetSaveName(instance->downloadMgr, saveName);
}

UPSint upui_fw_srvFileMgrGetPrecent(Search_FileMgr *instance)
{
	if(instance == UP_NULL)
	{
		UPLOG(upui_fw_srvFilemgrGetPrecent, ("Search File not available instance"));
		return -1;
	}
	
	return srv_com_downloadGetPrecent(instance->downloadMgr);
}

void* upui_fw_srvAudioListPop(UPUI_VideoCtx *ctx)
{
	if(ctx == UP_NULL)
	{
		UPLOG(upui_fw_srvAudioListPop, ("not available instance"));
		return UP_NULL;
	}

	return srv_com_audioListPop(ctx->srvInstance);
}

void* upui_fw_srvAudioPoolPush(UPUI_VideoCtx *ctx, void *data)
{
	if(ctx == UP_NULL)
	{
		UPLOG(upui_fw_srvAudioPoolPush, ("not available instance"));
		return UP_NULL;
	}
	
	return srv_com_audioPoolPush(ctx->srvInstance, data);
}

