#pragma once
#ifndef _DVS_H_
#define _DVS_H_

#include "NetDvrApi.h"
#include "Global.h"

class Dvs
{
public:
	Dvs();
	virtual ~Dvs();

	virtual void Logout();
	virtual bool InitDvs() = 0;
	virtual bool GetDeviceInfo(LPDVS_DEV_INFO lpDeviceInfo) = 0;	
	virtual UNS32 QueryRecordFile(DVS_TIME dtStartTime, DVS_TIME dtEndTime, S32 type, LPDVS_RECORDFILE_LIST lpRecordfileList) = 0;
	virtual UNS32 OpenRemotePlay(S8 *pchRemoteFileName, UNS8 playCounts) = 0;
	virtual void CloseRemotePlay() = 0;
	virtual void SetRemoteChannelMask(UNS32 chnnlMask) = 0;
	virtual void RemotePlayControl(UNS8 dwControlCode) = 0;
	virtual bool GetRemotePlayTime(LPDVS_TIME lpCurPlayTime) = 0;
	virtual void StartRemotePlayAudio(UNS8 channel) = 0;
	virtual void StopRemotePlayAudio() = 0;
	virtual void SeekRemotePlayByTime(UNS64 dwOffsetTime) = 0;
	virtual UNS32 PTZControl(UNS8 channel, UNS32 dwPTZCommand, UNS32 dwParam) = 0;
	virtual void StartDVRMessCallBack(fMessCallBack cbMessFunc, HANDLE hDvs, UNS32 dwUser) = 0;
	virtual void StopDVRMessCallBack() = 0;
	virtual UNS32 GetDvrConfig(UNS32 cmdId, LPVOID pCfg, UNS32 size) = 0;
	virtual UNS32 SetDvrConfig(UNS32 cmdId, LPVOID pCfg, UNS32 size) = 0;
	virtual bool RemotePlayIsEnd() = 0;
	virtual UNS32 RemoteDumpOpen(S8 *filename, S8 *pSaveAs, UNS32 startTime, UNS32 endTime, UNS32 chnnlMask) = 0;
	virtual void RemoteDumpClose() = 0;
	virtual S32 RemoteDumpProgress() = 0;
	//对讲函数
	virtual UNS32 SpeechStart(const S8 *ip, UNS16 port) = 0;
	virtual void SpeechStop() = 0;
	virtual bool IsSpeech() = 0;
	virtual void GetBiAudio(UNS8 chnnl, UNS8 &biAudio) = 0;
	virtual void GetVideoSignalType(UNS8 &videoType) = 0;//获取输入视频信号类型
	virtual void GetChnnlName(UNS8 chnnl, S8 *chnnlName) = 0;
	virtual void GetVideoResolution(UNS8 chnnl, bool bSub, UNS8 &videoRes) = 0;
	virtual bool IsOnline() = 0;
	void SetCmdPort(UNS16 cmdport);
	void SetDvrAddr(const S8 *pAddr);	
	const S8* GetDvrAddr(bool bFromIP = true);
	UNS16 GetCmdPort();	
	virtual void GetUserRight(LPVOID pCfg, S32 size) = 0;//获取用户权限
	virtual void GetLinkNum(LPVOID pCfg, S32 size) = 0;//获取网络资源连接数
	virtual bool ChnnlNameChanged() = 0;//通道名称改变
	virtual UNS32 RequestEmailTest() = 0;//请求DVR发送测试邮件
	virtual void SetRemoteDataCallBack(process_cb_ex cbRemoteFunc, void* dwUser) = 0;

	UNS16 m_cmdPort;
	S8 m_cAddr[DVS_MAX_DOMAINLEN + 1];
};

#endif
