// RemoteFilePlayer.h: interface for the CRemoteFilePlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOTEFILEPLAYER_H__0E019F87_54E3_41D2_86FC_D370EA306429__INCLUDED_)
#define AFX_REMOTEFILEPLAYER_H__0E019F87_54E3_41D2_86FC_D370EA306429__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RemotePSFile.h"

typedef void (*process_cb_ex)(void* param, void* data, int len, int dropFrame, int fps);

typedef struct
{
	process_cb_ex	cbRemoteFunc;
	void*			dwUser;
}CbRemoteStruct;
/*
typedef struct
{
	fDataCallBack	cbRemoteFunc;
	UNS32			dwUser;
}CbRemoteStruct;
*/
class CRemoteFilePlayer  
{
public:
	CRemoteFilePlayer();
	virtual ~CRemoteFilePlayer();

	bool InitPlayer(UNS8 playCounts);
	void PlayAudio(UNS8 chnnl);
	void StopAudio();
	void PlayAllVideo();			//play all video stream
    void SendPlayStatus(UNS32 playstatus);
	void Pause();					//pause all playing stream
    void Stop();
	bool Seek(UNS64 pos);  //seek the prefer position, and unit is ms.
	void StepNext();				//display next frame
	void StepBack();				//display previous frame
	void ClosePlayer();			//close the player and release the resource allocated by OpenPSFile.
	void FastBackward();           //
	void Backward();
	void SetPlayRate(S32 rate = 1);//rate >0 && rate < 10;
	void SetPlayIFrame(bool bIFrame){ m_bKeyFrame = bIFrame; }
	bool IsPlayIFrame() { return m_bKeyFrame; }
	bool IsEnd();
	void SetNeedRenderChn(UNS32 Chn/*按bit*/);//ku 如果是新显示的通道，要把寻找i帧设置成flase
	void SetRemoteDataCallBack(process_cb_ex cbRemoteFunc, void* dwUser);

	//Here you can add other attributes.
	MyEvent	m_hStepEvent;

	bool	m_bExit;
	bool	m_bStepPlay;
	bool	m_bPause;
	bool	m_bFastPlay;
	bool	m_bFindIFrame[DVS_MAX_SOLO_CHANNEL_16];
	bool	m_bNeedReset[MAX_SOLO_CHANNEL_16];
	
	CRemotePSFile m_RemotePSFile;	
	CMyCriticalSection m_SeekSection;	
	CMyCriticalSection m_AudioSection;

	S32		m_PlayDirect;
	S32		m_AudioChnnl;
	S32		m_BackSleep;
	S32		m_RateScale;
	UNS32	m_NeedRenderChn;//ku 12-26
	UNS32	m_VideoWidth[MAX_SOLO_CHANNEL_16];
	UNS32	m_VideoHeight[MAX_SOLO_CHANNEL_16];

	CbRemoteStruct m_cbRemoteData;
    
private:
	MyThread	m_hPlayer;
	bool		m_bKeyFrame;
};

#endif // !defined(AFX_REMOTEFILEPLAYER_H__0E019F87_54E3_41D2_86FC_D370EA306429__INCLUDED_)
