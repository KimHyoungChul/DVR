/*
 *  kt_dvr_manage.h
 *  PlayerDVR
 *
 *  Created by Kelvin on 3/18/11.
 *  Copyright 2011 KangTop. All rights reserved.
 *
 */
#ifndef _KT_DVR_MANAGE_HEADER
#define _KT_DVR_MANAGE_HEADER

#include "srv_vs_vmstcp.h"
#include "srv_vs_vmsdvr.h"
#include "../GlobalDef.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

typedef struct _dvrparamter_t{
    DVS_VGASETUP    dvxVGA;
    DVS_RECSETUP    dvxREC;
    DVS_EVENTSETUP  dvxEvent;
    DVS_SYSSETUP    dvxSYS;
    DVS_USERSETUP   dvxUser;
    DVS_CAMERASETUP dvxCamera;
    char            dvxNet[sizeof(DVS_NETSETUP_VER4)];
}dvrparameter_t;
    
typedef struct _DvrConfig
{
	unsigned char dvrName[64];
	unsigned char userName[32];
	unsigned char password[32];
	unsigned char serverAddr[128];
	unsigned int port;
	unsigned int rememberFlag; /*1: on; 0: off*/
	unsigned int netType;	/*0: internet; 1: lan*/
	unsigned int language;	/*0: english; 1: chinese simple*/
	unsigned int channelNum;
	unsigned int userType;	/*0: admin; 1: normal user*/
	
	systemparameter_t *sysParam;	/*dvr parameters*/
    
    //add by wuzy
    dvrparameter_t  *dvrParam;

	
	struct _DvrConfig *next;
}DvrConfig;
	
typedef struct _DvrManagement
{
	unsigned int dvrNum;
	DvrConfig **config;
	
	DvrConfig *curCfg;
	
	char cfgPath[128];
}DvrManager;
	
int kt_dvrMgr_init();
int kt_dvrMgr_fini();
int kt_dvrMgr_loadDvrList();
DvrConfig *kt_dvrMgr_getCurDvr();
int kt_dvrMgr_updateCurDvrParam();
systemparameter_t *kt_dvrMgr_getCurDvrParam();
int kt_dvrMgr_setCurDvrParam(systemparameter_t *sysParam);
int kt_dvrMgr_updateCurDvr(DvrConfig *cfg);
int kt_dvrMgr_commit();

DvrManager* dvr_manage_init(unsigned int num);
void dvr_manage_fini(DvrManager *mgr);
int dvr_manage_initDvrList(DvrManager *mgr);
DvrConfig *dvr_manage_getCurDvr(DvrManager *mgr);
int dvr_manage_updateCurDvrParam(DvrManager *mgr);
int dvr_manage_setCurDvrParam(DvrManager *mgr, systemparameter_t *sysParm);
int dvr_manage_updateCurDvr(DvrManager *mgr, DvrConfig *cfg);
int dvr_manage_commitList(DvrManager *mgr);	
	
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_KT_DVR_MANAGE_HEADER*/

