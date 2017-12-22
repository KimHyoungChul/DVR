/*
 *  kt_file_manage.c
 *  DVRPlayer
 *
 *  Created by Kelvin on 3/31/11.
 *  Copyright 2011 KangTop. All rights reserved.
 *
 */

#include "kt_file_manage.h"
#include "kt_stream_playback.h"
#include "kt_util.h"
#include "../GlobalDef.h"


static int m_find_dot(char *str)
{
	int i;
	int ret = -1;
	int len = strlen(str);
	
	for(i = 0; i < len; i++)
	{
		if(str[i] == '.')
		{
			ret = i;
			break;
		}
	}
	
	return ret;
}

static int m_get_channelNum(char *name)
{
	int i = 0;
	int j = 0;
	int found = 0;
	int ret = -1;
	int len = strlen(name);
	char channel[32] = {0};
	
	for(i = 0; i < len; i++)
	{
		if(name[i] == 'c' && name[i+1] == 'h')	
		{
			//printf("found channel key word \n");
			channel[0] = name[i+2];
			channel[1] = name[i+3];
			//ret = 0;
			found = 1;
			i++;
			continue;
			//break;
		}
		if(found == 1 && name[i] != '-')
		{
			channel[j] = name[i];
			j++;
		}
		if(name[i] == '-')
		{
			ret = 0;
			break;
		}
	}
	
	if(ret == 0)
	{
		printf("channel str: %s\n", channel);
		ret = atoi(channel);
	}
	
	return ret;
}

static unsigned long m_get_fileSize(char *path, char *name, char *strDes)
{
	int ret = 0;
	unsigned long size;
	unsigned int sizeM;
	unsigned int sizeK;
	struct stat st;
	char filename[256] = {0};
	
	sprintf(filename, "%s%s", path, name);
	if(stat(filename, &st) < 0)
	{
		return 0;
	}
	
	size = (unsigned long)st.st_size;
	sizeM = size/1024/1024;
	if(sizeM == 0)
	{
		sizeK = size/1024;
		sprintf(strDes, "%dK", sizeK);
	}
	else {
		sprintf(strDes, "%dM", sizeM);
	}

	ret = st.st_size;
	return ret;
}

static unsigned long m_get_fileSize2(char *pathName, char *strDes)
{
	int ret = 0;
	unsigned long size;
	unsigned int sizeM;
	unsigned int sizeK;
	struct stat st;
	
	if(stat(pathName, &st) < 0)
	{
		return 0;
	}
	
	size = (unsigned long)st.st_size;
	sizeM = size/1024/1024;
	if(sizeM == 0)
	{
		sizeK = size/1024;
		sprintf(strDes, "%dK", sizeK);
	}
	else {
		sprintf(strDes, "%dM", sizeM);
	}
	
	ret = st.st_size;
	return ret;
}

static int m_get_startTime(char *name, char *startTime)
{
	int i = 0, j = 0, len = 0;
	int found = 0;
//	char base[3] = {0};
//	int hour = 0, minute = 0, second = 0;
	
	if(name == NULL || startTime == NULL)
	{
		printf("parameters invailable \n");
		return -1;
	}
	
	len = strlen(name);
	
	for(i = 0; i < len; i++)
	{
		if(name[i] == '-' && found == 0)
		{
			found = 1;
			j = 0;
			continue;
		}
		
		if(found == 1 && name[i] != '.')
		{
			startTime[j] = name[i];
			j++;
		}
		else if(name[i] == '.')
		{
			break;
		}

	}
	
//	printf("local file startTime: %s \n", startTime);
//	memcpy(base, startTime + 9, 2 );
//	printf("hour: %s \n", base);
//	hour = atoi(base);
//	memcpy(base, startTime + 11, 2);
//	printf("minute: %s\n", base);
//	minute = atoi(base);
//	memcpy(base, startTime + 13, 2);
//	printf("second: %s \n", base);
//	second = atoi(base);
//	printf("%d:%d:%d\n", hour, minute, second);
//	sprintf(startTime, "%02d:%02d:%02d", hour, minute, second);
//	return 0;
	
	if(j != 0)
	{
		return 1;
	}

	return 0;
}

/*just for test*/
static int m_get_fileList(char *path, char *extend)
{
	int extPos;
	DIR *pDir;
	struct dirent *pdirEnt;
	char *tmp = NULL;
	
	if(path == NULL || extend == NULL)
	{
		printf("parameters not available \n");
		return -1;
	}
	
	if((pDir = opendir(path)) == NULL)
	{
		printf("open %s dir error \n", path);
		return -1;
	}
	
	while((pdirEnt = readdir(pDir)) != NULL)
	{
		/*check special dir*/
		if(strcmp(pdirEnt->d_name, ".") == 0 || strcmp(pdirEnt->d_name, "..") == 0)
		{
			printf("\".\" or \"..\" \n");
			continue;
		}
		if(pdirEnt->d_type == DT_DIR)
		{
			printf("this is a dir");
			continue;
		}
		extPos = m_find_dot(pdirEnt->d_name);
		if(extPos == -1)
		{
			printf("not the type we need \n");
			continue;
		}
		
		tmp = pdirEnt->d_name + extPos;
		//printf("d_name: %s, tmp: %s \n", pdirEnt->d_name, tmp);
		if(strncmp(tmp, extend, strlen(extend)) == 0)
		{
			printf("file name: %s \n", pdirEnt->d_name);
			printf("channel: %d \n", m_get_channelNum(pdirEnt->d_name));
			m_get_fileSize(path, pdirEnt->d_name, NULL);
		}
		else
		{
			printf("not the type we need 2\n");
			continue;
		}
	}
	
	closedir(pDir);
	
	return 0;
}

static void m_set_RecFile(char *name, RecFile *rec)
{
	rec->channel = m_get_channelNum(name);
	//printf("rec->channel: %d \n", rec->channel);
	strcpy(rec->name, name);
	//printf("rec->name: %s \n", rec->name);
}

//ch04-20110701-143128.264
//ch00000000000001-110620-091512-091512-02p001000000.264
RecFile *kt_fileMgr_convertLocalFile(const char *pathName, char *extend, int standard)
{
	int extPos = 0;
	char baseName[128] = {0};
	char *tmpStr = NULL;
	char startTime[64] = {0};
	
	RecFile *recFile = (RecFile*)malloc(sizeof(RecFile));
	if(recFile == NULL)
	{
		printf("%s#%d::not enought memory \n", __FILE__, __LINE__);
		return NULL;
	}
	memset((void*)recFile, 0x0, sizeof(RecFile));
	if(pathName == NULL || extend == NULL)
	{
		printf("parameters not available\n");
		free(recFile);
		recFile = NULL;
		return recFile;
	}
	
	kt_util_basename((char*)pathName, (int)strlen(pathName), baseName);
	extPos = m_find_dot(baseName);
	if(extPos == -1)
	{
		printf("not the type we need \n");
		free(recFile);
		recFile = NULL;
		return recFile;
	}
	
	tmpStr = baseName + extPos;
	if(strncmp(tmpStr, extend, strlen(extend)) != 0)
	{
		printf("not this type");
		free(recFile);
		recFile = NULL;
		return recFile;
	}
	
	if(baseName[4] == '-' && baseName[13] == '-')
	{
		standard = 0;
	}
	else if(baseName[16] == '-' && baseName[23] == '-' && baseName[30] == '-')
	{
		standard = 1;
	}
	else
	{
		free(recFile);
		recFile = NULL;
		return recFile;
	}

	
	m_set_RecFile(baseName, recFile);
	strcpy(recFile->path, pathName);
	m_get_fileSize2((char*)pathName, recFile->size);
	recFile->next = NULL;
	memset(startTime, 0x0, sizeof(startTime));
	if(standard == 0)
	{
		/*local file*/
		m_get_startTime(baseName, startTime);
	}
	else if(standard == 1)
	{
		/*remote file*/
		kt_fileMgr_getFileStartTime(baseName, startTime);
	}
	strcpy(recFile->startTime, startTime);
	
	return recFile;
}

RecFile *kt_fileMgr_getRecFileInFS(char *path, char *extend, int channel, int standard, int *conut)
{
	int extPos;
	DIR *pDir;
	struct dirent *pdirEnt;	
	int count = 0;
	RecFile *head = NULL;
	RecFile *tmp = NULL;
	char filePath[256] = {0};
	char *tmpStr;
	char startTime[64] = {0};
	
	head = (RecFile*)malloc(sizeof(RecFile));
	if(head == NULL)
	{
		printf("%s#%d::not enought memory \n", __FILE__, __LINE__);
		return NULL;
	}
	memset((void*)head, 0x0, sizeof(RecFile));
	
	if(path == NULL || extend == NULL)
	{
		printf("parameters not available \n");
		free(head);
		return NULL;
	}
	
	if((pDir = opendir(path)) == NULL)
	{
		printf("open %s dir error \n", path);
		free(head);
		return NULL;
	}
	
	while((pdirEnt = readdir(pDir)) != NULL)
	{
		/*check special dir*/
		if(strcmp(pdirEnt->d_name, ".") == 0 || strcmp(pdirEnt->d_name, "..") == 0)
		{
			printf("\".\" or \"..\" \n");
			continue;
		}
		if(pdirEnt->d_type == DT_DIR)
		{
			printf("this is a dir");
			continue;
		}
		extPos = m_find_dot(pdirEnt->d_name);
		if(extPos == -1)
		{
			printf("not the type we need \n");
			continue;
		}
		tmpStr = pdirEnt->d_name + extPos;
		if(strncmp(tmpStr, extend, strlen(extend)) != 0)
		{
			continue;
		}
		
		sprintf(filePath, "%s%s", path, pdirEnt->d_name);
		if(count == 0)
		{
			m_set_RecFile(pdirEnt->d_name, head);
			strcpy(head->path, filePath);
			m_get_fileSize(path, pdirEnt->d_name, head->size);
			tmp = head;
			tmp->next = NULL;
			memset(startTime, 0x0, sizeof(startTime));
			if(standard == 0)
			{
				/*local file*/
				m_get_startTime(pdirEnt->d_name, startTime);
			}
			else if(standard == 1)
			{
				/*remote file*/
				kt_fileMgr_getFileStartTime(pdirEnt->d_name, startTime);
			}
			//printf("startTime: %s \n", startTime);
			strcpy(head->startTime, startTime);
			count++;
		}
		else
		{
			tmp->next = (RecFile*)malloc(sizeof(RecFile));
			memset((void*)tmp->next, 0x0, sizeof(RecFile));
			tmp = tmp->next;
			m_set_RecFile(pdirEnt->d_name, tmp);
			strcpy(tmp->path, filePath);
			m_get_fileSize(path, pdirEnt->d_name, tmp->size);
			tmp->next = NULL;
			memset(startTime, 0x0, sizeof(startTime));
			if(standard == 0)
			{
				/*local file*/
				m_get_startTime(pdirEnt->d_name, startTime);
			}
			else if(standard == 1)
			{
				/*remote file*/
				kt_fileMgr_getFileStartTime(pdirEnt->d_name, startTime);
			}
			//printf("startTime: %s \n", startTime);
			strcpy(tmp->startTime, startTime);
			count++;
		}

	}
	
	if(count == 0)
	{
		free(head);
		return NULL;
	}
	
	return head;
}

static void m_get_convertSize(int fileSize, char *strDes)
{
	int sizeM = 0, sizeK = 0;
	
	if(strDes == NULL)
	{
		return;
	}
	
	if(fileSize < 0)
	{
		sprintf(strDes, "%dk", sizeK);
		return ;
	}
	sizeM = fileSize/1024/1024;
	if(sizeM == 0)
	{
		sizeK = fileSize/1024;
		sprintf(strDes, "%dK", sizeK);
	}
	else {
		sprintf(strDes, "%dM", sizeM);
	}
	
	return;
}

static void m_get_convertChannel(int channel, int *dstCh)
{
	int i = 0;
	
	if(dstCh == NULL)
	{
		return;
	}
	
	for(i = 0; i < 16; i++)
	{
		if(channel & (int)(0x01<<i))
		{
			*dstCh = i + 1;
			break;
		}
	}
}

RecFile *kt_fileMgr_getHistoryFileOnline(void *instance, char *path)
{
	int count = 0;
	Playback_Online *online = (Playback_Online*)instance;
	if(online == NULL)
	{
		printf("----------adfaga.>>>>%s#%d::not available instance \n", __FILE__, __LINE__);
		return NULL;
	}
	
	if(online->cacheFileList)
	{
		/*fini*/
		kt_fileMgr_listFini(online->cacheFileList);
	}
	
	online->cacheFileList = kt_fileMgr_getRecFileInFS(path, ".h264", 0xff, 1, &count);
	
	return online->cacheFileList;
}

RecFile *kt_fileMgr_getRecFileOnline(void *instance, int channel, int type)
{
    int i = 0;
	int count = 0;
	RecFile *head = NULL;
	RecFile *tmp = NULL;
     
	DVS_RECORDFILE_LIST *online = (DVS_RECORDFILE_LIST*)instance;
    
    if(online == NULL)
        return NULL;
    
    head = (RecFile*)malloc(sizeof(RecFile));
	if(head == NULL)
	{
		printf("%s#%d::not enought memory \n", __FILE__, __LINE__);
		return NULL;
	}
	memset((void*)head, 0x0, sizeof(RecFile));

    char size[16] = "<1M";
    while(i < online->dwRecordfileCount)
	{
        if(count == 0)
        {
            tmp = head;
            tmp->next = NULL;
            strcpy(tmp->path, online->pRecordfileList->sFileName);
            sprintf(tmp->startTime, "%02d:%02d:%02d-%02d:%02d:%02d", 
                    online->pRecordfileList[i].dtStartTime.dwHour, 
                    online->pRecordfileList[i].dtStartTime.dwMinute, 
                    online->pRecordfileList[i].dtStartTime.dwSecond, 
                    online->pRecordfileList[i].dtEndTime.dwHour, 
                    online->pRecordfileList[i].dtEndTime.dwMinute, 
                    online->pRecordfileList[i].dtEndTime.dwSecond);	
            
            tmp->channel = 1;
            if(online->pRecordfileList[i].dwFileSize == 0)
                memcpy(tmp->size, size, sizeof(tmp->size));
            else 
                sprintf(tmp->size, "%dM", online->pRecordfileList[i].dwFileSize);                     
                        
          
            tmp->type = online->pRecordfileList[i].type;
            count++;
        }
        else
        {
            tmp->next = (RecFile*)malloc(sizeof(RecFile));
            tmp = tmp->next;
            tmp->next = NULL;
            strcpy(tmp->path, online->pRecordfileList->sFileName);
            sprintf(tmp->startTime, "%02d:%02d:%02d-%02d:%02d:%02d", 
                    online->pRecordfileList[i].dtStartTime.dwHour, 
                    online->pRecordfileList[i].dtStartTime.dwMinute, 
                    online->pRecordfileList[i].dtStartTime.dwSecond, 
                    online->pRecordfileList[i].dtEndTime.dwHour, 
                    online->pRecordfileList[i].dtEndTime.dwMinute, 
                    online->pRecordfileList[i].dtEndTime.dwSecond);	
            
            tmp->channel = 1;
            if(online->pRecordfileList[i].dwFileSize == 0)
                memcpy(tmp->size, size, sizeof(tmp->size));
            else 
                sprintf(tmp->size, "%dM", online->pRecordfileList[i].dwFileSize);     
            tmp->type = online->pRecordfileList[i].type;
            count++;
        }

        i++;
	}
	
	if(count == 0)
	{
		free(head);
		head = NULL;
		return head;
	}
    
    /*
	int i = 0;
	int count = 0;
	RecFile *head = NULL;
	RecFile *tmp = NULL;
	Playback_Online *online = (Playback_Online*)instance;       
	Search_FileInfo *fileInfo = NULL;
	
	if(online->searchMgr == NULL)
	{
		return NULL;
	}
	
	head = (RecFile*)malloc(sizeof(RecFile));
	if(head == NULL)
	{
		printf("%s#%d::not enought memory \n", __FILE__, __LINE__);
		return NULL;
	}
	memset((void*)head, 0x0, sizeof(RecFile));
	
	//channel = 0xffff;
	
	while(i < online->searchMgr->fileNum)
	{
		fileInfo = online->searchMgr->list[i];
		//printf("channel: %04x, fileInfo->channel: %04x\n", channel, fileInfo->channel);
		if((channel & fileInfo->channel))
		{
			if(type == 3 || fileInfo->rec_type == type)
			{
				if(count == 0)
				{
					tmp = head;
					tmp->next = NULL;
					strcpy(tmp->path, fileInfo->file_name);
					sprintf(tmp->startTime, "%02d:%02d:%02d-%02d:%02d:%02d", fileInfo->start_hour, fileInfo->start_min, fileInfo->start_sec, fileInfo->end_hour, fileInfo->end_min, fileInfo->end_sec);
					//sprintf(tmp->startTime, "%04d%02d%02d-%02d%02d%02d", (online->searchMgr->year + 2000), online->searchMgr->month, online->searchMgr->day, fileInfo->start_hour, fileInfo->start_min, fileInfo->start_sec);
					m_get_convertChannel(fileInfo->channel, &(tmp->channel));
					m_get_convertSize(fileInfo->file_size, tmp->size);
					tmp->type = fileInfo->rec_type;
					count++;
				}
				else
				{
					tmp->next = (RecFile*)malloc(sizeof(RecFile));
					tmp = tmp->next;
					tmp->next = NULL;
					strcpy(tmp->path, fileInfo->file_name);
					//sprintf(tmp->startTime, "%04d%02d%02d-%02d%02d%02d", (online->searchMgr->year + 2000), online->searchMgr->month, online->searchMgr->day, fileInfo->start_hour, fileInfo->start_min, fileInfo->start_sec);
					sprintf(tmp->startTime, "%02d:%02d:%02d-%02d:%02d:%02d", fileInfo->start_hour, fileInfo->start_min, fileInfo->start_sec, fileInfo->end_hour, fileInfo->end_min, fileInfo->end_sec);
					m_get_convertChannel(fileInfo->channel, &(tmp->channel));
					m_get_convertSize(fileInfo->file_size, tmp->size);
					tmp->type = fileInfo->rec_type;
					count++;
				}
			}
			else
			{
				printf("22222222222fileInfo->rec_type: %d\n", fileInfo->rec_type);
			}

		}
		else
		{
//			printf("i: %d--1111111111111fileInfo->channel: %04x\n", i, fileInfo->channel);
//			printf("S %d:%d:%d E %d:%d:%d\n", fileInfo->start_hour, fileInfo->start_min, fileInfo->start_sec, 
//				   fileInfo->end_hour, fileInfo->end_min, fileInfo->end_sec);
		}

		i++;
	}
	
	if(count == 0)
	{
		free(head);
		head = NULL;
		return head;
	}
	*/
	return head;
}

static void m_recfile_dump(RecFile *file)
{
	//printf("path: %s, size: %s, channel: %d, startTime: %s \n", file->path, file->size, file->channel, file->startTime);
}

void kt_fileMgr_dump(RecFile *list)
{
	RecFile *file = NULL;
	
	file = list;
	
	while(file)
	{
		m_recfile_dump(file);
		file = file->next;
	}
	
	return;
}

void kt_fileMgr_listFini(RecFile *list)
{
	RecFile *file = NULL;
	RecFile *tmpFile = NULL;
	
	file = list;
	
	while(file)
	{
		tmpFile = file->next;
		free(file);
		file = tmpFile;
	}
	
}

/*name: /stm/disk/0/p1/2011-04-18/ch00000000000007-110418-000000-000000-02p007000000.nvr parser*/
int kt_fileMgr_getFileName(const char *path, char *name)
{
	int i = 0;
	int j = 0;
	int len = strlen(path);
	int found = 0;
	
	if(path == NULL)
	{
		return -1;
	}
	
	for(i = 0; i < len; i++)
	{
		if((path[i] == 'c') && (path[i+1] == 'h') && (found == 0)) /*find ch flag*/
		{
			found = 1;
			j = 0;
		}
		
		if(found == 1 && path[i] != '.')
		{
			name[j] = path[i];
			j++;
		}
		else if(found == 1 && path[i] == '.')
		{
			found = 2;
			break;
		}
	}
	
	if(found == 2)
	{
		printf("------------->name: %s \n", name);
		return 0;
	}
	
	printf("can't get name information \n");
	return -1;
}

int kt_fileMgr_getFileStartTime(const char *fileName, char *startTime)
{
	char tmp[64] = {0};
//	char base[2] = {0};
//	int hour = 0, minute = 0, second = 0;
	
	if(fileName == NULL)
	{
		return -1;
	}
	
	{
		memcpy(tmp, fileName+17, 13);
		sprintf(startTime, "20%s", tmp);
		printf("startTime: %s \n", startTime);
//		/*startTime: 20110603-085041*/
//		memcpy(base, startTime + 9, 2 );
//		printf("hour: %s \n", base);
//		hour = atoi(base);
//		memcpy(base, startTime + 11, 2);
//		printf("minute: %s\n", base);
//		minute = atoi(base);
//		memcpy(base, startTime + 13, 2);
//		printf("second: %s \n", base);
//		second = atoi(base);
//		printf("%d:%d:%d\n", hour, minute, second);
//		sprintf(startTime, "%02d:%02d:%02d", hour, minute, second);
		return 0;
	}
	
	return -1;
}