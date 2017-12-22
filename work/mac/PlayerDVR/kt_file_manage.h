/*
 *  kt_file_manage.h
 *  DVRPlayer
 *
 *  Created by Kelvin on 3/31/11.
 *  Copyright 2011 KangTop. All rights reserved.
 *
 */
#ifndef _KT_FILE_MANAGE_HEADER
#define _KT_FILE_MANAGE_HEADER

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
	
typedef struct _RecFile
{
	char name[64];
	char path[256];
	int channel;
	int type;
	char size[16];
	char startTime[64];
	struct _RecFile *next;
}RecFile;
	
typedef struct _RecFileManage
{
	RecFile *head;
	RecFile *tail;
	
	int count;
}RecFileManage;
	
RecFile *kt_fileMgr_convertLocalFile(const char *pathName, char *extend, int standard);
RecFile *kt_fileMgr_getRecFileInFS(char *path, char *extend, int channel, int standard, int *conut);
RecFile *kt_fileMgr_getRecFileOnline(void *instance, int channel, int type);
RecFile *kt_fileMgr_getHistoryFileOnline(void *instance, char *path);
void kt_fileMgr_dump(RecFile *list);
void kt_fileMgr_listFini(RecFile *list);
	
/*parser hi3515 record file name*/
int kt_fileMgr_getFileName(const char *path, char *name);
int kt_fileMgr_getFileStartTime(const char *fileName, char *startTime);
	
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_KT_FILE_MANAGE_HEADER*/