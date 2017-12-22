#include "kt_stream_playback.h"
#include "kt_core_config.h"
#include "kt_util.h"
#include <unistd.h>
#include <pthread.h>

static UPBool m_codec_initFlag = 0;

void rgb_queue_reset(Data_Pool *pool)
{
	RGB_Entry *pEntry = NULL;
	while(UP_NULL != (pEntry = (RGB_Entry*)util_cl_list_pop(pool->list)))
	{
		util_cl_list_push(pool->pool, (void*)pEntry);
	}
}

UPSint rgb_queue_fini(Data_Pool *pool);
UPSint rgb_queue_init(Data_Pool *pool, UPUint size)
{
	int ret = 0;
	RGB_Entry *pEntry = NULL;
	int count = pool->count;
	
	pool->list = util_cl_create(count);
	pool->pool = util_cl_create(count);
	
	while(--count >= 0)
	{
		pEntry = (RGB_Entry*)memset((void*)ucm_malloc(UP_NULL, sizeof(RGB_Entry)), 0, sizeof(RGB_Entry));
		pEntry->width = 0;
		pEntry->height = 0;
		
		pEntry->data = (RGB_Render*)ucm_malloc(UP_NULL, sizeof(RGB_Render));
		if(pEntry->data == NULL)
		{
			UPLOG(rgb_queue_init, ("not enought memory"));
			rgb_queue_fini(pool);
			ret = -1;
			break;
		}
		memset((void*)pEntry->data, 0x0, sizeof(RGB_Render));
		
		pEntry->flag = 0;
		util_cl_list_push(pool->pool, (void*)pEntry);
	}
	
	return ret;
}

UPSint rgb_queue_fini(Data_Pool *pool)
{
	RGB_Entry *pEntry = NULL;
	RGB_Render *pRender = NULL;
	int count = pool->count;
	
	if(pool->pool == NULL || pool->list == NULL)
	{
		UPLOG(yuv_queue_fini, ("the queue has already release"));
		return 0;
	}
	UPLOG(rgb_queue_fini, ("the pool entry count: %d", pool->count));
	
	rgb_queue_reset(pool);
	while(--count >= 0)
	{
		pEntry = (RGB_Entry*)pool->pool->list[count];
		if(pEntry)
		{
			if(pEntry->data)
			{
				pRender = (RGB_Render*)pEntry->data;
				if(pRender->outData)
				{
					av_free(pRender->outData);
					pRender->outData = NULL;
				}
				ucm_free(UP_NULL, pEntry->data);
			}
			ucm_free(UP_NULL, pEntry);
		}
	}
	util_cl_destroy(pool->pool);
	pool->pool = NULL;
	util_cl_destroy(pool->list);
	pool->list = NULL;
	
	return 0;
}

RGB_Entry *rgb_queue_listPop(Data_Pool *pool)
{
	if(!pool)
	{
		return NULL;
	}
	
	return pool->list ? (RGB_Entry*)util_cl_list_pop(pool->list) : NULL;
}

RGB_Entry *rgb_queue_listPush(Data_Pool *pool, RGB_Entry *data)
{
	if(!pool)
	{
		return NULL;
	}
	
	return (RGB_Entry*)util_cl_list_push(pool->list, (void*)data);
}

RGB_Entry *rgb_queue_poolPop(Data_Pool *pool)
{
	if(!pool)
	{
		return NULL;
	}
	
	return pool->pool ? (RGB_Entry*)util_cl_list_pop(pool->pool) : NULL;
}

RGB_Entry *rgb_queue_poolPush(Data_Pool *pool, RGB_Entry *data)
{
	if(!pool)
	{
		return NULL;
	}
	
	return (RGB_Entry*)util_cl_list_push(pool->pool, (void*)data);
}

UPSint stream_playback_queueReset2(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return 0;
	}
	
	if(instance->yuvPool == NULL)
	{
		return 0;
	}
	
	rgb_queue_reset(instance->yuvPool);
	
	return 0;
}

RGB_Entry *stream_playback_poolPop2(Stream_Playback *instance)
{
	RGB_Entry *pEntry = NULL;
	
	pEntry = (RGB_Entry*)rgb_queue_poolPop(instance->yuvPool);
	
	return pEntry;
}

RGB_Entry *stream_playback_poolPush2(Stream_Playback *instance, RGB_Entry *data)
{
	return rgb_queue_poolPush(instance->yuvPool, data);
}

RGB_Entry *stream_playback_listPop2(Stream_Playback *instance)
{
	RGB_Entry *pEntry = NULL;
	
	pEntry = (RGB_Entry*)rgb_queue_listPop(instance->yuvPool);
	
	return pEntry;
}

RGB_Entry *stream_playback_listPush2(Stream_Playback *instance, RGB_Entry *data)
{
	return rgb_queue_listPush(instance->yuvPool, data);
}

Stream_Playback* stream_playback_init(StreamType type)
{
	UPSint ret = 0;
	char *homePath = getenv("HOME");
	char recPath[128] = {0};

	Stream_Playback *instance = (Stream_Playback*)ucm_malloc(UP_NULL, sizeof(Stream_Playback));
	if(instance == UP_NULL)
	{
		UPLOG(stream_playback_init, ("create stream playback error"));
		return UP_NULL;
	}
	memset((void*)instance, 0x0, sizeof(Stream_Playback));
	
	instance->type = type;

	instance->codecID = CODEC_ID_H264;
	instance->codecCtx = UP_NULL;
	instance->codec = UP_NULL;
	instance->frame = UP_NULL;
	
	instance->yuvData[0] = UP_NULL;
	instance->yuvData[1] = UP_NULL;
	instance->yuvData[2] = UP_NULL;
	instance->yuvData[3] = UP_NULL;
	instance->linesize[0] = 0;
	instance->linesize[1] = 0;
	instance->linesize[2] = 0;
	instance->linesize[3] = 0;
	
	/*default cif*/
	instance->dstWidth = 352;
	instance->dstHeight = 288;

	instance->isFirst = 0;
	instance->streamTimeBase = 0;
	instance->localTimeBase = 0;
	instance->streamCtl = STREAM_NORMAL;
	instance->streamCtlStatus = 0;
	instance->streamCtlMutex = (UpMutexHandle)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((pthread_mutex_t*)instance->streamCtlMutex, NULL);
	//instance->streamCtlEvent = up_event_create();
	instance->streamCtlEvent = (UpEventHandle)malloc(sizeof(pthread_cond_t));
	pthread_cond_init((pthread_cond_t*)instance->streamCtlEvent, NULL);

	if(type == STREAM_LIVE)
	{
		instance->videoCtx = upui_fw_initCtx(1, 0);
	}
	else if(type == STREAM_LOCAL)
	{
		instance->videoCtx = upui_fw_initCtx(1, 1);
	}
	else if(type == STREAM_ONLINE)
	{
		instance->videoCtx = upui_fw_initCtx(1, 2);
	}
	if(instance->videoCtx == UP_NULL)
	{
		pthread_mutex_destroy((pthread_mutex_t*)instance->streamCtlMutex);
		free((void*)instance->streamCtlMutex);
		//up_event_destroy(instance->streamCtlEvent);
		pthread_cond_destroy((pthread_cond_t*)instance->streamCtlEvent);
		free((void*)instance->streamCtlEvent);
		ucm_free(UP_NULL, instance);
		instance = UP_NULL;
		return UP_NULL;
	}

	instance->yuvPool = pool_create(8);
	if(instance->yuvPool == UP_NULL)
	{
		pthread_mutex_destroy((pthread_mutex_t*)instance->streamCtlMutex);
		free((void*)instance->streamCtlMutex);
		//up_event_destroy(instance->streamCtlEvent);
		pthread_cond_destroy((pthread_cond_t*)instance->streamCtlEvent);
		free((void*)instance->streamCtlEvent);
		upui_fw_finiCtx(instance->videoCtx);
		instance->videoCtx = UP_NULL;
		ucm_free(UP_NULL, instance);
		instance = UP_NULL;
		return UP_NULL;
	}

	/*pool data size: 512K*/
	ret = rgb_queue_init(instance->yuvPool, 512*1024);
	if(ret < 0)
	{
		pthread_mutex_destroy((pthread_mutex_t*)instance->streamCtlMutex);
		free((void*)instance->streamCtlMutex);
		//up_event_destroy(instance->streamCtlEvent);
		pthread_cond_destroy((pthread_cond_t*)instance->streamCtlEvent);
		free((void*)instance->streamCtlEvent);
		upui_fw_finiCtx(instance->videoCtx);
		instance->videoCtx = UP_NULL;
		pool_release(instance->yuvPool);
		instance->yuvPool = UP_NULL;
		ucm_free(UP_NULL, instance);
		instance = UP_NULL;
		return UP_NULL;
	}
	
	instance->downloadPrecent = 0;
	instance->downloadFileSize = 0;
	instance->externMod = UP_NULL;
		
	/*create DvrRecord for local record*/
	homePath = getenv("HOME");
	sprintf(recPath, "%s/DvrRecord", homePath);
	
	if(kt_util_isExist(recPath) == 0)
	{
		UPLOG(stream_playback_init, ("create tmp path for online playback"));
		mkdir((const char*)recPath, 0777);
	}

	return instance;
}

void stream_playback_fini(Stream_Playback *instance)
{
	if(instance == UP_NULL)
	{
		return;
	}
	
	if(instance->streamCtlMutex)
	{
		pthread_mutex_destroy((pthread_mutex_t*)instance->streamCtlMutex);
		free((void*)instance->streamCtlMutex);
	}
	if(instance->streamCtlEvent)
	{
		//up_event_destroy(instance->streamCtlEvent);
		pthread_cond_destroy((pthread_cond_t*)instance->streamCtlEvent);
		free((void*)instance->streamCtlEvent);
	}

	if(instance->videoCtx)
	{
		upui_fw_stopCtx(instance->videoCtx);
		upui_fw_stopSrv(instance->videoCtx);
		upui_fw_finiCtx(instance->videoCtx);
		instance->videoCtx = UP_NULL;
	}

	/*release avcodec*/
	stream_playback_finiDecode(instance);
	
	/*release yuv data buffer*/
	if(instance->yuvData[0])
	{
		ucm_free(UP_NULL, instance->yuvData[0]);
		instance->linesize[0] = 0;
		instance->yuvData[0] = UP_NULL;
	}
	if(instance->yuvData[1])
	{
		ucm_free(UP_NULL, instance->yuvData[1]);
		instance->linesize[1] = 0;
		instance->yuvData[1] = UP_NULL;
	}
	if(instance->yuvData[2])
	{
		ucm_free(UP_NULL, instance->yuvData[2]);
		instance->linesize[2] = 0;
		instance->yuvData[2] = UP_NULL;
	}
	if(instance->yuvData[3])
	{
		ucm_free(UP_NULL, instance->yuvData[3]);
		instance->linesize[3] = 0;
		instance->yuvData[3] = UP_NULL;
	}

	if(instance->yuvPool)
	{
		rgb_queue_fini(instance->yuvPool);
		pool_release(instance->yuvPool);
		instance->yuvPool = UP_NULL;
	}

	ucm_free(UP_NULL, instance);
	instance = UP_NULL;
}

void stream_playback_initCodec()
{
	if(m_codec_initFlag == 0)
	{
		avcodec_init();
		avcodec_register_all();
		m_codec_initFlag = 1;
	}
}

UPSint stream_playback_initDecode(Stream_Playback *instance, enum CodecID codecId)
{
	instance->isFirst = 0;
	instance->streamTimeBase = 0;
	instance->localTimeBase = 0;

	instance->codecID = codecId;
	if((instance->codec = avcodec_find_decoder(instance->codecID)) == NULL)
	{
		UPLOG(stream_playback_initDecode, ("not find codec, maybe need recompile ffmpeg"));

		return -1;
	}

	if((instance->codecCtx = avcodec_alloc_context()) == NULL)
	{
		UPLOG(stream_playback_initDecode, ("alloc avcodec context error"));

		return -1;
	}

	if((instance->frame = avcodec_alloc_frame()) == NULL)
	{
		UPLOG(stream_playback_initDecode, ("alloc avcodec frame error"));
		av_free(instance->codecCtx);
		instance->codecCtx = UP_NULL;

		return -1;
	}
	
	if((instance->rgbFrame = avcodec_alloc_frame()) == NULL)
	{
		UPLOG(stream_playback_initDecode, ("alloc avcodec rgb frame error"));
		avcodec_close(instance->codecCtx);
		av_free(instance->codecCtx);
		av_free(instance->frame);
		
		return -1;
	}

	if(avcodec_open(instance->codecCtx, instance->codec) < 0)
	{
		UPLOG(stream_playback_initDecode, ("open avcodec error"));
		avcodec_close(instance->codecCtx);
		av_free(instance->codecCtx);
		av_free(instance->frame);

		return -1;
	}

	return 0;
}

UPSint stream_playback_finiDecode(Stream_Playback *instance)
{
	if(instance->codecCtx)
	{
		avcodec_close(instance->codecCtx);
		av_free(instance->codecCtx);
		instance->codecCtx = UP_NULL;
		instance->codec = UP_NULL;
	}

	if(instance->frame)
	{
		av_free(instance->frame);
		instance->frame = UP_NULL;
	}
	
	if(instance->rgbFrame)
	{
		av_free(instance->rgbFrame);
		instance->rgbFrame = UP_NULL;
	}

	return 0;
}

UPSint stream_playback_resetDecode(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	
	stream_playback_finiDecode(instance);
	stream_playback_initDecode(instance, CODEC_ID_H264);
	
	return 0;
}

UPSint stream_playback_decode(Stream_Playback *instance, char *inData, int inDataLen, char *outData[], int *outLineSize[], int *outGotPicture, int *outWidth, int *outHeight)
{
	int nLen = 0;

	if(instance->codecCtx == NULL && instance->codec == NULL)
	{
		return -1;
	}

	nLen = avcodec_decode_video(instance->codecCtx, instance->frame, outGotPicture, inData, inDataLen);
	if(*outGotPicture > 0)
	{
		(outData)[0] = (char*)instance->frame->data[0];
		(outData)[1] = (char*)instance->frame->data[1];
		(outData)[2] = (char*)instance->frame->data[2];

		outLineSize[0] = &instance->frame->linesize[0];
		outLineSize[1] = &instance->frame->linesize[1];
		outLineSize[2] = &instance->frame->linesize[2];

		*outWidth = instance->codecCtx->width;
		*outHeight = instance->codecCtx->height;
	}

	return nLen;
}

static void yuv_save(AVFrame *pic, int width, int height, char *path)
{
	int i;
	FILE *fp = NULL;
	int line_y = pic->linesize[0];
	int line_u = pic->linesize[1];
	int line_v = pic->linesize[2];
	int xsize = width;
	int ysize = height;

	unsigned char *buf_y = pic->data[0];
	unsigned char *buf_u = pic->data[1];
	unsigned char *buf_v = pic->data[2];

	fp = fopen(path, "a");
	if(fp == NULL)
	{
		return;
	}
	for(i = 0; i < ysize; ++i)
	{
		fwrite(buf_y + i*line_y, xsize, 1, fp);
	}
	for(i = 0; i < ysize/2; ++i)
	{
		fwrite(buf_u + i*line_u, xsize/2, 1, fp);
	}
	for(i = 0; i < ysize/2; ++i)
	{
		fwrite(buf_v + i*line_v, xsize/2, 1, fp);
	}

	fclose(fp);
}

UPSint stream_playback_setDstSize(Stream_Playback *instance, UPUint width, UPUint height)
{
	if(instance)
	{
		instance->dstWidth = width;
		instance->dstHeight = height;
		
		return 0;
	}
	
	return -1;
}

UPSint stream_playback_setFilePath(Stream_Playback *instance, const UPSint8* path)
{
	UPSint ret;
	
	if(instance == UP_NULL)
	{
		return -1;
	}
	
	ret = upui_fw_setCtxFilePath(instance->videoCtx, path);
	
	return ret;
}

UPSint stream_playback_getFilePos(Stream_Playback *instance)
{
	if(instance == UP_NULL)
	{
		return -1;
	}
	
	return upui_fw_getSrvFilePos(instance->videoCtx);
}

UPSint stream_playback_setFilePos(Stream_Playback *instance, UPSint pos)
{
	if(instance == UP_NULL)
	{
		UPLOG(stream_playback_setFilePos, ("parameter error"));
		return -1;
	}
	return upui_fw_setSrvFilePos(instance->videoCtx, pos);
}

UPSint stream_playback_getNetworkStatus(Stream_Playback *instance)
{
	UPSint ret;
	
	ret = upui_fw_getNetworkStatus(instance->videoCtx);
	
	if(ret == NETWORK_TIMEOUT || ret == NETWORK_DROPLINE)
	{
		ret = 1;
	}
	else
	{
		ret = 0;
	}

	return ret;
}

/*callback function for video data handler*/
static void stream_playback_process(void *param, void *data, int len, int dropFrame, int fps)
{
	int ret;
	int count = 0;
	int rgbLen = 0;
	int got_picture = 0;
	char *outData[4];
	int *outLineSize[4];
	int width, height;
	UPUint localInterval = 0;
	UPUint frameIntreval = 0;
#ifdef FFMPEG_SAVE_YUV
	FILE *fp = NULL;
#endif /*FFMPEG_SAVE_YUV*/
	RGB_Entry *yuvEntry = NULL;
	RGB_Render *yuvRender = NULL;
	struct SwsContext *img_convert_ctx = NULL;

	PRC_data_entry_str pEntry = UP_NULL;
	Stream_Playback *instance = (Stream_Playback*)param;

	pEntry = (PRC_data_entry_str)data;
	
#if 1
	ret = stream_playback_decode(instance, pEntry->data, pEntry->dataLen, outData, outLineSize, &got_picture, &width, &height);
	if(got_picture > 0)
	{
#if defined(KT_VIDEO_SYNC) && KT_VIDEO_SYNC == 1
		if(instance->isFirst == 1)
		{
			localInterval = up_util_getCurrentTick() - instance->localTimeBase;
			frameIntreval = (UPUint)(pEntry->pts - instance->streamTimeBase);
			frameIntreval = frameIntreval/1000;	
		}
		
		if(instance->isFirst == 0)
		{
			/*do something here*/
			instance->isFirst = 1;
			instance->streamTimeBase = pEntry->pts;
			instance->localTimeBase = up_util_getCurrentTick();
			printf("(%d)instance->streamTimeBase: %lld \n", pthread_self(), instance->streamTimeBase);
			printf("(%d)instance->localTimeBase: %d \n", pthread_self(), instance->localTimeBase);
		}
		
		if(instance->streamCtl == STREAM_NORMAL)
		{
			if(localInterval < frameIntreval)
			{
				//UPLOG(stream_playback_process, ("(%d)interval: %d", pthread_self(), frameIntreval - localInterval));
				if((frameIntreval - localInterval) < 1200)
				{
					usleep((frameIntreval - localInterval)*1000);
				}
				else /*reset time base*/
				{
					UPLOG(stream_playback_process, ("11111(%d)interval: %d", pthread_self(), frameIntreval - localInterval));
					instance->streamTimeBase = pEntry->pts;
					instance->localTimeBase = up_util_getCurrentTick();
				}

			}
			else if(localInterval > frameIntreval)
			{
				if(localInterval - frameIntreval > 500)
				{
					//usleep(80000);
					UPLOG(stream_playback_process, ("22222(%d)interval: %d", pthread_self(), localInterval - frameIntreval));
					instance->streamTimeBase = pEntry->pts;
					instance->localTimeBase = up_util_getCurrentTick();
					//usleep(1000000);
				}
			}
		}
		else if(instance->streamCtl == STREAM_FF_2X)
		{
			UPLOG(stream_playback_process, ("stream ff 2x"));
			usleep(10000);
		}
		else if(instance->streamCtl == STREAM_FF_4X)
		{
			UPLOG(stream_playback_process, ("stream ff 4x"));
			usleep(8000);
		}
		else if(instance->streamCtl == STREAM_FF_8X)
		{
			UPLOG(stream_playback_process, ("stream ff 8x"));
		}
		else if(instance->streamCtl == STREAM_SF_2X)
		{
			UPLOG(stream_playback_process, ("stream sf 2x"));
			usleep(100000);
		}
		else if(instance->streamCtl == STREAM_SF_4X)
		{
			UPLOG(stream_playback_process, ("stream sf 4x"));
			usleep(400000);
		}
		else if(instance->streamCtl == STREAM_SF_8X)
		{
			UPLOG(stream_playback_process, ("stream sf 8x"));
			usleep(800000);
		}
		else if(instance->streamCtl == STREAM_PAUSE || instance->streamCtl == STREAM_STEP)
		{
			pthread_mutex_lock((pthread_mutex_t*)instance->streamCtlMutex);
			instance->streamCtlStatus = 1;
			up_event_wait(instance->streamCtlEvent, instance->streamCtlMutex, UP_EVENT_WAIT_INFINITE);
			pthread_mutex_unlock((pthread_mutex_t*)instance->streamCtlMutex);
		}
#endif /*KT_VIDEO_SYNC*/
		
		width = instance->dstWidth;
		height = instance->dstHeight;
		if(width == 692 && height == 552)
		{
			//UPLOG(stream_playback_process, ("src width: %d, src height: %d", instance->codecCtx->width, instance->codecCtx->height));
		}

		count = 0;
		while((yuvEntry = stream_playback_poolPop2(instance)) == UP_NULL)
		{
			/*drop frame here*/
			if(count == 10)
			{
				UPLOG(stream_playback_process, ("(%d)drop one frame 2", pthread_self()));
				count = 0;
				break;
			}
			//else
			{
				count++;
				usleep(10);
			}
		}
		if(yuvEntry == UP_NULL)
		{
			return;
		}
		yuvRender = (RGB_Render*)yuvEntry->data;
		if(yuvRender == NULL)
		{
			printf("yuvRender not available \n");
			return ;
		}
		rgbLen = avpicture_get_size(PIX_FMT_ARGB, width, height);
		
		if(yuvRender->outDataSize < rgbLen)
		{
			if(yuvRender->outData)
				av_free(yuvRender->outData);
			yuvRender->outData = (char*)av_malloc(rgbLen);
			if(yuvRender->outData == NULL)
			{
				UPLOG(stream_playback_process, ("not enought memory"));
			}
			yuvRender->outDataSize = rgbLen;
		}
		
		avpicture_fill((AVPicture*)(instance->rgbFrame), yuvRender->outData, PIX_FMT_ARGB, width, height);
		
		if(instance->linesize[0] < *outLineSize[0])
		{
			if(instance->yuvData[0])
				ucm_free(UP_NULL, instance->yuvData[0]);
			instance->yuvData[0] = (UPUint8*)ucm_malloc(UP_NULL, (*outLineSize[0]) * instance->codecCtx->height);
			instance->linesize[0] = *outLineSize[0];
		}
		instance->linesize[0] = *outLineSize[0];
		memcpy((void*)instance->yuvData[0], outData[0], (*outLineSize[0]) * instance->codecCtx->height);
		
		if(instance->linesize[1] < *outLineSize[1])
		{
			if(instance->yuvData[1])
				ucm_free(UP_NULL, instance->yuvData[1]);
			instance->yuvData[1] = (UPUint8*)ucm_malloc(UP_NULL, ((*outLineSize[1]) * instance->codecCtx->height)/2);
			instance->linesize[1] = *outLineSize[1];
		}
		instance->linesize[1] = *outLineSize[1];
		memcpy((void*)instance->yuvData[1], outData[1], ((*outLineSize[1]) * instance->codecCtx->height)/2);
		
		if(instance->linesize[2] < *outLineSize[2])
		{
			if(instance->yuvData[2])
				ucm_free(UP_NULL, instance->yuvData[2]);
			instance->yuvData[2] = (UPUint8*)ucm_malloc(UP_NULL, ((*outLineSize[2]) * instance->codecCtx->height)/2);
			instance->linesize[2] = *outLineSize[2];
		}
		instance->linesize[2] = *outLineSize[2];
		memcpy((void*)instance->yuvData[2], outData[2], ((*outLineSize[2]) * instance->codecCtx->height)/2);		
		
		img_convert_ctx = sws_getContext(instance->codecCtx->width, instance->codecCtx->height, instance->codecCtx->pix_fmt, width, height, PIX_FMT_ARGB, SWS_BICUBIC, NULL, NULL, NULL);
		if(img_convert_ctx == NULL)
		{
			UPLOG(stream_playback_process, ("get sws context error"));
		}
		if(img_convert_ctx)
		{
			sws_scale(img_convert_ctx, instance->yuvData, instance->linesize, 0, instance->codecCtx->height, (AVPicture*)(instance->rgbFrame)->data, instance->rgbFrame->linesize);			
			sws_freeContext(img_convert_ctx);
		}
		yuvEntry->width = width;
		yuvEntry->height = height;

#if defined(KT_SAVE_YUV) && KT_SAVE_YUV == 1
		fp = fopen("./yuv.data", "a");
		for(i = 0; i < height; ++i)
		{
			fwrite(yuvRender->outData[0] + i*(yuvRender->outlinesize[0]), width, 1, fp);
		}
		for(i = 0; i < height/2; ++i)
		{
			fwrite(yuvRender->outData[1] + i*(yuvRender->outlinesize[1]), width/2, 1, fp);
		}
		for(i = 0; i < height/2; ++i)
		{
			fwrite(yuvRender->outData[2] + i*(yuvRender->outlinesize[2]), width/2, 1, fp);
		}
		fclose(fp);
#endif /*KT_SAVE_YUV*/

		stream_playback_listPush2(instance, yuvEntry);

	}
#endif

	return;
}

static UPUint m_vChannel = 0;
UPSint stream_playback_setup(Stream_Playback *instance, UPUint channel)
{
	UPSint ret = 0;
	if(instance == NULL)
	{
		return 0;
	}
	stream_playback_initCodec();
	ret = stream_playback_initDecode(instance, CODEC_ID_H264);
	if(ret < 0)
	{
		/*init decode error*/
		return -1;
	}
	
	UPLOG(stream_playback_setup, ("stream playback setup information"));
	stream_playback_setUsrInfo(instance, (UPSint8*)"kelvin", (UPSint8*)"123456");
	stream_playback_setNetInfo(instance, (UPSint8*)"192.168.0.187", 9977, 0);
	stream_playback_setVideoChannel(instance, channel);
	m_vChannel++;

	upui_fw_setHandleCBCtx(instance->videoCtx, stream_playback_process, (void*)instance);

	return 0;
}

UPSint stream_playback_setUsrInfo(Stream_Playback *instance, UPSint8* name, UPSint8* password)
{
	if(instance == UP_NULL || instance->videoCtx == UP_NULL)
	{
		UPLOG(stream_playback_setUsrInfo, ("instance is NULL"));
		return -1;
	}
	
	return upui_fw_setCtxUsrInfo(instance->videoCtx, name, password);
}

UPSint stream_playback_setNetInfo(Stream_Playback *instance, UPSint8* addr, UPSint32 port, UPUint type)
{
	if(instance == UP_NULL || instance->videoCtx == UP_NULL)
	{
		UPLOG(stream_playback_setNetInfo, ("instance is NULL"));
		return -1;
	}
	
	return upui_fw_setCtxNetInfo(instance->videoCtx, addr, port, type);
}

UPSint stream_playback_setVideoChannel(Stream_Playback *instance, UPUint channel)
{
	upui_fw_setCtxChannel(instance->videoCtx, channel);
	
	return 0;
}

UPSint stream_playback_getDownloadPrecent(Stream_Playback *instance)
{
	return instance->downloadPrecent;
}

UPSint stream_playback_setDownloadPrecent(Stream_Playback *instance, UPSint precent)
{
	instance->downloadPrecent = precent;
	
	return 0;
}

UPSint stream_playback_setDownloadFileSize(Stream_Playback *instance, UPUint size)
{
	if(instance == UP_NULL)
	{
		UPLOG(stream_playback_setDownloadFileSize, ("invailable instance"));
		return -1;
	}
	
	UPLOG(stream_playback_setDownloadFileSize, ("size: %d", size));
	
	return upui_fw_setSrvFileSize(instance->videoCtx, size);
}

UPUint stream_playback_getVideoChannel(Stream_Playback *instance)
{
	return upui_fw_getCtxChannel(instance->videoCtx);
}

UPSint stream_playback_start(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return 0;
	}
	stream_playback_queueReset2(instance);
	upui_fw_resetFramePool(instance->videoCtx);
	upui_fw_startSrv(instance->videoCtx);
	upui_fw_startCtx(instance->videoCtx);
	instance->isFirst = 0;
	instance->streamCtl = STREAM_NORMAL;

	return 0;
}

UPSint stream_playback_stop(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return 0;
	}
	if(instance->streamCtlStatus == 1)
	{
		pthread_mutex_lock((pthread_mutex_t*)instance->streamCtlMutex);
		pthread_cond_signal((pthread_cond_t*)instance->streamCtlEvent);
		instance->streamCtlStatus = 0;
		instance->streamCtl = STREAM_STOP;
		pthread_mutex_unlock((pthread_mutex_t*)instance->streamCtlMutex);
	}
	upui_fw_stopCtx(instance->videoCtx);
	upui_fw_stopSrv(instance->videoCtx);
	instance->isFirst = 0;
	instance->streamCtl = STREAM_STOP;

	return 0;
}

static UPSint m_playback_notify(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return 0;
	}
	
	if(instance->streamCtlStatus == 1)
	{
		pthread_mutex_lock((pthread_mutex_t*)instance->streamCtlMutex);
		pthread_cond_signal((pthread_cond_t*)instance->streamCtlEvent);
		instance->streamCtlStatus = 0;
		pthread_mutex_unlock((pthread_mutex_t*)instance->streamCtlMutex);
	}
	
	return 0;
}

UPSint stream_playback_normal(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return 0;
	}
	if(instance->streamCtl == STREAM_NORMAL)
		return 0;
	
	if(instance->streamCtlStatus == 1)
	{
		pthread_mutex_lock((pthread_mutex_t*)instance->streamCtlMutex);
		pthread_cond_signal((pthread_cond_t*)instance->streamCtlEvent);
		instance->streamCtlStatus = 0;
		instance->streamCtl = STREAM_NORMAL;
		pthread_mutex_unlock((pthread_mutex_t*)instance->streamCtlMutex);
	}
	
	instance->streamCtl = STREAM_NORMAL;
	instance->isFirst = 0;

	return 0;
}

UPSint stream_playback_pause(Stream_Playback *instance)
{
	if(instance == NULL)
		return -1;
	if(instance->streamCtl == STREAM_PAUSE)
		return 0;
	
	if(instance->streamCtlStatus == 1)
	{
		pthread_mutex_lock((pthread_mutex_t*)instance->streamCtlMutex);
		pthread_cond_signal((pthread_cond_t*)instance->streamCtlEvent);
		instance->streamCtlStatus = 0;
		instance->streamCtl = STREAM_PAUSE;
		pthread_mutex_unlock((pthread_mutex_t*)instance->streamCtlMutex);
	}
	
	instance->streamCtl = STREAM_PAUSE;

	return 0;
}

UPSint stream_playback_fastForward(Stream_Playback *instance, UPSint speed)
{
	if(instance == NULL)
		return -1;
	
	if(instance->streamCtl == STREAM_FF_2X && speed == 1)
		return 0;
	if(instance->streamCtl == STREAM_FF_4X && speed == 2)
		return 0;
	if(instance->streamCtl == STREAM_FF_8X && speed == 3)
		return 0;
	
	if(instance->streamCtlStatus == 1)
	{
		pthread_mutex_lock((pthread_mutex_t*)instance->streamCtlMutex);
		pthread_cond_signal((pthread_cond_t*)instance->streamCtlEvent);
		instance->streamCtlStatus = 0;
		if(speed == 1)
		{
			instance->streamCtl = STREAM_FF_2X;
		}
		else if(speed == 2)
		{
			instance->streamCtl = STREAM_FF_4X;
		}
		else if(speed == 3)
		{
			instance->streamCtl = STREAM_FF_8X;
		}
		pthread_mutex_unlock((pthread_mutex_t*)instance->streamCtlMutex);
	}
	
	if(speed == 1)
	{
		instance->streamCtl = STREAM_FF_2X;
	}
	else if(speed == 2)
	{
		instance->streamCtl = STREAM_FF_4X;
	}
	else if(speed == 3)
	{
		instance->streamCtl = STREAM_FF_8X;
	}

	return 0;
}

UPSint stream_playback_slowForward(Stream_Playback *instance, UPSint speed)
{
	if(instance == NULL)
		return -1;
	
	if(instance->streamCtl == STREAM_SF_2X && speed == 1)
		return 0;
	if(instance->streamCtl == STREAM_SF_4X && speed == 2)
		return 0;
	if(instance->streamCtl == STREAM_SF_8X && speed == 3)
		return 0;
	
	if(instance->streamCtlStatus == 1)
	{
		pthread_mutex_lock((pthread_mutex_t*)instance->streamCtlMutex);
		pthread_cond_signal((pthread_cond_t*)instance->streamCtlEvent);
		instance->streamCtlStatus = 0;
		if(speed == 1)
		{
			instance->streamCtl = STREAM_SF_2X;
		}
		else if(speed == 2)
		{
			instance->streamCtl = STREAM_SF_4X;
		}
		else if(speed == 3)
		{
			instance->streamCtl = STREAM_SF_8X;
		}
		pthread_mutex_unlock((pthread_mutex_t*)instance->streamCtlMutex);
	}
	
	if(speed == 1)
	{
		instance->streamCtl = STREAM_SF_2X;
	}
	else if(speed == 2)
	{
		instance->streamCtl = STREAM_SF_4X;
	}
	else if(speed == 3)
	{
		instance->streamCtl = STREAM_SF_8X;
	}
	
	return 0;
}

UPSint stream_playback_step(Stream_Playback *instance)
{
	if(instance == NULL)
		return -1;
	
	if(instance->streamCtlStatus == 1)
	{
		pthread_mutex_lock((pthread_mutex_t*)instance->streamCtlMutex);
		pthread_cond_signal((pthread_cond_t*)instance->streamCtlEvent);
		instance->streamCtlStatus = 0;
		instance->streamCtl= STREAM_STEP;
		pthread_mutex_unlock((pthread_mutex_t*)instance->streamCtlMutex);
	}
	
	instance->streamCtl = STREAM_STEP;
	
	return 0;
}

UPSint stream_playback_startRecord(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	
	return upui_fw_startSrvRecord(instance->videoCtx);
}

UPSint stream_playback_stopRecord(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	
	return upui_fw_stopSrvRecord(instance->videoCtx);
}

UPSint stream_playback_ptzUp(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	upui_fw_srvPtzUp(instance->videoCtx, 0);
	return 0;
}

UPSint stream_playback_ptzDown(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	upui_fw_srvPtzDown(instance->videoCtx, 0);
	return 0;
}

UPSint stream_playback_ptzLeft(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	upui_fw_srvPtzLeft(instance->videoCtx, 0);
	return 0;
}

UPSint stream_playback_ptzRight(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	upui_fw_srvPtzRight(instance->videoCtx, 0);
	return 0;
}

UPSint stream_playback_ptzZoomin(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	upui_fw_srvPtzZoomin(instance->videoCtx, 0);
	return 0;
}

UPSint stream_playback_ptzZoomout(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	upui_fw_srvPtzZoomout(instance->videoCtx, 0);
	return 0;
}

UPSint stream_playback_ptzFocusFar(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	upui_fw_srvPtzFocusFar(instance->videoCtx, 0);
	return 0;
}

UPSint stream_playback_ptzFocusNear(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	upui_fw_srvPtzFocusNear(instance->videoCtx, 0);
	return 0;
}

UPSint stream_playback_ptzIRISOpen(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	upui_fw_srvPtzIrisOpen(instance->videoCtx, 0);
	return 0;
}

UPSint stream_playback_ptzIRISClose(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	upui_fw_srvPtzIrisClose(instance->videoCtx, 0);
	return 0;
}

UPSint stream_playback_ptzCruisePreSet(Stream_Playback *instance, UPUint para)
{
	if(instance == NULL)
	{
		return -1;
	}
	
	return upui_fw_srvPtzCruisePreSet(instance->videoCtx, para);
}

UPSint stream_playback_ptzCruiseRecall(Stream_Playback *instance, UPUint para)
{
	if(instance == NULL)
	{
		return -1;
	}
	
	return upui_fw_srvPtzCruiseRecall(instance->videoCtx, para);
	
}

UPSint stream_playback_ptzCruiseStart(Stream_Playback *instance, UPUint para)
{
	if(instance == NULL)
	{
		return -1;
	}
	
	return upui_fw_srvPtzCruiseStart(instance->videoCtx, para);
}

UPSint stream_playback_ptzCruiseClear(Stream_Playback *instance, UPUint para)
{
	if(instance == NULL)
	{
		return -1;
	}
	
	return upui_fw_srvPtzCruiseClear(instance->videoCtx, para);
}

UPSint stream_playback_ptzCruiseLoad(Stream_Playback *instance, UPUint para)
{
	if(instance == NULL)
	{
		return -1;
	}

	return upui_fw_srvPtzCruiseLoad(instance->videoCtx, para);
}

UPSint stream_playback_ptzCruiseSave(Stream_Playback *instance, UPUint para)
{
	if(instance == NULL)
	{
		return -1;
	}
	
	return upui_fw_srvPtzCruiseSave(instance->videoCtx, para);
}

UPSint stream_playback_ptzCruiseStop(Stream_Playback *instance, UPUint para)
{
	if(instance == NULL)
	{
		return -1;
	}
	
	return upui_fw_srvPtzCruiseStop(instance->videoCtx, para);
}

UPSint stream_playback_ptzStop(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return -1;
	}
	upui_fw_srvPtzStop(instance->videoCtx, 0);
	return 0;
}

void *stream_playback_AudioListPop(Stream_Playback *instance)
{
	if(instance == NULL)
	{
		return NULL;
	}
	
	return upui_fw_srvAudioListPop(instance->videoCtx);
}

void *stream_playback_AudioPoolPush(Stream_Playback *instance, void *data)
{
	if(instance == NULL)
	{
		return NULL;
	}
	
	return upui_fw_srvAudioPoolPush(instance->videoCtx, data);
}

Playback_Online* playback_online_init(UPSint8 *addr, UPSint32 port)
{
	Playback_Online *instance = UP_NULL;
	
	instance = (Playback_Online*)ucm_malloc(UP_NULL, sizeof(Playback_Online));
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_init, ("init playback online error, not enought memory"));
		return NULL;
	}
	memset((void*)instance, 0x0, sizeof(Playback_Online));
	
	instance->searchMgr = upui_fw_srvFileMgrInit();
	if(instance->searchMgr == UP_NULL)
	{
		UPLOG(playback_online_init, ("init playback online error, init search manage error"));
		ucm_free(UP_NULL, instance);
		instance = UP_NULL;
		return instance;
	}
	
	upui_fw_srvFileMgrSetServer(instance->searchMgr, addr, port);
	
	if(addr != UP_NULL)
	{
		if(instance->addr)
		{
			ucm_free(UP_NULL, instance->addr);
			instance->addr = UP_NULL;
		}
		instance->addr = (UPSint8*)ucm_strdup(UP_NULL, (const char*)addr);
	}
	instance->port = port;
	
	instance->calendaMap = 0;
	
	instance->dstFile = UP_NULL;
	instance->dstFileSize = 0;
	instance->dstRecvFileSize = 0;
	
	instance->cacheFileList = UP_NULL;
	instance->remoteFileList = UP_NULL;
	
	/*create tmp document*/

	return instance;
}

void playback_online_fini(Playback_Online *instance)
{
	UPLOG(playback_online_fini, ("finish start"));
	if(instance == UP_NULL)
	{
		return ;
	}
	
	if(instance->addr)
	{
		ucm_free(UP_NULL, instance->addr);
		instance->addr = UP_NULL;
	}
	
	if(instance->cacheFileList)
	{
		/*delete cache file list*/
		kt_fileMgr_listFini(instance->cacheFileList);
	}
	
	if(instance->remoteFileList)
	{
		/*delete remote file list*/
		kt_fileMgr_listFini(instance->remoteFileList);
	}
	
	if(instance->searchMgr)
	{
		/*delete search Mgr*/
		upui_fw_srvFileMgrFini(instance->searchMgr);
		instance->searchMgr = UP_NULL;
	}
		
	ucm_free(UP_NULL, instance);
}

static void m_dump_calenda(Playback_Online *instance)
{
	UPUint i;
	
	if(instance == UP_NULL)
	{
		return ;
	}
	
	for(i = 0; i < 32; i++)
	{
		if(instance->calendaMap & (UPUint)(0x01 << i))
		{
			UPLOG(m_dump_calenda, ("day: %i, have record file", i+1));
		}
	}
}

void playback_online_getCalenda(Playback_Online *instance, UPUint8 year, UPUint8 month, UPUint8 day, UPUint8 type)
{
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_getCalenda, ("invailable instance"));
		return ;
	}
	if(instance->addr == UP_NULL)
	{
		UPLOG(playback_online_getCalenda, ("not available ip address"));
		return ;
	}
	
	UPLOG(playback_online_getCalenda, ("srv get calenda"));
	instance->calendaMap = upui_fw_srvGetCalenda(instance->addr, instance->port, year, month, day, type);

	m_dump_calenda(instance);
}

void playback_online_getDescription(Playback_Online *instance, UPSint8 *description)
{
	UPSint i = 0;
	UPSint8 *tmp;
	int flag = 0;
	
	tmp = description;
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_getDescription, ("invailable instance"));
		return ;
	}
	
	if(instance->calendaMap == 0)
	{
		UPLOG(playback_online_getDescription, ("no record all month"));
		//strcpy((char*)description, (char*)"No Record at this month");
		return ;
	}
	
	//sprintf((char*)description, "%s: ", "Record day");
	//tmp = description + strlen((char*)description);
	
	for(i = 0; i < 32; i++)
	{
		if(instance->calendaMap & ((UPUint)(0x01 << i)))
		{
			UPLOG(playback_online_getDescription, ("this day have record"));
			if(flag == 0)
			{
				sprintf((char*)tmp, "%d", i+1);
				flag = 1;
			}
			else
			{
				sprintf((char*)tmp, ", %d", i+1);
			}
			
			tmp = description + strlen((char*)description);
		}
	}
	
}

UPSint playback_online_getSearchMgr(Playback_Online *instance, UPUint8 year, UPUint8 month, UPUint8 day)
{
	UPSint ret = 0;
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_getSearchMgr, ("not available instance"));
		return -1;
	}
	
	if(instance->searchMgr->year == year && instance->searchMgr->month == month && instance->searchMgr->day == day)
	{
		UPLOG(playback_online_getSearchMgr, ("the same day"));
		return 1;
	}
	
	ret = upui_fw_srvSearchFile(instance->searchMgr, instance->addr, instance->port, 0xff, year, month, day, 3);
	
	if(instance->searchMgr == UP_NULL)
	{
		UPLOG(playback_online_getSearchMgr, ("error occur or no record this day"));
		return -1;
	}
	
	if(ret != 0)
	{
		UPLOG(playback_online_getSearchMgr, ("error occur or no record this day"));
		return -1;
	}
	
	return 0;
}

#ifdef KT_GET_FILELIST_ASYNC
/*return file Numbers*/
UPSint playback_online_getOnlineFileNumber(Playback_Online *instance, UPUint8 year, UPUint8 month, UPUint8 day)
{
	UPSint ret = 0;
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_getOnlineFileNumber, ("not available instance"));
		return -1;
	}
	
	if(instance->searchMgr == UP_NULL)
	{
		UPLOG(playback_online_getOnlineFileNumber, ("error occur or no record this day"));
		return -1;
	}
	
	if(instance->searchMgr->year == year && instance->searchMgr->month == month && instance->searchMgr->day == day)
	{
		UPLOG(playback_online_getOnlineFileNumber, ("the same day"));
		return 1;
	}
	
	ret = upui_fw_srvSearchFileGetNumber(instance->searchMgr, instance->addr, instance->port, 0xffff, year, month, day, 3);
	
	if(ret <= 0)
	{
		UPLOG(playback_online_getOnlineFileNumber, ("error occur or no record this day"));
		return -1;
	}
	
	return ret;
}

UPSint playback_online_getOnlineFilePage(Playback_Online *instance, UPUint8 year, UPUint8 month, UPUint8 day, UPUint page)
{
	UPSint ret = 0;
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_getOnlineFilePage, ("not available instance"));
		return -1;
	}
	
	if(instance->searchMgr == UP_NULL)
	{
		UPLOG(playback_online_getOnlineFilePage, ("error occur or no record this day"));
		return -1;
	}
	
	if(instance->searchMgr->year == year && instance->searchMgr->month == month && instance->searchMgr->day == day)
	{
		UPLOG(playback_online_getOnlineFilePage, ("the same day"));
		return 1;
	}
	
	ret = upui_fw_srvSearchFileGetPage(instance->searchMgr, instance->addr, instance->port, 0xffff, year, month, day, 3, page);
	
	//	if(ret <= 0)
	//	{
	//		UPLOG(playback_online_getOnlineFileNumber, ("error occur or no record this day"));
	//		return -1;
	//	}
	
	return ret;
}
#endif /*KT_GET_FILELIST_ASYNC*/

UPSint playback_online_startDownload(Playback_Online *instance, UPSint8 *name, UPUint offset)
{
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_getSearchMgr, ("not available instance"));
		return -1;
	}
	
	upui_fw_srvFileMgrSetDst(instance->searchMgr, name, offset);
	
	return 0;
}

UPSint playback_online_stopDownload(Playback_Online *instance)
{
	return upui_fw_srvFileMgrDownloadStop(instance->searchMgr);
}

UPSint playback_online_getDownloadSize(Playback_Online *instance)
{
	if(instance == UP_NULL)
	{
		return -1;
	}
	
	return upui_fw_srvFileMgrGetDstSize(instance->searchMgr);
}

UPSint playback_online_setSavePath(Playback_Online *instance, UPSint8 *savePath)
{
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_setSavePath, ("not available instance"));
		return -1;
	}
	
	return upui_fw_srvFileMgrSetSavePath(instance->searchMgr, savePath);
}

UPSint playback_online_setSaveName(Playback_Online *instance, UPSint8 *saveName)
{
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_setSaveName, ("not available instance"));
		return -1;
	}
	
	return upui_fw_srvFileMgrSetSaveName(instance->searchMgr, saveName);
}

UPSint playback_online_getPrecent(Playback_Online *instance)
{
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_getPrecent, ("not available instance"));
		return -1;
	}
	
	return upui_fw_srvFileMgrGetPrecent(instance->searchMgr);
}

UPSint playback_online_getCacheFileList(Playback_Online *instance, UPSint8 *cachePath)
{
	int count;
	
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_getCacheFileList, ("not available instance"));
		return -1;
	}
	
	if(instance->cacheFileList)
	{
		kt_fileMgr_listFini(instance->cacheFileList);
		instance->cacheFileList = UP_NULL;
	}
	
	instance->cacheFileList = kt_fileMgr_getRecFileInFS((char*)cachePath, ".264", 0xff, 1, &count);
	
	return 0;
}

UPSint playback_online_isFileInCache(Playback_Online *instance, UPSint8 *filePath, UPSint type)
{
	RecFile *recFile = UP_NULL;
	
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_isFileInCache, ("not available instance"));
		return -1;
	}
	
	if(instance->cacheFileList)
	{
		recFile = instance->cacheFileList;
		while(recFile)
		{
			/*handler*/
			UPLOG(playback_online_isFileInCache, ("recFile path: %s", recFile->path));
			if(type == 0)
			{
				/*path*/
				if(memcmp(recFile->path, filePath, strlen((char*)filePath)) == 0)
				{
					UPLOG(playback_online_isFileInCache, ("find this file in cache"));
					return 0;
				}
			}
			else if(type == 1)
			{
				/*name*/
				UPLOG(playback_online_isFileInCache, ("recFile name: %s", recFile->name));
				if(memcmp(recFile->name, filePath, strlen((char*)filePath)) == 0)
				{
					UPLOG(playback_online_isFileInCache, ("find this file in cache"));
					return 0;
				}
			}
			
			recFile = recFile->next;
		}
	}
	
	UPLOG(playback_online_isFileInCache, ("not found this file"));
	return -1;
}

UPUint playback_online_getFileSizeInCache(Playback_Online *instance, UPSint8 *name)
{
	int ret = 0;
	struct stat st;
	RecFile *recFile = UP_NULL;
	
	if(instance == UP_NULL)
	{
		UPLOG(playback_online_getFileSizeInCache, ("not available instance"));
		return -1;
	}
	
	if(instance->cacheFileList)
	{
		recFile = instance->cacheFileList;
		while(recFile)
		{
			if(memcmp(recFile->name, name, strlen((char*)name)) == 0)
			{
				UPLOG(playback_online_getFileSizeInCache, ("find this file in cache"));
				break;
			}
			
			recFile = recFile->next;
		}
	}
	
	ret = lstat(recFile->path, &st);
	if(ret == 0)
	{
		UPLOG(playback_online_getFileSizeInCache, ("size: %d", st.st_size));
		return (UPUint)st.st_size;
	}
	
	return 0;
}
