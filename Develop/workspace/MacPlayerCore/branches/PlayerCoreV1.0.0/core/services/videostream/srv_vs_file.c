#include "up_types.h"
#include "ucm.h"
#include "up_sys_log.h"

#include <pthread.h>

#include "srv_com_handler.h"
#include "srv_vs_file.h"
#include "up_ui_context.h"

#include <unistd.h>
#include <sys/stat.h>

/****************************************************************************
 Hi H264 PARSER
 ****************************************************************************/

UPSint vs_file_init()
{
	return 0;
}

void vs_file_fini()
{
	return;
}

UPSint vs_file_BufferHandle(SRV_Instance* instance)
{
	UPSint packLen;
	UPSint offset = 0;
	UPSint i = 0;
	char header[24];
	unsigned long long pts = 0;
	PRC_data_entry_str pEntry = UP_NULL;
	
	if(instance == UP_NULL || instance->fileHandle == UP_NULL)
	{
		return -1;
	}
	
	while(instance->fileHandle->frameBufLen - offset > 24)
	{
		memset((void*)header, 0x0, sizeof(header));
		if(memcmp(instance->fileHandle->frameBuf + 2 + offset, "dcH264", 6) == 0)
		{
			memcpy(header, instance->fileHandle->frameBuf + offset, 24);
			packLen = (*((int*)(header + 8))+7)/8*8 + 24 + (*((unsigned short*)(header + 12)) + 7) / 8 * 8;
			pts = *((unsigned long long*)(header + 16));
			if(instance->fileHandle->frameBufLen - offset >= packLen)
			{
				if(instance->fileHandle->foundIFrame == 1)
				{
					while((pEntry = (PRC_data_entry_str)srv_com_poolPop(instance)) == UP_NULL && 
						  instance->videoStreamStatus == SRV_Thread_status_running)
					{
						pthread_mutex_lock((pthread_mutex_t*)instance->videoStreamMutex);
						instance->videoStreamStatus = SRV_Thread_status_idle;
						up_event_wait(instance->videoStreamEvent, instance->videoStreamMutex, UP_EVENT_WAIT_INFINITE);
						pthread_mutex_unlock((pthread_mutex_t*)instance->videoStreamMutex);
					}
				
					if(pEntry != UP_NULL)
					{
						pEntry->dataLen = packLen - 24;
						memcpy((void*)pEntry->data, instance->fileHandle->frameBuf + 24 + offset, pEntry->dataLen);
						pEntry->pts = pts;
					
						srv_com_listPush(instance, pEntry);
						upui_ctx_notify(instance->partner);
					}
				}
				else if(memcmp(header + 1, "0dcH264", 7) == 0) /*I Frame*/
				{
					UPLOG(vs_file_BufferHandle, ("get first IFrame success"));
					while((pEntry = (PRC_data_entry_str)srv_com_poolPop(instance)) == UP_NULL && 
						  instance->videoStreamStatus == SRV_Thread_status_running)
					{
						pthread_mutex_lock((pthread_mutex_t*)instance->videoStreamMutex);
						instance->videoStreamStatus = SRV_Thread_status_idle;
						up_event_wait(instance->videoStreamEvent, instance->videoStreamMutex, UP_EVENT_WAIT_INFINITE);
						pthread_mutex_unlock((pthread_mutex_t*)instance->videoStreamMutex);
					}
					
					if(pEntry != UP_NULL)
					{
						pEntry->dataLen = packLen - 24;
						memcpy((void*)pEntry->data, instance->fileHandle->frameBuf + 24 + offset, pEntry->dataLen);
						pEntry->pts = pts;
						
						srv_com_listPush(instance, pEntry);
						upui_ctx_notify(instance->partner);
					}					
					instance->fileHandle->foundIFrame = 1;
				}
				
				/**/
				instance->fileHandle->frameBufLen -= packLen;
				instance->fileHandle->frameBufLen -= offset;
				i = 0;
				memcpy(instance->fileHandle->frameBuf, instance->fileHandle->frameBuf + packLen + offset, instance->fileHandle->frameBufLen);
				offset = 0;
			}
			else
			{
				break;
			}
			
		}
		else if(memcmp(instance->fileHandle->frameBuf + offset, "3wb", 3) == 0)
		{
			/*audio handler*/
			if(instance->fileHandle->frameBufLen - offset >= 7 + 0xA8)
			{
				/*for test*/
#ifdef AUDIO_RAW_SAVE
				FILE *fp = NULL;
				fp = fopen("/Users/kelvin/source.pcm", "a");
				if(fp)
				{
					fwrite(instance->fileHandle->frameBuf + offset, 1, 7+0xA8, fp);
					fclose(fp);
				}
#endif /*AUDIO_RAW_SAVE*/
				/*push audio to audio list*/
				pEntry = (PRC_data_entry_str)srv_com_audioPoolPop(instance);
				
				if(pEntry != UP_NULL)
				{
					pEntry->dataLen = 0xA8;
					memcpy((void*)pEntry->data, instance->fileHandle->frameBuf + 7 + offset, pEntry->dataLen);
					pEntry->pts = 0;
					
					srv_com_audioListPush(instance, pEntry);
				}
				
				instance->fileHandle->frameBufLen -= (0xA8+7);
				instance->fileHandle->frameBufLen -= offset;
				memcpy(instance->fileHandle->frameBuf, instance->fileHandle->frameBuf + 7 + 0xA8 + offset, instance->fileHandle->frameBufLen);
				
				offset = 0;
			}
			else
			{
				break;
			}
		}
		else
		{
			offset++;
		}

		
	}
	
	if(offset > 0)
	{
		memcpy(instance->fileHandle->frameBuf, instance->fileHandle->frameBuf + offset, instance->fileHandle->frameBufLen - offset);
		instance->fileHandle->frameBufLen -= offset;
	}
	
	return 0;
}

UPSint m_file_readable(FILE_Instance *instance)
{
	struct stat st;
	
	if(instance == UP_NULL)
	{
		UPLOG(m_file_readable, ("invailable FILE_Instance"));
		return 0;
	}
	
	if(stat((const char*)instance->path, &st) < 0)
	{
		UPLOG(vs_file_readable, ("file playback: stat file error"));
		return 0;
	}
	
	if(st.st_size == instance->size)
	{
		return 1;
	}
	
	if(instance->readPos + 1*1024 < st.st_size)
	{
		return 1;
	}
	
	return 0;
}

void vs_file_main(void* data)
{
	int ret = 0;
	int count = 0;
	struct stat st;
	SRV_Instance *instance = (SRV_Instance*)data;
	
	if(instance == UP_NULL)
	{
		UPLOG(vs_file_main, ("NO SRV_Instance setting"));
		instance->videoStreamStatus = SRV_Thread_status_exited;
		return;
	}
	
	UPLOG(vs_file_main, ("playback type: %d", instance->type));
	if(instance->type == SRV_ID_SZPBONLINE)
	{
		UPLOG(vs_file_main, ("Playback Online ongoing"));
		/*make sure the file is created*/
		while(up_fs_fileExist((char*)instance->fileHandle->path) == 0)
		{
			usleep(1000);
			if(count == 2000)
			{
				break;
			}
			count++;
		}
	}
	
	if(instance->fileHandle->fp)
	{
		fclose(instance->fileHandle->fp);
		instance->fileHandle->fp = NULL;
	}
	
	UPLOG(vs_file_main, ("file path: %s", instance->fileHandle->path));
	instance->fileHandle->fp = fopen((const char*)instance->fileHandle->path, "r");
	if(instance->fileHandle->fp == NULL)
	{
		UPLOG(vs_file_main, ("file playback: open file error"));
		instance->videoStreamStatus = SRV_Thread_status_exited;
		return;
	}
	if(stat((const char*)instance->fileHandle->path, &st) < 0)
	{
		UPLOG(vs_file_main, ("file playback: stat file error"));
		instance->videoStreamStatus = SRV_Thread_status_exited;
		return;
	}
	
	if(instance->type == SRV_ID_SZRAWDATA)
	{
		instance->fileHandle->size = (UPUint)st.st_size;
	}
	else if(instance->type == SRV_ID_SZPBONLINE)
	{
		UPLOG(vs_file_main, ("instance->dstFileSize: %d", instance->dstFileSize));
		instance->fileHandle->size = instance->dstFileSize;
	}
	instance->fileHandle->readPos = (((UPUint64)(instance->fileHandle->basePos)*instance->fileHandle->size)/1000);
	UPLOG(vs_file_main, ("base read pos: %ld", instance->fileHandle->readPos));
	fseek(instance->fileHandle->fp, instance->fileHandle->readPos, SEEK_SET);
	instance->fileHandle->frameBufLen = 0;
	instance->netStatus = SRV_NETWORK_NORMAL;
	instance->fileHandle->foundIFrame = 0;
	
	UPLOG(vs_file_main, ("dstFileSize: %d, readBase: %d", instance->dstFileSize, instance->fileHandle->readPos));
	
	while(instance->videoStreamStatus == SRV_Thread_status_running || 
		  instance->videoStreamStatus == SRV_Thread_status_idle)
	{
		if(instance->fileHandle->frameBufLen + 1*1024 > instance->fileHandle->frameBufSize)
		{
			/*handle buffer first*/
			vs_file_BufferHandle(instance);			
			continue;
		}
		
		if(instance->type == SRV_ID_SZPBONLINE)
		{
			if(m_file_readable(instance->fileHandle) == 0)
			{
				usleep(80000);
				continue;
			}
		}
		
		ret = fread(instance->fileHandle->frameBuf + instance->fileHandle->frameBufLen, 1, 1*1024, instance->fileHandle->fp);
		if(ret > 0) /*read 16K*/
		{
			instance->fileHandle->frameBufLen += ret;
			vs_file_BufferHandle(instance);
		}
		
		instance->fileHandle->readPos = ftell(instance->fileHandle->fp);
		
		/*at the end of the file*/
		if(ret < 1*1024)
		{
			UPLOG(vs_file_main, ("check read status"));
			if(feof(instance->fileHandle->fp))
			{
				UPLOG(vs_file_main, ("at the end of the file"));
				usleep(1000000);
			}
		}
		usleep(1000);
	}
	
	instance->fileHandle->readPos = 0;
	
	pthread_mutex_lock((pthread_mutex_t*)instance->videoStreamMutex);
	instance->videoStreamStatus = SRV_Thread_status_exited;
	pthread_mutex_unlock((pthread_mutex_t*)instance->videoStreamMutex);
	UPLOG(vs_file_main, ("videostream thread exited"));
	
	return;
}

