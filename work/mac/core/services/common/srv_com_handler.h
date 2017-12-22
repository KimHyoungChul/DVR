#ifndef _SRV_COM_HANDLER_HEADER
#define _SRV_COM_HANDLER_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "up_sys_config.h"
#include "up_types.h"
#include "ucm.h"
#include "up_sys_log.h"
#include "up_sys_sock.h"
//#include "up_sys_mutex.h"
//#include "up_sys_event.h"
#include "up_sys_util.h"
#include "util_circle_list.h"

typedef enum
{
	SRV_ID_SZRAWDATA,
	SRV_ID_SZPBONLINE,
	SRV_ID_SZOWSP,
	SRV_ID_SZVMS
}SRV_Identify;

typedef enum
{
	SRV_Thread_status_idle,
	SRV_Thread_status_running,
	SRV_Thread_status_exitReq,
	SRV_Thread_status_exited,
	SRV_Thread_status_notCreate
}SRV_Thread_Status;

typedef struct _NET_Instance
{
	UPSint type;
	UPSocketHandle pSock;

	/*for some network need login*/
	UPSint8 *username;
	UPSint8 *password;
	UPSint8 *addr;
	UPSint32 port;
	
	UPSint32 loginResult;
	UPUint16 sequence;

	/*for DVR*/
	UPUint videoChannel;
	UPUint networkType; /*0: internet; 1: lan*/
	//UPUint videoWidth;
	//UPUint videoHeight;

	/*for protect*/
	UpMutexHandle mutex;
	UpEventHandle event;

	/*for send data*/
	UPSint8* sendQBuf;
	UPSint sendQBufLen;
	UPSint sendQBufSize;

	/*for recv data*/
	UPSint8* recvQBuf;
	UPSint recvQBufLen;
	UPSint recvQBufSize;
	UPUint recvLastTick;
	UPUint dueTick;

	struct _NET_Instance *next;
}NET_Instance;

typedef struct _FILE_Instance
{
	UPSint type;
	UPSint8* path;
	FILE *fp;

	UPSint8* frameBuf;
	UPUint frameBufLen;
	UPUint frameBufSize;

	/*for backup yuv data*/
	UPSint8* yuvPath;
	FILE *yuvFp;
	UPUint size;
	UPUint readPos;
	UPUint basePos;
	UPUint foundIFrame;
}FILE_Instance;

typedef struct _Data_Pool
{
	UPSint count;
	putilCircleListStr list;
	putilCircleListStr pool;
}Data_Pool;
	
typedef enum
{
	SRV_NETWORK_NORMAL,
	SRV_NETWORK_TIMEOUT,
	SRV_NETWORK_DROPLINE,
	SRV_NETWORK_NOTCREATE,
	
}SRV_Network_Status;

typedef struct _SRV_Instance
{
	/*srv identify*/
	SRV_Identify type;
	UPUint8* name;

	/*video stream thread handler*/
	UpThreadHandle videoStream;
	UpMutexHandle videoStreamMutex;
	UpEventHandle videoStreamEvent;
	SRV_Thread_Status videoStreamStatus;

	/*video stream from network*/
	NET_Instance *netHandle;
	SRV_Network_Status netStatus;

	/*video stream from raw data*/
	FILE_Instance *fileHandle;

	/*circle list for data handler*/
	Data_Pool *videoPool;
	Data_Pool *audioPool;
	
	/*for record*/
	FILE *recFp;
	UPUint8 recFlag;
	
	/*for playback online*/
	UPUint dstFileSize;
	
	void* partner;

	struct _SRV_Instance *next;
}SRV_Instance;

typedef struct _RC_data_entry_str_
{
	unsigned int mediaType;

	//the original pts from server
	unsigned long long pts;
	//the repected tick when receiving frame
	unsigned int recvTick;

	unsigned int tmPos;

	unsigned int size;    

	char  frameType;  

	char* data;
	unsigned int   dataLen;

	struct _RC_data_entry_str_* next;

}RC_data_entry_str, *PRC_data_entry_str;
	
typedef enum
{
	PTZ_ACTION_NONE,
	PTZ_ACTION_UP,
	PTZ_ACTION_DOWN,
	PTZ_ACTION_LEFT,
	PTZ_ACTION_RIGHT,
	PTZ_ACTION_ZOOMIN,
	PTZ_ACTION_ZOOMOUT,
	PTZ_ACTION_FOCUSFAR,
	PTZ_ACTION_FOCUSNEAR,
	PTZ_ACTION_IRISOPEN,
	PTZ_ACTION_IRISCLOSE,
	PTZ_ACTION_AUTOPAN,
	PTZ_ACTION_CRUISE_PRESET,
	PTZ_ACTION_CRUISE_RECALL,
	PTZ_ACTION_CRUISE_START,
	PTZ_ACTION_CRUISE_CLEARPRESET,
	PTZ_ACTION_CRUISE_LOAD,
	PTZ_ACTION_CRUISE_SAVE,
	PTZ_ACTION_CRUISE_STOP,
	PTZ_ACTION_STOP,

}PTZControlAction;
	
typedef enum _SRV_Download_status
{
	SRV_DOWNLOAD_NOTSTART,
	SRV_DOWNLOAD_ONGOING,
	SRV_DOWNLOAD_STOP,
	SRV_DOWNLOAD_FINISH,
	SRV_DOWNLOAD_ERROR,
	
}SRV_Download_status;
	
typedef struct _SRV_Download
{
	/*for network handler*/
	UPSocketHandle pSock;
	UPSint8 *addr;
	UPSint32 port;
	UPSint8* recvQBuf;
	UPSint recvQBufLen;
	UPSint recvQBufSize;	
	
	/*for file handle*/
	UPUint fileSize;
	UPUint recvSize;
	UPUint startOffset;
	UPSint8 *filePath;
	UPSint8 savePath[128];
	UPSint8 saveName[64];
	FILE *saveFp;
	SRV_Download_status downloadStatus;
	
	/*for resource protect*/
	UpThreadHandle thHandler;
	UpMutexHandle thMutex;
	UpEventHandle thEvent;
	SRV_Thread_Status thStatus;
	
	struct _SRV_Download *next;
}SRV_Download;
	
SRV_Download *srv_com_downloadCreate();
void srv_com_downloadDestroy(SRV_Download *instance);
UPSint srv_com_downloadSetup(SRV_Download *instance, UPSint8 *addr, UPSint32 port);
UPSint srv_com_downloadSetFileInfo(SRV_Download *instance, UPSint8 *path, UPUint fileSize, UPUint offset);
UPSint srv_com_downloadSetSavePath(SRV_Download *instance, UPSint8 *savePath);
UPSint srv_com_downloadSetSaveName(SRV_Download *instance, UPSint8 *saveName);
UPSint srv_com_downloadGetPrecent(SRV_Download *instance);
UPSint srv_com_downloadStart(SRV_Download *instance);
UPSint srv_com_downloadStop(SRV_Download *instance);

/*impl function*/
Data_Pool *pool_create(UPSint count);
void pool_release(Data_Pool *pool);
SRV_Instance *srv_com_create(SRV_Identify type, const UPSint8* name);
void srv_com_destroy(SRV_Instance *instance);
void srv_com_setChannel(SRV_Instance *instance, UPUint channel);
UPSint srv_com_setUsrInfo(SRV_Instance *instance, UPSint8* name, UPSint8* password);
UPSint srv_com_setNetInfo(SRV_Instance *instance, UPSint8* addr, UPSint32 port, UPUint type);	
UPSint srv_com_createFileInstance(SRV_Instance *instance, const UPSint8* path);
void srv_com_destroyFileInstance(SRV_Instance *instance);
UPSint srv_com_fileInstanceSetPath(SRV_Instance *instance, const UPSint8* path);
UPSint srv_com_fileInstanceGetPos(SRV_Instance *instance);
UPSint srv_com_fileInstanceSetPos(SRV_Instance *instance, UPSint pos);
UPSint srv_com_fileInstanceSetSize(SRV_Instance *instance, UPUint size);
	
UPSint srv_com_audioPoolReset(SRV_Instance *instance);
PRC_data_entry_str srv_com_audioPoolPop(SRV_Instance *instance);
PRC_data_entry_str srv_com_audioPoolPush(SRV_Instance *instance, PRC_data_entry_str data);
PRC_data_entry_str srv_com_audioListPop(SRV_Instance *instance);
PRC_data_entry_str srv_com_audioListPush(SRV_Instance *instance, PRC_data_entry_str data);
	
UPSint srv_com_poolReset(SRV_Instance *instance);
PRC_data_entry_str srv_com_poolPop(SRV_Instance *instance);
PRC_data_entry_str srv_com_poolPush(SRV_Instance *instance, PRC_data_entry_str data);
PRC_data_entry_str srv_com_listPop(SRV_Instance *instance);
PRC_data_entry_str srv_com_listPush(SRV_Instance *instance, PRC_data_entry_str data);
UPSint srv_com_start(SRV_Instance *instance);
UPSint srv_com_stop(SRV_Instance *instance);
UPSint srv_com_restart(SRV_Instance *instance);
UPSint srv_com_pause(SRV_Instance *instance);
UPSint srv_com_continue(SRV_Instance *instance);
UPSint srv_com_notify(SRV_Instance *instance);
UPSocketHandle srv_com_connect(UPSint8* addr, UPSint32 port);
UPSint srv_com_Record(SRV_Instance *instance);
UPSint srv_com_StopRecord(SRV_Instance *instance);
UPSint srv_com_ptzControl(SRV_Instance *instance, PTZControlAction action, UPUint para);
SRV_Network_Status srv_com_getNetworkStatus(SRV_Instance *instance);
UPUint srv_com_vmsGetCalenda(UPSint8 *addr, UPUint32 port, UPUint8 year, UPUint8 month, UPUint8 day);
UPSint8* srv_com_vmsGetFileList(UPSint8 *addr, UPSint port, UPUint channel, UPUint8 year, UPUint8 month, UPUint8 day, UPSint type, UPUint *fileNum);
UPSint srv_com_vmsGetFileNum(UPSint8 *addr, UPSint port, UPUint channel, UPUint8 year, 
								UPUint8 month, UPUint8 day, UPSint type);
UPSint8* srv_com_vmsGetFilePage(UPSint8 *addr, UPSint port, UPUint channel, UPUint8 year, 
								UPUint8 month, UPUint8 day, UPSint type, UPUint page, UPUint *fileNum);
UPSint srv_com_vmsGetChannelNum(UPSint8 *addr, UPSint port);
UPSint srv_com_vmsLogin(UPSint8 *addr, UPSint port, UPUint8 *userName, UPUint8 *password);
	
UPSint up_fs_fileExist(const char *fileName);

/*event*/
UPSint up_event_wait(UpEventHandle cond, UpMutexHandle mux, UPSint duration);
	
/*util*/
	
	
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_SRV_COM_HANDLER_HEADER*/
