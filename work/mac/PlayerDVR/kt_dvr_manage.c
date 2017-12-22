/*
 *  kt_dvr_manage.c
 *  PlayerDVR
 *
 *  Created by Kelvin on 3/18/11.
 *  Copyright 2011 KangTop. All rights reserved.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "kt_dvr_manage.h"
#include "kt_util.h"
#include "../NetDVRSDK/NetDvrApi.h"
#include "shared/up_types.h"

DvrManager *m_dvrMgr = NULL;
extern UPHandle g_Dvs;


int kt_dvrMgr_init()
{
	m_dvrMgr = dvr_manage_init(10);
	if(m_dvrMgr)
	{
		return 0;
	}
	
	return -1;
}

int kt_dvrMgr_fini()
{
	if(m_dvrMgr)
	{
		dvr_manage_fini(m_dvrMgr);
	}
	
	return 0;
}

int kt_dvrMgr_loadDvrList()
{
	int ret = -1;
	if(m_dvrMgr)
	{
		ret = dvr_manage_initDvrList(m_dvrMgr);
	}
	
	return ret;
}

DvrConfig *kt_dvrMgr_getCurDvr()
{
	DvrConfig *cfg = NULL;
	
	if(m_dvrMgr)
	{
		cfg = dvr_manage_getCurDvr(m_dvrMgr);
	}
	
	return cfg;
}

int kt_dvrMgr_updateCurDvrParam()
{
	return dvr_manage_updateCurDvrParam(m_dvrMgr);
}

systemparameter_t *kt_dvrMgr_getCurDvrParam()
{
	DvrConfig *cfg = NULL;
	
	if(m_dvrMgr == NULL)
	{
		return NULL;
	}
	
	cfg = dvr_manage_getCurDvr(m_dvrMgr);
	
	return cfg->dvrParam;
}

int kt_dvrMgr_setCurDvrParam(systemparameter_t *sysParam)
{
	if(m_dvrMgr == NULL)
	{
		return -1;
	}
	
	return dvr_manage_setCurDvrParam(m_dvrMgr, sysParam);
}

int kt_dvrMgr_updateCurDvr(DvrConfig *cfg)
{
	int ret = -1;
	
	if(m_dvrMgr)
	{
		/*update current DVR*/
		dvr_manage_updateCurDvr(m_dvrMgr, cfg);
	}

	return ret;
}

int kt_dvrMgr_commit()
{
	int ret = -1;
	
	if(m_dvrMgr)
	{
		ret = dvr_manage_commitList(m_dvrMgr);
	}
	
	return ret;
}

DvrManager* dvr_manage_init(unsigned int num)
{
	DvrManager *mgr = (DvrManager*)malloc(sizeof(DvrManager));
	if(mgr == NULL)
	{
		return NULL;
	}
	
	mgr->dvrNum = num;
	mgr->config = (DvrConfig**)malloc(num *sizeof(void*));
	memset(mgr->config, 0x0, num*sizeof(void*));
	
	char *path = getenv("HOME");
	char cfgPath[128];
	memset((void*)cfgPath, 0x0, sizeof(cfgPath));
	
	sprintf(cfgPath, "%s/.PlayerDVR", path);

	if(kt_util_isExist(cfgPath) == 0)
	{
		if(mkdir(cfgPath, 0777))
		{
			perror("/Users/kelvin/.PlayerDVR");
		}
	}
	
	sprintf(cfgPath, "%s/.list.cfg", cfgPath);
	
	memset((void*)mgr->cfgPath, 0x0, sizeof(mgr->cfgPath));
	memcpy((void*)mgr->cfgPath, cfgPath, strlen(cfgPath));
	
	return mgr;
}

void dvr_manage_fini(DvrManager *mgr)
{
	int i = 0;
	if(mgr == NULL)
	{
		return;
	}
	
	for(i = 0; i < mgr->dvrNum; i++)
	{
		if(mgr->config[i])
		{
			if(mgr->config[i]->sysParam)
			{
				free(mgr->config[i]->sysParam);
			}
			free(mgr->config[i]);
			mgr->config[i] = NULL;
		}
	}
	if(mgr->config)
	{
		free(mgr->config);
		mgr->config = NULL;
	}
	
	free(mgr);
	mgr = NULL;
}

//0: not section line; 1: section line
static int m_isSectionLine(const char *line)
{
	int flag = 0;
	int i = 0;
	
	if(line)
	{
		if((line[0] == '[') && (strlen(line) >= 3))
		{
			for(i = 2; i < strlen(line); i++)
			{
				if(line[i] == ']')
				{
					flag = 1;
				}
			}
		}
		
		return flag;
	}
	
	return flag;
}

static int m_getSectionName(const char *line, char *value)
{
	int i = 0, j = 0;
	int sectionStart = 0;
	
	if(line)
	{
		for(i = 0; i < strlen(line); i++)
		{
			if(line[i] == '[')
			{
				sectionStart = 1;
				continue;
			}
			if(sectionStart == 1)
			{
				if(line[i] != ']')
				{
					value[j] = line[i];
					j++;
				}
				else
				{
					break;
				}

			}
		}
	}
	
	return 0;
}

//0: not value line; 1: is value line
static int m_isIdValueLine(const char *line)
{
	int i = 0;
	int flag = 0;
	
	if(line)
	{
		for(i = 0; i < strlen(line); i++)
		{
			if((line[i] == '=') && (i != 0))
			{
				flag = 1;
				break;
			}
		}
		return flag;
	}
	
	return flag;
}

static int m_getId(const char *line, char *id)
{
	int i = 0;
	
	if(line)
	{
		for(i = 0; i < strlen(line); i++)
		{
		
			if(line[i] != '=')
			{
				id[i] = line[i];
			}
			else if(line[i] == '=')
			{
				id[i] = '\0';
				break;
			}
		}
	}
	
	return 0;
}

static int m_getIdValue(const char *line, char *value)
{
	int i = 0, j = 0;
	int startFlag = 0;
	
	if(line)
	{
		for(i = 0; i < strlen(line); i++)
		{
			if(line[i] == '=' && i != 0)
			{
				startFlag = 1;
				continue;
			}
			if(startFlag == 1)
			{
				if(line[i] == '\n' || line[i] == '\t')
				{
					value[j] = '\0';
					break;
				}
				value[j] = line[i];
				j++;
			}
		}
	}
	
	return 0;
}

//1: success; 0: fail
static int m_dvrMgr_add(DvrManager *mgr, DvrConfig *cfg)
{
	int i = 0;
	int flag = 0;
	for(i = 0; i < mgr->dvrNum; i++)
	{
		if(mgr->config[i] == NULL)
		{
			mgr->config[i] = cfg;
			flag = 1;
			break;
		}
	}
	
	return flag;
}

int dvr_manage_initDvrList(DvrManager *mgr)
{
	int ret;
	DvrConfig *cfg;
	char newLine[256] = {0};
	char value[128] = {0};
	FILE *cfgFp = NULL;
	
	if(mgr == NULL)
	{
		return -1;
	}

	cfgFp = fopen(mgr->cfgPath, "r");
	if(cfgFp == NULL)
	{
		perror("fopen");
		return -1;
	}
	
	while(fgets(newLine, sizeof(newLine), cfgFp))
	{
		if(m_isSectionLine(newLine))
		{
			m_getSectionName(newLine, value);
			if(strncmp("Device", value, sizeof("Device")) == 0)
			{
				/*new Dvr Device, add to config list*/
				cfg = (DvrConfig*)malloc(sizeof(DvrConfig));
				memset((void*)cfg, 0x0, sizeof(DvrConfig));
				cfg->sysParam = NULL;/*init system parameters*/
                cfg->dvrParam = NULL;/*init dvr parameters*/

                ret = m_dvrMgr_add(mgr, cfg);
				if(ret == 0) /*config list full*/
				{
					free(cfg);
					cfg = NULL;
					break;
				}
			}
		}
		if(m_isIdValueLine(newLine))
		{
			m_getId(newLine, value);
			if(strncmp("IPAddress", value, sizeof("IPAddress")) == 0)
			{
				/*IPAddress*/
				m_getIdValue(newLine, value);
				memcpy(cfg->serverAddr, value, strlen(value));
				
			}
			else if(strncmp("Port", value, sizeof("Port")) == 0)
			{
				/*Port*/
				m_getIdValue(newLine, value);
				cfg->port = atoi(value);
			}
			else if(strncmp("NetType", value, sizeof("NetType")) == 0)
			{
				/*Network Type*/
				m_getIdValue(newLine, value);
				cfg->netType = atoi(value);
				
			}
			else if(strncmp("UserName", value, sizeof("UserName")) == 0)
			{
				/*UserName*/
				m_getIdValue(newLine, value);
				memcpy(cfg->userName, value, strlen(value));
				
			}
			else if(strncmp("Password", value, sizeof("Password")) == 0)
			{
				/*password*/
				m_getIdValue(newLine, value);
				memcpy(cfg->password, value, strlen(value));
				
			}
			else if(strncmp("Language", value, sizeof("Language")) == 0)
			{
				/*language*/
				m_getIdValue(newLine, value);
				cfg->language = atoi(value);
			}
		}
	}
	fclose(cfgFp);
	return 0;
}

DvrConfig *dvr_manage_getCurDvr(DvrManager *mgr)
{
	int i = 0;
	
	for(i = 0; i < mgr->dvrNum; i++)
	{
		if(mgr->config[i])
		{
			continue;
		}
		else
		{
			if(i != 0)
			{
				return mgr->config[i - 1];
			}
			else
			{
				return NULL;
			}

		}

	}

	return NULL;
}

int dvr_manage_updateCurDvrParam(DvrManager *mgr)
{
	int ret = 0;
	DvrConfig *cfgCur = NULL;
	if(mgr == NULL)
	{
		printf("not available instance\n");
		return -1;
	}
	cfgCur = dvr_manage_getCurDvr(mgr);
	if(cfgCur == NULL)
	{
		printf("not available current dvr config\n");
		return -1;
	}
	
	if(cfgCur->serverAddr)
	{
		if(cfgCur->sysParam == NULL)
		{
			cfgCur->sysParam = (systemparameter_t*)malloc(sizeof(systemparameter_t));
			if(cfgCur == NULL)
			{
				printf("not enought memory, error\n");
				return -1;
			}
			memset((void*)cfgCur->sysParam, 0x0, sizeof(systemparameter_t));			
		}
        
        if(cfgCur->dvrParam == NULL)
		{
			cfgCur->dvrParam = (dvrparameter_t*)malloc(sizeof(dvrparameter_t));
			if(cfgCur == NULL)
			{
				printf("not enought memory, error\n");
				return -1;
			}
			memset((void*)cfgCur->dvrParam, 0x0, sizeof(dvrparameter_t));			
		}
	}

	
	//ret = vs_vms_getParam(cfgCur->sysParam, cfgCur->serverAddr, cfgCur->port);
    //add by wuzy
    //ret = DVS_GetConfig(g_Dvs, SETUP_DISPLAY, &g_dvxVGA, sizeof(g_dvxVGA));    
    ret = DVS_GetConfig(g_Dvs, SETUP_DISPLAY, &(cfgCur->dvrParam->dvxVGA), sizeof(DVS_VGASETUP));    
	if(ret != 0)
	{
		printf("get SETUP_DISPLAY parameters error \n");
		return -1;
	}
    
    ret = DVS_GetConfig(g_Dvs, SETUP_EVENT, &(cfgCur->dvrParam->dvxEvent), sizeof(DVS_EVENTSETUP));    
	if(ret != 0)
	{
		printf("get SETUP_EVENT parameters error \n");
		return -1;
	}
    
    ret = DVS_GetConfig(g_Dvs, SETUP_SYS, &(cfgCur->dvrParam->dvxSYS), sizeof(DVS_SYSSETUP));    
	if(ret != 0)
	{
		printf("get SETUP_SYS parameters error \n");
		return -1;
	}
    
    ret = DVS_GetConfig(g_Dvs, SETUP_NET, cfgCur->dvrParam->dvxNet, sizeof(DVS_NETSETUP_VER4));    
	if(ret != 0)
	{
		printf("get SETUP_NET parameters error \n");
		return -1;
	}
    
    ret = DVS_GetConfig(g_Dvs, SETUP_RECORD, &(cfgCur->dvrParam->dvxREC), sizeof(DVS_RECSETUP));    
	if(ret != 0)
	{
		printf("get SETUP_REC parameters error \n");
		return -1;
	}
    
    ret = DVS_GetConfig(g_Dvs, SETUP_CAMERA, &(cfgCur->dvrParam->dvxCamera), sizeof(DVS_CAMERASETUP));    
	if(ret != 0)
	{
		printf("get SETUP_CAMERA parameters error \n");
		return -1;
	}


	return 0;
}

int dvr_manage_setCurDvrParam(DvrManager *mgr, systemparameter_t *sysParm)
{
	int ret = 0;
	DvrConfig *cfgCur = NULL;
	if(mgr == NULL)
	{
		printf("not available instance \n");
		return -1;
	}
	cfgCur = dvr_manage_getCurDvr(mgr);
	if(cfgCur)
	{
		/*set parameters*/
		//ret = vs_vms_setSystemParam(sysParm, cfgCur->serverAddr, cfgCur->port);
		if(ret != 0)
		{
			printf("set system parameters error \n");
			return -1;
		}
	}

	return 0;
}

int dvr_manage_updateCurDvr(DvrManager *mgr, DvrConfig *cfg)
{
	DvrConfig *cfgCur = NULL;
	
	cfgCur = dvr_manage_getCurDvr(mgr);
	if(cfgCur == NULL)
	{
		/*save this dvr config*/
		cfgCur = (DvrConfig*)malloc(sizeof(DvrConfig));
		memcpy((void*)cfgCur, cfg, sizeof(DvrConfig));
		m_dvrMgr_add(mgr, cfgCur);
	}
	else
	{
		if(strncmp((char*)cfgCur->serverAddr, (char*)cfg->serverAddr, strlen((char*)cfgCur->serverAddr)) != 0)
		{
			memset((void*)cfgCur->serverAddr, 0x0, sizeof(cfgCur->serverAddr));
			memcpy((void*)cfgCur->serverAddr, cfg->serverAddr, strlen((char*)cfg->serverAddr));
		}
#if 0
		if(strncmp((char*)cfgCur->password, (char*)cfg->password, strlen((char*)cfg->password)) != 0)
		{
			memset((void*)cfgCur->password, 0x0, sizeof(cfgCur->password));
			memcpy((void*)cfgCur->password, cfg->password, strlen((char*)cfg->password));
		}
#endif
		
		memset((void*)cfgCur->password, 0x0, sizeof(cfgCur->password));
		memcpy((void*)cfgCur->password, cfg->password, strlen((char*)cfg->password));
		
		cfgCur->port = cfg->port;
		cfgCur->netType = cfg->netType;
		cfgCur->language = cfg->language;
		cfgCur->userType = cfg->userType;
		cfgCur->channelNum = cfg->channelNum;
	}

	return 0;
}

static int m_dvrMgr_commitCfg(FILE *fp, DvrConfig *cfg)
{
	char newLine[128];
	if(fp == NULL)
	{
		return -1;
	}
	
	sprintf(newLine, "[Device]\n");
	fwrite(newLine, 1, strlen(newLine), fp);
	sprintf(newLine, "IPAddress=%s\n", cfg->serverAddr);
	printf("newLine: %s \n", newLine);
	fwrite(newLine, 1, strlen(newLine), fp);
	sprintf(newLine, "Port=%d\n", cfg->port);
	fwrite(newLine, 1, strlen(newLine), fp);
	sprintf(newLine, "NetType=%d\n", cfg->netType);
	fwrite(newLine, 1, strlen(newLine), fp);
	sprintf(newLine, "UserName=%s\n", cfg->userName);
	fwrite(newLine, 1, strlen(newLine), fp);
	sprintf(newLine, "Password=%s\n", cfg->password);
	fwrite(newLine, 1, strlen(newLine), fp);
	sprintf(newLine, "Language=%d\n", cfg->language);
	fwrite(newLine, 1, strlen(newLine), fp);
	
	return 0;
}

int dvr_manage_commitList(DvrManager *mgr)
{
	int i;
	FILE *cfgFp = NULL;
	if(mgr == NULL)
	{
		return -1;
	}

	cfgFp = fopen(mgr->cfgPath, "w+");
	if(cfgFp == NULL)
	{
		perror("fopen");
		return -1;
	}
	
	for(i = 0; i < mgr->dvrNum; i++)
	{
		if(mgr->config[i] != NULL)
		{
			m_dvrMgr_commitCfg(cfgFp, mgr->config[i]);
		}
	}
	
	fclose(cfgFp);
	
	return 0;
}