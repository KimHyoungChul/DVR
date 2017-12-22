// LocalFilePlayer.h: interface for the CLocalFilePlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOCALFILEPLAYER_H__3E38F522_3B0B_4BFF_8FA9_9791A292ED41__INCLUDED_)
#define AFX_LOCALFILEPLAYER_H__3E38F522_3B0B_4BFF_8FA9_9791A292ED41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PSFile.h"
#include "Event.h"
#include "Thread.h"

#define PLAY_BACKWARD 1
#define PLAY_FORWARD  2
#define PLAY_FIRST    3

typedef struct
{
	fDataCallBack	cbLocalFunc;
	UNS32			dwUser;
}CbLocalStruct;

class CLocalFilePlayer  
{
public:
	CLocalFilePlayer();
	virtual ~CLocalFilePlayer();

	bool IsEnd();
	void PlayAudio(UNS8 chnnl);
	void StopAudio();
	bool GetDuration(UNS32 &startTime, UNS32 &endTime);
	UNS32 GetCurPlayTime();
	UNS32 GetCurTime();
	UNS32 GetCurDate();
	//Open ps file and initialize the resource used by local file player
	//注意，该打开函数同时还绑定了将要显示的窗口句柄数组
	bool OpenPSFile(S8 *filename, UNS8 nHwndCount);
    bool OpenLocalPlay(S8 *pchLocalFileName, UNS8 playCounts);
	void PlayAllVideo();			//play all video stream
	void Pause();					//pause all playing stream
    void Stop();                    //czg 08-13
	bool Seek(UNS64 pos);  //seek the prefer position, and unit is ms.
    void SetLocalChannelMask(UNS32 chnnlMask);
	void StepNext();				//display next frame
	void StepBack();				//display previous frame
	void ClosePlayer();			//close the player and release the resource allocated by OpenPSFile.
	void FastBackward();           //
	void Backward();
	void SetPlayRate(S32 rate = 1);//rate >0 && rate < 10;
	void SetPlayIFrame(bool bIFrame){ m_bKeyFrame = bIFrame; }
	bool IsPlayIFrame() { return m_bKeyFrame ; }
	void SetNeedRenderChn(UNS32 Chn/*按bit*/);//ku 如果是新显示的通道，要把寻找i帧设置成flase
	void SetLocalDataCallBack(fDataCallBack cbLocalFunc, UNS32 dwUser);

	MyEvent	m_hStepEvent;

	bool	m_bExit;
	bool	m_bStepPlay; //标志现在时候进行播放
	bool	m_bPause;    //标志视频是否停止
	bool	m_bMove;
	bool	m_bFastPlay;
	bool	m_bNeedReset[MAX_SOLO_CHANNEL_16];
	
	S32		m_PlayDirect; //用来表示播放模式，如快进，后退
	S32		m_AudioChnnl;
	S32		m_BackSleep;
	S32		m_nOldLocalPlayMode;
	S32		m_RateScale; //用来控制播放速率
	UNS8	m_biAudio;
	//该定义用来表示需要显示的视频通道
	UNS32	m_NeedRenderChn;//ku 12-26
	UNS32	m_VideoWidth[MAX_SOLO_CHANNEL_16];
	UNS32	m_VideoHeight[MAX_SOLO_CHANNEL_16];
	
	CPSFile m_PESPack;
	CMyCriticalSection m_SeekSection;	
	CMyCriticalSection m_AudioSection;	
	CbLocalStruct m_cbLocalData;

private:
	MyThread	m_hPlayer;//用来标志播放线程的句柄
	bool		m_bKeyFrame; //用来判断是否I帧
};

#endif // !defined(AFX_LOCALFILEPLAYER_H__3E38F522_3B0B_4BFF_8FA9_9791A292ED41__INCLUDED_)

