//
//  MAC_NetDvrApi.c
//  DVRPlayer
//
//  Created by sz Kangtop on 5/19/12.
//  Copyright (c) 2012 KangTop. All rights reserved.
//

#include "MAC_NetDvrApi.h"
#include "../NetDVRSDK/NetDvrApi.h"


UPUint MAC_DVS_Init()
{
    return DVS_Init();    
}

void   MAC_DVS_Cleanup()
{
    return DVS_Cleanup();
}

UPHandle MAC_DVS_Login(UPSint8 *addr, UPUint32 port, UPUint8 *userName, UPUint8 *password)
{
    return DVS_Login((char*)addr, (short)port, (char*)userName, (char*)password);
}

UPUint	MAC_DVS_Logout(UPHandle hDvs)
{
    return DVS_Logout(hDvs);
}
                        
UPHandle MAC_DVS_OpenRealStream(UPHandle hDvs, UPSint8 *pchHostIP, UPUint32 wHostPort, UPUint8 channel, UPHandle hRealHandle,Stream_Playback *instance)
{
    return DVS_OpenRealStream(hDvs, pchHostIP, wHostPort, channel, hRealHandle,instance);
}

UPUint	 MAC_DVS_StopRealStream(UPHandle hRealHandle)
{
    return DVS_StopRealStream(hRealHandle);
}

UPUint MAC_DVS_PlayRealStream(UPHandle hRealHandle)
{
    return DVS_PlayRealStream(hRealHandle);
}

UPUint	 MAC_DVS_SaveRealDataStart(UPHandle hRealHandle, UPSint8 *sFileName)
{
    return DVS_SaveRealDataStart(hRealHandle, sFileName);
}

UPUint	 MAC_DVS_SaveRealDataStop(UPHandle hRealHandle)
{
    return DVS_SaveRealDataStop(hRealHandle);
}

UPUint	 MAC_DVS_PTZControl(UPHandle hDvs, UPSint8 channel, UPUint dwPTZCommand, UPUint dwParam)
{
    return DVS_PTZControl(hDvs, channel, dwPTZCommand, dwParam);
}

UPHandle	 MAC_DVS_OpenLocalPlay(UPSint8 *pchLocalFileName, UPUint8 playCounts, UPHandle hPlayHandle)
{
    UPHandle pp = UP_NULL;
    pp = DVS_OpenLocalPlay(pchLocalFileName, playCounts, hPlayHandle);
    return pp;
}