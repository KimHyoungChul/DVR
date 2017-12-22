/*
 *  kt_util.h
 *  PlayerDVR
 *
 *  Created by Kelvin on 3/18/11.
 *  Copyright 2011 KangTop. All rights reserved.
 *
 */

#ifndef _KT_UTIL_HEADER
#define _KT_UTIL_HEADER

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct tm *kt_util_getLocalTime();
int kt_util_isDir(char *path);
int kt_util_isExist(char *path);
	
int kt_util_bitValue(unsigned char *buffer, int pos);	
void kt_util_bitXOR(unsigned char *buffer, int pos);
	
void kt_util_remove(const char *path);
	
int kt_util_basename(const char *pathName, int len, char *name);
	
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_KT_UTIL_HEADER*/