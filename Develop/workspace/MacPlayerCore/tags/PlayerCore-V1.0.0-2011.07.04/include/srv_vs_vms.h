/*
 *  srv_vs_vms.h
 *  PlayerCore
 *
 *  Created by Kelvin on 2/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __SRV_VS_VMS_HEADER
#define __SRV_VS_VMS_HEADER

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
	
#define KT_LOGIN_FAILED 0x04

UPSint vs_vms_login(const UPSint8* addr, UPSint32 port, const UPUint8* userName, const UPUint8* password);
UPSint vs_vms_setParam(void *pSystem, UPSint length, UPSint8* addr, UPSint32 port);
UPSint vs_vms_setSystemParam(systemparameter_t *pSystem, UPSint8* addr, UPSint32 port);	
UPSint vs_vms_getParam(systemparameter_t *pSystem, UPSint8* addr, UPSint32 port);
UPSint vs_vms_getSystemParam(systemparameter_t *pSystem, UPSint8* addr, UPSint32 port);
UPSint vs_vms_getCalenda(UPSint8* addr, UPSint32 port, UPUint ch, UPUint8 year, UPUint8 month, UPUint8 day, UPUint8 type, UPUint *calendar);	
UPSint vs_vms_searchFile(UPSint8* addr, UPSint32 port, UPUint ch, UPUint8 year, UPUint8 month, UPUint8 day, UPSint type, UPSint8 **fileList, UPUint *fileNum);	
UPSint vs_vms_searchGetNum(UPSint8* addr, UPSint32 port, UPUint ch, UPUint8 year, 
						UPUint8 month, UPUint8 day, UPSint type, UPUint *fileNum);
UPSint vs_vms_searchGetPage(UPSint8* addr, UPSint32 port, UPUint ch, UPUint8 year, 
						UPUint8 month, UPUint8 day, UPSint type, UPUint page, UPSint8 **fileList, UPUint *fileNum);
UPSint vs_vms_getChnNumber(UPSint8* addr, UPSint32 port);
UPSint vs_vms_ptzControl(UPSint8* addr, UPSint32 port, UPUint ch, PTZControlAction action, UPUint param);	
void vs_vms_main(void* data);
void vs_vms_downloadMain(void* data);
UPSint vs_vms_downloadStop(UPSint8 *addr, UPSint port);
	
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__SRV_VS_VMS_HEADER*/
