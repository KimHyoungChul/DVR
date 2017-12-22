#ifndef _KT_STREAM_HANDLER_HEADER
#define _KT_STREAM_HANDLER_HEADER

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#include "../kt_core_config.h"

/*core module*/
#include "./core/include/srv_com_handler.h"
#include "./core/include/srv_rc_pool.h"
#include "./core/include/up_ui_context.h"
#include "./core/include/up_ui_core.h"
#include "../kt_file_manage.h"

/*ffmpeg module*/
#include "../../ffmepg_sdk/include/libavutil/avutil.h"
#include "../../ffmepg_sdk/include/libavcodec/avcodec.h"
#include "../../ffmepg_sdk/include/libswscale/swscale.h"
    
//#include "../../up_types.h"
	
#define Search_DVR_BUSY		0xF0000001
#define Search_DVR_NOFILE	0xF0000002
#define Search_DVR_ERROR	0xF0000003
	
/*playback stream control*/
typedef enum _StreamControl
{
	STREAM_NORMAL,
	STREAM_STOP,
	STREAM_PAUSE,
	STREAM_FF_2X,
	STREAM_FF_4X,
	STREAM_FF_8X,
	STREAM_SF_2X,
	STREAM_SF_4X,
	STREAM_SF_8X,
	STREAM_STEP
	
}StreamControl;
	
typedef enum _StreamType
{
	STREAM_LIVE,
	STREAM_ONLINE,
	STREAM_LOCAL
	
}StreamType;
    
    
/*playback module for handle video stream*/
typedef struct _Stream_Playback
{
	UPSint type;
	
	enum CodecID codecID;
	AVCodecContext *codecCtx;
	AVCodec *codec;
	AVFrame *frame;
	AVFrame *rgbFrame;
	
	UPUint8* yuvData[4];
	UPSint linesize[4];
	
	UPUint dstWidth;
	UPUint dstHeight;

	/*maybe use for sync*/
	unsigned char isFirst;
	UPUint64 streamTimeBase;
	UPUint localTimeBase;
	
	/*for stream control*/
	StreamControl streamCtl;
	UPUint streamCtlStatus; /*0: normal, 1: blocked*/
	UpMutexHandle streamCtlMutex;
	UpEventHandle streamCtlEvent;

	UPUI_VideoCtx *videoCtx;
    
    UPSint8 ip[100];
    UPUint port;
    UPUint channel;//当前的通道号
    UPUint channelNum;//通道个数
    process_cb cbRealVideoFunc ;
    void* hDevStreamHandle;   
    
    void* hDvs;// 设备句柄
    void* hPlayUtility;//录像文件的播放句柄
    UPSint8 localOrRemoteVideoFilePath[100];

	/*pool for yuv stream*/
	Data_Pool *yuvPool;
	
	/*download module*/
	void *externMod;
	UPSint downloadPrecent;
	UPUint downloadFileSize;
	
	/*for audio handle*/
	//void *audioOutput;

	struct _Stream_Playback *next;
}Stream_Playback;
	
typedef struct _RGB_Render
{
	char* outData;
	int outLineSize;
	unsigned int outDataSize;
}RGB_Render;
	
typedef struct _RGB_Entry
{
	int width;
	int height;
	int flag;
	RGB_Render *data;
	
}RGB_Entry;
	
typedef struct _Playback_Online
{
	UPSint8 *addr;
	UPSint32 port;
	
	UPUint calendaMap;
	
	RecFile *dstFile;
	UPUint dstFileSize;
	UPUint dstRecvFileSize;
	
	RecFile *cacheFileList;
	RecFile *remoteFileList;
	
	Search_FileMgr *searchMgr;
	//SRV_Download *downloadMgr;
}Playback_Online;

Stream_Playback* stream_playback_init(StreamType type);
void stream_playback_fini(Stream_Playback *instance);
UPSint stream_playback_start(Stream_Playback *instance);
UPSint stream_playback_stop(Stream_Playback *instance);
UPSint stream_playback_normal(Stream_Playback *instance);
UPSint stream_playback_pause(Stream_Playback *instance);
UPSint stream_playback_fastForward(Stream_Playback *instance, UPSint speed);
UPSint stream_playback_slowForward(Stream_Playback *instance, UPSint speed);
UPSint stream_playback_step(Stream_Playback *instance);

void stream_playback_initCodec();
UPSint stream_playback_initDecode(Stream_Playback *instance, enum CodecID codecId);
UPSint stream_playback_finiDecode(Stream_Playback *instance);
UPSint stream_playback_resetDecode(Stream_Playback *instance);
UPSint stream_playback_setup(Stream_Playback *instance, UPUint channel);
UPSint stream_playback_setUsrInfo(Stream_Playback *instance, UPSint8* name, UPSint8* password);
UPSint stream_playback_setNetInfo(Stream_Playback *instance, UPSint8* addr, UPSint32 port, UPUint type);
UPSint stream_playback_setChannelNum(Stream_Playback *instance,UPUint chNum);
UPSint stream_playback_setVideoChannel(Stream_Playback *instance, UPUint channel);	
UPUint stream_playback_getVideoChannel(Stream_Playback *instance);	
UPSint stream_playback_queueReset(Stream_Playback *instance);
UPSint stream_playback_setDstSize(Stream_Playback *instance, UPUint width, UPUint height);
UPSint stream_playback_setFilePath(Stream_Playback *instance, const UPSint8* path);
UPSint stream_playback_getFilePos(Stream_Playback *instance);
UPSint stream_playback_setFilePos(Stream_Playback *instance, UPSint pos);
UPSint stream_playback_getNetworkStatus(Stream_Playback *instance); /*1: need reconnect; 0: normal*/
UPSint stream_playback_getDownloadPrecent(Stream_Playback *instance);
UPSint stream_playback_setDownloadPrecent(Stream_Playback *instance, UPSint precent);
UPSint stream_playback_setDownloadFileSize(Stream_Playback *instance, UPUint size);
	
RGB_Entry *stream_playback_poolPop2(Stream_Playback *instance);
RGB_Entry *stream_playback_poolPush2(Stream_Playback *instance, RGB_Entry *data);
RGB_Entry *stream_playback_listPop2(Stream_Playback *instance);
RGB_Entry *stream_playback_listPush2(Stream_Playback *instance, RGB_Entry *data);
	
UPSint stream_playback_startRecord(Stream_Playback *instance);
UPSint stream_playback_stopRecord(Stream_Playback *instance);	
	
UPSint stream_playback_ptzUp(Stream_Playback *instance);
UPSint stream_playback_ptzDown(Stream_Playback *instance);
UPSint stream_playback_ptzLeft(Stream_Playback *instance);
UPSint stream_playback_ptzRight(Stream_Playback *instance);
UPSint stream_playback_ptzZoomin(Stream_Playback *instance);
UPSint stream_playback_ptzZoomout(Stream_Playback *instance);
UPSint stream_playback_ptzFocusFar(Stream_Playback *instance);
UPSint stream_playback_ptzFocusNear(Stream_Playback *instance);
UPSint stream_playback_ptzIRISOpen(Stream_Playback *instance);
UPSint stream_playback_ptzIRISClose(Stream_Playback *instance);
UPSint stream_playback_ptzCruisePreSet(Stream_Playback *instance, UPUint para);
UPSint stream_playback_ptzCruiseRecall(Stream_Playback *instance, UPUint para);
UPSint stream_playback_ptzCruiseStart(Stream_Playback *instance, UPUint para);
UPSint stream_playback_ptzCruiseClear(Stream_Playback *instance, UPUint para);
UPSint stream_playback_ptzCruiseLoad(Stream_Playback *instance, UPUint para);
UPSint stream_playback_ptzCruiseSave(Stream_Playback *instance, UPUint para);
UPSint stream_playback_ptzCruiseStop(Stream_Playback *instance, UPUint para);
UPSint stream_playback_ptzStop(Stream_Playback *instance);
	
/*Audio queue handler*/
void *stream_playback_AudioListPop(Stream_Playback *instance);
void *stream_playback_AudioPoolPush(Stream_Playback *instance, void *data);
	
/*online playback*/	
Playback_Online* playback_online_init(UPSint8 *addr, UPSint32 port);
void playback_online_fini(Playback_Online *instance);
void playback_online_getCalenda(Playback_Online *instance, UPUint8 year, UPUint8 month, UPUint8 day, UPUint8 type);
void playback_online_getDescription(Playback_Online *instance, UPSint8 *description);
UPSint playback_online_getSearchMgr(Playback_Online *instance, UPUint8 year, UPUint8 month, UPUint8 day);
#ifdef KT_GET_FILELIST_ASYNC
UPSint playback_online_getOnlineFileNumber(Playback_Online *instance, UPUint8 year, UPUint8 month, UPUint8 day);
UPSint playback_online_getOnlineFilePage(Playback_Online *instance, UPUint8 year, UPUint8 month, UPUint8 day, UPUint page);
#endif /*KT_GET_FILELIST_ASYNC*/
UPSint playback_online_startDownload(Playback_Online *instance, UPSint8 *name, UPUint offset);
UPSint playback_online_stopDownload(Playback_Online *instance);
UPSint playback_online_getDownloadSize(Playback_Online *instance);
UPSint playback_online_setSavePath(Playback_Online *instance, UPSint8 *savePath);
UPSint playback_online_setSaveName(Playback_Online *instance, UPSint8 *saveName);
UPSint playback_online_getPrecent(Playback_Online *instance);
UPSint playback_online_isFileInCache(Playback_Online *instance, UPSint8 *filePath, UPSint type);
UPSint playback_online_getCacheFileList(Playback_Online *instance, UPSint8 *cachePath);
UPUint playback_online_getFileSizeInCache(Playback_Online *instance, UPSint8 *name);
    
    
//add by wuzy
void  stream_playback_setCallBackFunc(Stream_Playback *instance);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_KT_STREAM_HANDLER_HEADER*/
