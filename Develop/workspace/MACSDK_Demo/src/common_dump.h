#ifndef _PS_FILE_COMMON_HEADER
#define _PS_FILE_COMMON_HEADER

#include "NetDvrApi.h"

struct BlockInfo
{
	UNS32	recSize[DVS_MAX_SOLO_CHANNEL_16];//每个通道已经保存的数据尺寸，单位是KB；
	UNS32	indexCount;	//本块索引条数
	UNS32	BlockSize;	//本块数据大小，包括数据区和索引去的总大小，包括blockinfo本身；
	UNS32	endMagic;	//数据块结束符 0x0058554E
};

#endif
