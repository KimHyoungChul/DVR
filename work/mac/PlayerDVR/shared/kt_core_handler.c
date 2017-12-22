#include "kt_stream_playback.h"
#include "kt_core_handler.h"
#include "kt_util.h"

int kt_core_fwInit()
{
	char *homePath = NULL;
	char basePath[128] = {0};
	
	/*create base dir*/
	homePath = getenv("HOME");
	sprintf(basePath, "%s/DvrRecord", homePath);
	
	if(kt_util_isExist(basePath) == 0)
	{
		mkdir((const char*)basePath, 0777);
	}
	
	return 0;//upui_fw_init();
}

int kt_core_fwFini()
{
	return 0;//upui_fw_fini();
}

int kt_core_createDvrHome(char *ipAddr)
{
	char *homePath = NULL;
	char dvrPath[128] = {0};
	char tmpPath[128] = {0};
	char dvrRoot[128] = {0};
	
	/*create root dir*/
	homePath = getenv("HOME");
	sprintf(dvrRoot, "%s/DvrRecord", homePath);
	if(kt_util_isExist(dvrRoot) == 0)
	{
		mkdir(dvrRoot, 0777);
	}
	
	/*create dvr base dir*/
	sprintf(dvrPath, "%s/DvrRecord/%s", homePath, ipAddr);
	if(kt_util_isExist(dvrPath) == 0)
	{
		mkdir(dvrPath, 0777);
	}
	
	/*create picture capture dir*/
	memset(tmpPath, 0x0, sizeof(tmpPath));
	sprintf(tmpPath, "%s/capture", dvrPath);
	if(kt_util_isExist(tmpPath) == 0)
	{
		mkdir(tmpPath, 0777);
	}
	
	/*create local record dir*/
	memset(tmpPath, 0x0, sizeof(tmpPath));
	sprintf(tmpPath, "%s/record", dvrPath);
	if(kt_util_isExist(tmpPath) == 0)
	{
		mkdir(tmpPath, 0777);
	}
	
	/*create remote backup dir*/
	memset(tmpPath, 0x0, sizeof(tmpPath));
	sprintf(tmpPath, "%s/backup", dvrPath);
	if(kt_util_isExist(tmpPath) == 0)
	{
		mkdir(tmpPath, 0777);
	}
	
	/*create video cache dir*/
	memset(tmpPath, 0x0, sizeof(tmpPath));
	sprintf(tmpPath, "%s/tmp", dvrPath);
	if(kt_util_isExist(tmpPath) == 0)
	{
		mkdir(tmpPath, 0777);
	}
	
	return 0;
}
