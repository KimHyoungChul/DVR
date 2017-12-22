/*
 *  kt_util.c
 *  PlayerDVR
 *
 *  Created by Kelvin on 3/18/11.
 *  Copyright 2011 KangTop. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>


#include "kt_util.h"

struct tm *kt_util_getLocalTime()
{
	time_t timep;
	struct tm *p;
	
	time(&timep);
	p = gmtime(&timep);

	return p;
}

//0: not dir 1: dir
int kt_util_isDir(char *path)
{
	int ret;
	struct stat st;
	
	ret = lstat(path, &st);
	if(ret == 0)
	{
		if(S_ISDIR(st.st_mode))
		{
			printf("dir\n");
			ret = 1;
		}
		else
		{
			printf("file \n");
			ret = 0;
		}
		
	}
	else
	{
		printf("file not exist \n");
		ret = 0;
	}
	
	return ret;
}

//0: not exist 1: exist
int kt_util_isExist(char *path)
{
	int ret;
	struct stat st;
	
	ret = lstat(path, &st);
	if(ret == 0)
	{
		printf("file exist \n");
		ret = 1;
	}
	else {
		printf("file not exist \n");
		ret = 0;
	}

	return ret;
}

int kt_util_bitValue(unsigned char *buffer, int pos)
{
	int idx;
	int offset;
	unsigned char ctmp;
	
	idx = pos/8;
	offset = pos%8;
	
	ctmp = buffer[idx];
	
	return (ctmp & (0x01<<offset));
}

void kt_util_bitXOR(unsigned char *buffer, int pos)
{
	int idx;
	int offset;
	
	idx = pos/8;
	offset = pos%8;
	
	if(kt_util_bitValue(buffer, pos))
	{
		buffer[idx] = buffer[idx]&(~((unsigned char)(0x01<<offset)));
	}
	else
	{
		buffer[idx] = buffer[idx]|((unsigned char)(0x01<<offset));
	}
	
}

void kt_util_remove(const char *path)
{
	int ret = 0;
	
	if(path == NULL)
	{
		printf("not invailable parameters \n");
		return ;
	}
	
	if(kt_util_isExist((char*)path) == 1)
	{
		ret = remove(path);
		if(ret == 0)
		{
			printf("remove success\n");
		}
		return ;
	}
	else
	{
		printf("file not exist\n");
	}
	
	return ;
}