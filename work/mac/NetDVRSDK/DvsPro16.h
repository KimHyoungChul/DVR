#pragma once
#ifndef _DVSPRO16_H_
#define _DVSPRO16_H_

#include "Dvs.h"
#include "Global.h"
#include "DvrSpeech.h"
#include "RemoteConfig.h"
#include "RemoteFilePlayer.h"
#include "PSWriter.h"

typedef struct
{
	SOCKET sCmdSocket;
	UNS32 nCmdPort;
	UNS32 nIDCode;
	UNS32 nRemoteViewIP;
	NET_USER_ACCESS userAccess;
}DevInitUtility;

class DvsPro16 : public Dvs
{
public:
	DvsPro16();
	DvsPro16(DevInitUtility devInitUtility);
	~DvsPro16();

	virtual void Logout();
	virtual UNS32 GetDvrConfig(UNS32 cmdId, LPVOID pCfg, UNS32 size);
	virtual UNS32 SetDvrConfig(UNS32 cmdId, LPVOID pCfg, UNS32 size);
	virtual bool RemotePlayIsEnd();
	//Զ�̻ط�
	virtual UNS32 RemoteDumpOpen(S8 *filename, S8 *pSaveAs, UNS32 startTime, UNS32 endTime, UNS32 chnnlMask);
	virtual void RemoteDumpClose();
	virtual S32 RemoteDumpProgress();
	virtual UNS32 SpeechStart(const S8* ip, UNS16 port);
	virtual void SpeechStop();
	virtual bool IsSpeech();
	virtual void GetBiAudio(UNS8 chnnl, UNS8 &biAudio);
	virtual void GetVideoSignalType(UNS8 &videoType);//��ȡ������Ƶ�ź�����
	virtual void GetChnnlName(UNS8 chnnl, S8 *chnnlName);
	virtual void GetVideoResolution(UNS8 chnnl, bool bSub, UNS8 &videoRes);		
	UNS32 QueryRecordFile(DVS_TIME dtStartTime, DVS_TIME dtEndTime, S32 type, LPDVS_RECORDFILE_LIST lpRecordfileList);
	UNS32 OpenRemotePlay(S8 *pchRemoteFileName, UNS8 playCounts);
	void CloseRemotePlay();
	void SetRemoteChannelMask(UNS32 chnnlMask);
	void RemotePlayControl(UNS8 dwControlCode);
	bool GetRemotePlayTime(LPDVS_TIME lpCurPlayTime);
	void StartRemotePlayAudio(UNS8 channel);
	void StopRemotePlayAudio();
	void SeekRemotePlayByTime(UNS64 dwOffsetTime);
	UNS32 PTZControl(UNS8 channel, UNS32 dwPTZCommand, UNS32 dwParam);
	void StartDVRMessCallBack(fMessCallBack cbMessFunc, HANDLE hDvs, UNS32 dwUser);
	void StopDVRMessCallBack();	
	bool InitDvs();
	bool GetDeviceInfo(LPDVS_DEV_INFO lpDeviceInfo);
	bool IsOnline();	
	void GetUserRight(LPVOID pCfg, S32 size);//��ȡ�û�Ȩ��
	void GetLinkNum(LPVOID pCfg, S32 size);//��ȡ������Դ������
	bool ChnnlNameChanged();//ͨ�����Ƹı�
	UNS32 RequestEmailTest();//����DVR���Ͳ����ʼ�
	void SetRemoteDataCallBack(process_cb_ex cbRemoteFunc, void* dwUser);

	void DumpTask();

private:
	S32  CompareDvsTime(DVS_TIME dtFirstTime, DVS_TIME dtSecondTime);
	DvrSpeech   *m_pSpeech;

public:
	DVRSocket *m_pDVRSocket;
	CRemoteConfig m_RemoteConfig;
	CRemoteFilePlayer m_RemotePlayer;
	DVS_DEV_INFO m_DeviceInfo;
	S32 m_nOldRemotePlayMode;

private: //����ר��
	PS_FILE_HDR m_dumpFileInfo;
	PSWriter    *m_pWriter;
	LARGE_INTEGER m_dumpFileSize;
	LARGE_INTEGER m_dumpedSize;
	MyThread	m_hDump;
	bool		m_bExitDump;
};

#endif
