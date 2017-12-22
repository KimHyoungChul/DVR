//
//  MAC_NetDvrApi.h
//  DVRPlayer
//
//  Created by sz Kangtop on 5/19/12.
//  Copyright (c) 2012 KangTop. All rights reserved.
//

#ifndef DVRPlayer_MAC_NetDvrApi_h
#define DVRPlayer_MAC_NetDvrApi_h

#include "../GlobalDef.h"
//#include "../up_types.h"
#include "./shared/kt_stream_playback.h"

//#ifdef __cplusplus
//extern "C" {
//#endif /*__cplusplus*/
    /*wuzy-12.4.16*/
    
    //œ˚œ¢ªÿµ˜∫Ø ˝°™°™dwCommand±®æØ¿‡–Õ£¨hDvs--DVS_Login∑µªÿµƒæ‰±˙£¨pBuf-- ˝æ›£¨dwBufLen--pBuf≥§∂»£¨pchDeviceIP--DVRµƒIP£¨wDevicePort--Ω” ’œ˚œ¢µƒ∂Àø⁄£¨dwUser--”√ªß≤Œ ˝
    //typedef void (*fMessCallBack)(UNS32 dwCommand, HANDLE hDvs, S8 *pBuf, UNS32 dwBufLen, S8 *pchDeviceIP, UNS32 wDevicePort, UNS32 dwUser);
    //“Ù ”∆µªÿµ˜∫Ø ˝°™°™type ˝æ›¿‡–Õ∂‘”¶CallBackDataType£¨channel ”∆µÕ®µ¿∫≈ªÚ∞È“ÙÕ®µ¿∫≈£®1--16£©£¨pBuf-- ”∆µ ˝æ›£¨dwBufLen--pBuf≥§∂»£¨dwUser--”√ªß≤Œ ˝
    //typedef void (*fDataCallBack)(UNS32 type, UNS8 channel, S8 *pBuf, UNS32 dwBufLen, UNS32 dwUser);
    
    //SDKø‚µƒ≥ı ºªØ∫Õ◊¢œ˙
    //UNS32	DVS_Init();
    UPUint MAC_DVS_Init();
    void   MAC_DVS_Cleanup();
    //UNS32	DVS_GetLastError();//∑µªÿ¥ÌŒÛ¬Î
    
    //…Ë÷√¡¨Ω”≥¨ ± ±º‰∫Õ¡¨Ω”¥Œ ˝£¨ƒø«∞µ⁄∂˛∏ˆ≤Œ ˝¡¨Ω”¥Œ ˝‘›≤ª π”√£¨ÃÓ0
    //void	DVS_SetConnectTime(UNS32 dwWaitTime, UNS32 dwTryTimes);
    
    //µ«¬º∫Õ◊¢œ˙
    //HANDLE	DVS_Login(S8 *pchDeviceIP, UNS16 wDevicePort, S8 *pchUserName, S8 *pchPwd);
    UPHandle MAC_DVS_Login(UPSint8 *addr, UPUint32 port, UPUint8 *userName, UPUint8 *password);
    UPUint	 MAC_DVS_Logout(UPHandle hDvs);
/*    
    //…Ë÷√…Ë±∏œ˚œ¢ªÿµ˜∫Ø ˝
    void		 DVS_SetDVRMessCallBack(fMessCallBack cbMessFunc, UNS32 dwUser);
    UNS32	 DVS_StartListen(HANDLE hDvs);
    UNS32	 DVS_StopListen(HANDLE hDvs);
    //ªÒ»°…Ë±∏–≈œ¢
    UNS32	 DVS_GetDeviceInfo(HANDLE hDvs, LPDVS_DEV_INFO lpDeviceInfo);
    
    //∂‘Ω≤
    UNS32	 DVS_SpeechStart(HANDLE hDvs);
    void		 DVS_SpeechStop(HANDLE hDvs);
    UNS32	 DVS_IsOnSpeech(HANDLE hDvs);//∑«0--’˝‘⁄∂‘Ω≤£¨0--√ª”–∂‘Ω≤
    
    // µ ±¡˜‘§¿¿∫Ø ˝
/*    UNS32	 DVS_SetStreamType(HANDLE hDvs, S32 type);//0=÷˜¬Î¡˜,1=◊”¬Î¡˜
*/    UPHandle	 MAC_DVS_OpenRealStream(UPHandle hDvs, UPSint8 *pchHostIP, UPUint32 wHostPort, UPUint8 channel, UPHandle hRealHandle,Stream_Playback *instance);
//  UNS32	 DVS_CloseRealStream(HANDLE hRealHandle);
    UPUint	 MAC_DVS_PlayRealStream(UPHandle hRealHandle);
    UPUint	 MAC_DVS_StopRealStream(UPHandle hRealHandle);
/*    UNS32	 DVS_OpenAudioStream(HANDLE hDvs, S8 *pchHostIP, UNS16 wHostPort, HANDLE &hRealHandle);
    UNS32	 DVS_PlayRealAudio(HANDLE hDvs, HANDLE hRealHandle, UNS8 channel);
    UNS32	 DVS_StopRealAudio(HANDLE hRealHandle);
    UNS32	 DVS_GetRealBiAudio(HANDLE hRealHandle, UNS8 &biAudio);
    UNS32	 DVS_CloseAudioStream(HANDLE hRealHandle);
    UNS8	 DVS_GetRealChnnl(HANDLE hRealHandle);
    void	 DVS_SetRealVideoCallBack(fDataCallBack cbRealVideoFunc, HANDLE hRealHandle, UNS32 dwUser);
    void	 DVS_SetRealAudioCallBack(fDataCallBack cbRealAudioFunc, HANDLE hRealHandle, UNS32 dwUser);
 */   
    //ø™ ºªÚÕ£÷π±£¥Ê µ ±¡˜ ˝æ›Œƒº˛
    UPUint	 MAC_DVS_SaveRealDataStart(UPHandle hRealHandle, UPSint8 *sFileName);
    UPUint	 MAC_DVS_SaveRealDataStop(UPHandle hRealHandle);
 /*   UNS32	 DVS_IsRecording(HANDLE hRealHandle);//∑«0--’˝‘⁄¬ºœÒ£¨0--√ª”–¬ºœÒ
*/    
    //‘∆Ã®øÿ÷∆∫Ø ˝
    UPUint	 MAC_DVS_PTZControl(UPHandle hDvs, UPSint8 channel, UPUint dwPTZCommand, UPUint dwParam);
/*    
    //‘∂≥ÃŒƒº˛ªÿ∑≈
    UNS32	 DVS_QueryRecordFile(HANDLE hDvs, DVS_TIME dtStartTime, DVS_TIME dtEndTime, UNS8 type, LPDVS_RECORDFILE_LIST lpRecordfileList);
    UNS32	 DVS_OpenRemotePlay(HANDLE hDvs, S8 *pchRemoteFileName, UNS8 playCounts, HANDLE &hPlayHandle);
    UNS32	 DVS_CloseRemotePlay(HANDLE hPlayHandle);
    UNS32	 DVS_SetRemoteChannelMask(HANDLE hPlayHandle, UNS32 dwBitMaskChn);
    UNS32	 DVS_RemotePlayControl(HANDLE hPlayHandle, UNS8 dwControlCode);
    UNS32	 DVS_GetRemotePlayTime(HANDLE hPlayHandle, LPDVS_TIME lpCurPlayTime);
    UNS32	 DVS_StartRemotePlayAudio(HANDLE hPlayHandle, UNS8 channel);
    UNS32	 DVS_StopRemotePlayAudio(HANDLE hPlayHandle, UNS8 channel);
    UNS32	 DVS_SeekRemotePlay(HANDLE hPlayHandle, UNS8 type, UNS64 dwArg);//type£∫1∞¥’’ ±º‰∂®Œª£¨2∞¥’’◊÷Ω⁄∂®Œª£¨‘› ±÷ª÷ß≥÷1
    void	 DVS_SetRemoteDataCallBack(fDataCallBack cbRemoteDataFunc, HANDLE hPlayHandle, UNS32 dwUser);
    
    //‘∂≥ÃŒƒº˛œ¬‘ÿ
    UNS32	 DVS_DumpOpen(HANDLE hDvs, S8 *pDumpFile, S8 *pSaveAs, UNS32 startTime, UNS32 endTime, UNS32 chnnlMask);
    void	 DVS_DumpClose(HANDLE hDvs);
    S32		 DVS_DumpProgress(HANDLE hDvs);
 */   
    //±æµÿŒƒº˛ªÿ∑≈
    UPHandle	 MAC_DVS_OpenLocalPlay(UPSint8 *pchLocalFileName, UPUint8 playCounts, UPHandle hPlayHandle);
/*    UNS32	 DVS_GetDuration(HANDLE hPlay, UNS32 &startTime, UNS32 &endTime);
    UNS32	 DVS_GetLocalPlayCurTime(HANDLE hPlayHandle, UNS32 curTime);
    UNS32	 DVS_CloseLocalPlay(HANDLE hPlayHandle);
    UNS32	 DVS_SetLocalChannelMask(HANDLE hPlayHandle, UNS32 dwBitMaskChn);
    UNS32	 DVS_LocalPlayControl(HANDLE hPlayHandle, UNS8 dwControlCode);
    UNS32	 DVS_StartLocalPlayAudio(HANDLE hPlayHandle, UNS8 channel);
    UNS32	 DVS_StopLocalPlayAudio(HANDLE hPlayHandle, UNS8 channel);
    UNS32	 DVS_SeekLocalPlay(HANDLE hPlayHandle, UNS8 type, UNS64 dwArg);
    void	 DVS_SetLocalDataCallBack(fDataCallBack cbLocalDataFunc, HANDLE hPlayHandle, UNS32 dwUser);
    
    //≈–∂œŒƒº˛ «∑Ò≤•∑≈Ω· ¯
    UNS32	 DVS_PlayIsEnd(HANDLE hPlayHandle);
    UNS32	 DVS_IsOnline(HANDLE hDvs);//DVS‘⁄œﬂ◊¥Ã¨£¨∑«0--‘⁄œﬂ£¨0--≤ª‘⁄œﬂ
    UNS32	 DVS_GetUserNetRight(HANDLE hDvs, USER_NETRIGHT *pNetAccess);//ªÒ»°”√ªß»®œﬁ
    UNS32	 DVS_GetDvrVersion();//ªÒ»°DVR∞Ê±æ
    UNS32	 DVS_SetChName(HANDLE hRealHandle, S8 *cChName);//…Ë÷√Õ®µ¿√˚≥∆
    UNS32	 DVS_GetLinkNum(HANDLE hDvs, NET_LINK_INFO *pNetLinkInfo);//ªÒ»°Õ¯¬Á◊ ‘¥¡¨Ω” ˝
    UNS32	 DVS_ChnnlNameChanged(HANDLE hDvs);//Õ®µ¿√˚≥∆ «∑Ò∏ƒ±‰£¨∑«0--“—∏ƒ±‰£¨0--Œ¥∏ƒ±‰
    UNS32	 DVS_RequestEmailTest(HANDLE hDvs);//«Î«ÛDVR∑¢ÀÕ≤‚ ‘” º˛
    
    //Õ¯¬Á≤Œ ˝…Ë÷√”ÎªÒ»°
    UNS32	 DVS_SetConfig(HANDLE hDvs, UNS32 cmd, LPVOID lpvoid, UNS32 optSize);
    UNS32	 DVS_GetConfig(HANDLE hDvs, UNS32 cmd, LPVOID lpvoid, UNS32 bufSize);
*/    
//#ifdef __cplusplus
//}
//#endif /*__cplusplus*/

#endif
