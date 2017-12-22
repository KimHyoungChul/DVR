#ifndef _UP_UI_CORE_HEADER
#define _UP_UI_CORE_HEADER

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
	
#define NETWORK_DROPLINE SRV_NETWORK_DROPLINE
#define NETWORK_TIMEOUT  SRV_NETWORK_TIMEOUT
	
typedef struct _Search_FileInfo
{
	int file_size;		/*Bytes*/
	int rec_type;		/*1: normal rec; 2: alarm rec; 3: all rec*/
	int channel;		/*bit0~bit15: 1~16channel*/

	char file_name[128]; /*absolutely path*/
	//rec start time
	unsigned char start_hour;
	unsigned char start_min;
	unsigned char start_sec;
	//rec stop time
	unsigned char end_hour;
	unsigned char end_min;
	unsigned char end_sec;
	unsigned char reserve[2];

}Search_FileInfo;
	
typedef struct _Search_FileMgr
{
	int fileNum;	/*rec file num*/
	
	/*date*/
	unsigned char year;
	unsigned char month;
	unsigned char day;
	
	
	Search_FileInfo *dstFile;
	unsigned int dstOffset;
	
	/*file list*/
	Search_FileInfo **list;
	
	SRV_Download *downloadMgr;
}Search_FileMgr;
	
UPSint upui_fw_init();
UPSint upui_fw_fini();
UPUI_VideoCtx* upui_fw_initCtx(UPUint channel, UPSint type);
void upui_fw_finiCtx(UPUI_VideoCtx *ctx);
void upui_fw_setCtxChannel(UPUI_VideoCtx* instance, UPUint channel);
UPUint upui_fw_getCtxChannel(UPUI_VideoCtx* instance);	
UPSint upui_fw_setCtxUsrInfo(UPUI_VideoCtx* instance, UPSint8* name, UPSint8* password);
UPSint upui_fw_setCtxNetInfo(UPUI_VideoCtx* instance, UPSint8* addr, UPSint32 port, UPUint type);
UPSint upui_fw_setCtxFilePath(UPUI_VideoCtx* instance, const UPSint8* path);
UPSint upui_fw_resetFramePool(UPUI_VideoCtx *ctx);
UPSint upui_fw_getNetworkStatus(UPUI_VideoCtx *ctx);
	
UPSint upui_fw_startSrv(UPUI_VideoCtx *ctx);
UPSint upui_fw_stopSrv(UPUI_VideoCtx *ctx);
UPSint upui_fw_startCtx(UPUI_VideoCtx *ctx);
UPSint upui_fw_stopCtx(UPUI_VideoCtx *ctx);
UPSint upui_fw_setHandleCBCtx(UPUI_VideoCtx *ctx, process_cb handlecb, void *param);
	
UPSint upui_fw_startSrvRecord(UPUI_VideoCtx *ctx);
UPSint upui_fw_stopSrvRecord(UPUI_VideoCtx *ctx);
UPSint upui_fw_getSrvFilePos(UPUI_VideoCtx *ctx);
UPSint upui_fw_setSrvFilePos(UPUI_VideoCtx *ctx, UPSint pos);
UPSint upui_fw_setSrvFileSize(UPUI_VideoCtx *ctx, UPUint size);
	
/*PTZ Control Function*/
UPSint upui_fw_srvPtzUp(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzDown(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzLeft(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzRight(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzZoomin(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzZoomout(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzFocusFar(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzFocusNear(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzIrisOpen(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzIrisClose(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzAutoPan(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzCruisePreSet(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzCruiseRecall(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzCruiseStart(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzCruiseClear(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzCruiseLoad(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzCruiseSave(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzCruiseStop(UPUI_VideoCtx *ctx, UPUint para);
UPSint upui_fw_srvPtzStop(UPUI_VideoCtx *ctx, UPUint para);
	
/*audio output Function*/
void *upui_fw_srvAudioListPop(UPUI_VideoCtx *ctx);
void *upui_fw_srvAudioPoolPush(UPUI_VideoCtx *ctx, void *data);
	
UPUint upui_fw_srvGetCalenda(UPSint8 *addr, UPUint32 port, UPUint8 year, UPUint8 month, UPUint8 day, UPUint8 type);	
UPSint upui_fw_srvSearchFile(Search_FileMgr *instance, UPSint8 *addr, UPUint32 port, UPUint channel, UPUint8 year, UPUint8 month, UPUint8 day, UPUint8 type);
UPSint upui_fw_srvSearchFileGetNumber(Search_FileMgr *instance, UPSint8 *addr, UPUint32 port, 
							 UPUint channel, UPUint8 year, UPUint8 month, UPUint8 day, UPUint8 type);
UPSint upui_fw_srvSearchFileGetPage(Search_FileMgr *instance, UPSint8 *addr, UPUint32 port, 
							 UPUint channel, UPUint8 year, UPUint8 month, UPUint8 day, UPUint8 type, UPUint page);
UPSint upui_fw_srvGetChnNumber(UPSint8 *addr, UPUint32 port);
UPSint upui_fw_srvLogin(UPSint8 *addr, UPUint32 port, UPUint8 *userName, UPUint8 *password);
	
void upui_fw_srvFileMgrFini(Search_FileMgr *instance);
Search_FileMgr *upui_fw_srvFileMgrInit();
UPSint upui_fw_srvFileMgrDownloadStop(Search_FileMgr *instance);
UPSint upui_fw_srvFileMgrSetDst(Search_FileMgr *instance, UPSint8 *name, UPUint offset);
UPSint upui_fw_srvFileMgrGetDstSize(Search_FileMgr *instance);
UPSint upui_fw_srvFileMgrSetServer(Search_FileMgr *instance, UPSint8 *addr, UPSint32 port);
UPSint upui_fw_srvFileMgrSetSavePath(Search_FileMgr *instance, UPSint8 *savePath);
UPSint upui_fw_srvFileMgrSetSaveName(Search_FileMgr *instance, UPSint8 *saveName);
UPSint upui_fw_srvFileMgrGetPrecent(Search_FileMgr *instance);
	
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_UP_UI_CORE_HEADER*/
