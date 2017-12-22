#pragma once
#ifndef _AUDIOSTREAM_H_
#define _AUDIOSTREAM_H_

#include "DVRSocket.h"
#include "PSWriter.h"
#include "StreamSocket.h"

typedef struct
{
	fDataCallBack	cbRealAudioFunc;
	UNS32			dwUser;
}CbRealAudioStruct;

class CAudioStream
{
public:
	CAudioStream();
	~CAudioStream();

	//音频相关
	bool OpenAudioStream(S8 *destIP, UNS16 port);
	bool PlayRealAudio(UNS8 biAudio = 0);
	void StopRealAudio();
	void CloseAudioStream();
	void ReceiveAudio();
	bool GetDomainIP(S8 *pDomain, S8 *ip, bool bFromIP = true);
	void SetUserInfo(UNS32 userId, UNS32 accessId);
	void SetRealAudioCallBack(fDataCallBack cbRealAudioFunc, UNS32 dwUser);

private:
	//创建音频数据接收相关
	bool CreateAudioStream(UNSL32 ip, UNS16 port);
	void DestroyAudioStream();
	bool ChangeAudioStream(UNS8 chnnl);	
	void SaveRealAudioData(S8 *ps, UNS32 len);

private:
	StreamSocket *m_pAudioStream;
	StreamFifo	*m_pAudioFifo;
	MyThread	m_hAudioThread;
	bool		m_bExitAudio;
	bool		m_bSaveRealAudio;

	CMyCriticalSection m_SyncSection;
	CMyCriticalSection m_writeSection;

	UNS8 m_biAudio;
	UNS32 m_userId;
	UNS32 m_accessId;

	CbRealAudioStruct m_cbRealAudioData;
};

#endif
