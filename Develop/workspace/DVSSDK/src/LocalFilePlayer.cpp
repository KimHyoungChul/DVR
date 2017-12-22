// LocalFilePlayer.cpp: implementation of the CLocalFilePlayer class.
//
//////////////////////////////////////////////////////////////////////
#include "Event.h"
#include "LocalFilePlayer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UNSL32 __attribute__((stdcall)) LocalFilePlayer(LPVOID lpvoid);

CLocalFilePlayer::CLocalFilePlayer()
{
	m_bPause = false;
	m_bExit	  = false;
	m_bStepPlay = false;
	m_bMove = false;
	m_hStepEvent.CreateUnsignaledEvent();
	m_PlayDirect = PLAY_FORWARD;
	m_bFastPlay	 = false;
	m_bKeyFrame		= false;
	m_RateScale		= 1;
	m_BackSleep     = 40;
	m_NeedRenderChn = 0xFFFF;//ku 12-26
	m_AudioChnnl = -1;
	m_nOldLocalPlayMode = -1;
	m_biAudio = 0xff;
	for (S32 i = 0; i < MAX_SOLO_CHANNEL_16; i++)
	{
		m_bNeedReset[i] = true;
	}
	memset(&m_cbLocalData, 0, sizeof(CbLocalStruct));
}

CLocalFilePlayer::~CLocalFilePlayer()
{
	ClosePlayer();
}

void CLocalFilePlayer::SetPlayRate(S32 rate)
{	
	m_RateScale = rate;
	for (S32 i = 0; i < MAX_SOLO_CHANNEL_16; i++)
	{
		m_bNeedReset[i] = true;
	}
}

void CLocalFilePlayer::ClosePlayer()
{
	m_bExit = true;
	m_hStepEvent.SetSignaledEvent();
	m_hPlayer.ExitAThread();
	m_PESPack.ClosePSFile();	
}

bool CLocalFilePlayer::OpenPSFile(S8 *filename, UNS8 nHwndCount)
{
	ClosePlayer();

	UNS8 i = 0;
	UNS8 nMode = 0;
	UNS8 nChResolution = 0;
	//原来InitDecoder和m_Render.Init在这，为了动态获得尺寸移动到下面

	do{
		//打开ps文件,同时作如下出示处理
		//1.获取文件头结构
		//2.将索引号赋值为指向第一个包的索引号
		//3.读取第一个包头的信息
		//4.获取第一个包的视频流
		if (m_PESPack.OpenPSFile(filename) != true)
		{
			break;
		} 

		if (m_PESPack.GetPSFileID() != PSFILE_KEY)
		{
			break;
		}

		nMode = m_PESPack.m_psFileHead->VGAMode;
		UNS8 nVideoNum = 0;
		for (i=0; i<MAX_SOLO_CHANNEL_16; i++)
		{
			if((m_PESPack.m_psFileHead->AVNum>>i)&0x01)
			{
				nVideoNum++;
			}
		}
		nVideoNum = nVideoNum>nHwndCount?nHwndCount:nVideoNum;
		for (i = 0; i< nVideoNum; i++)
		{
			//获取文件各个通道的视频格式
			nChResolution = m_PESPack.m_psFileHead->ChResolution[i];
			switch(nChResolution)
			{
			case RESOLUTION_CIF:
				m_VideoWidth[i] = 352;
				if(nMode == 0)
				{//N制
					m_VideoHeight[i] = 240;
				}else
				{//pal制
					m_VideoHeight[i] = 288;
				}
				break;
			case RESOLUTION_D1:
				m_VideoWidth[i] = 704;
				if(nMode == 0)
				{
					m_VideoHeight[i] = 480;
				}else
				{
					m_VideoHeight[i] = 576;
				}
				break;
			case RESOLUTION_HD1:
				m_VideoWidth[i] = 704;
				if(nMode == 0)
				{
					m_VideoHeight[i] = 240;
				}else
				{
					m_VideoHeight[i] = 288;
				}
				break;
			}
		}

		m_bStepPlay	= true;
		m_hStepEvent.SetSignaledEvent();
		m_bExit	  = false;
		m_hPlayer.CreateAThread(LocalFilePlayer, this);
		if (m_hPlayer.ThreadIsNULL())
			break;

		return true;
	} while (0);

	ClosePlayer();
	return false;
}

bool CLocalFilePlayer::OpenLocalPlay(S8 *pchLocalFileName, UNS8 playCounts)
{
	bool bRet = false;
	bRet = OpenPSFile(pchLocalFileName, playCounts);
	if(bRet == false)
	{
		return false;
	}
    
	SetNeedRenderChn(0xffff);
	StopAudio();

	if (m_NeedRenderChn == 0)
	{
		ClosePlayer();
	}

	return true;
}

bool CLocalFilePlayer::GetDuration(UNS32 &startTime, UNS32 &endTime)
{
	if (m_PESPack.m_psFileHead != NULL)
	{
		startTime = m_PESPack.m_psFileHead->TimeStart;
		endTime = m_PESPack.m_psFileHead->TimeEnd;
		return true;
	}

	return false;
}

void CLocalFilePlayer::SetLocalChannelMask(UNS32 chnnlMask)
{
	SetNeedRenderChn(chnnlMask);
}

bool CLocalFilePlayer::Seek(UNS64 pos)
{
	m_SeekSection.Lock();
	m_PESPack.MoveToEx(pos);//ku 12-26
	Sleep(10);
	for (S32 i = 0; i < MAX_SOLO_CHANNEL_16; i++)
	{
		m_bNeedReset[i] = true;
	}
    m_bMove = true;
	m_SeekSection.Unlock();

	if (true == m_bPause || true == m_bStepPlay) 
	{
		m_hStepEvent.SetSignaledEvent();
		Sleep(100);
		m_hStepEvent.SetSignaledEvent();
	}	

	return true;
}

void CLocalFilePlayer::PlayAllVideo()
{
	m_bPause = false;
	m_bStepPlay = false;
	m_bFastPlay = false;
	for (S32 i = 0; i < MAX_SOLO_CHANNEL_16; i++)
	{
		m_bNeedReset[i] = true;
	}
	m_PlayDirect = PLAY_FORWARD;
	m_hStepEvent.SetSignaledEvent();
}

void CLocalFilePlayer::Pause()
{
	m_bPause = true;
}

void CLocalFilePlayer::Stop()
{
	m_hStepEvent.SetSignaledEvent();
	m_PlayDirect = PLAY_FIRST;
}

void CLocalFilePlayer::StepNext()
{
	if (m_PlayDirect == PLAY_BACKWARD)
	{
		if (m_bStepPlay)
		{
			m_hStepEvent.SetSignaledEvent();
			Sleep(100);
		}
	}

	m_PlayDirect = PLAY_FORWARD;
	if (m_bStepPlay)
	{
		m_hStepEvent.SetSignaledEvent();
	}
    
	if (IsPlayIFrame())
	{
		SetPlayIFrame(false);
	}
	
	m_bStepPlay = true;
}

void CLocalFilePlayer::StepBack()
{
	m_PlayDirect = PLAY_BACKWARD;
	if (m_bStepPlay)
	{
		m_hStepEvent.SetSignaledEvent();
	}
	m_bStepPlay = true;
}

void CLocalFilePlayer::FastBackward()
{
	m_bPause = false;
	m_bStepPlay = false;
	m_hStepEvent.SetSignaledEvent();
	m_bFastPlay		= true;
	m_PlayDirect	= PLAY_BACKWARD;
	m_BackSleep     = 40;
}

void CLocalFilePlayer::Backward()
{
	m_bPause = false;
	m_bStepPlay = false;
	m_hStepEvent.SetSignaledEvent();
	m_bFastPlay		= true;
	m_PlayDirect	= PLAY_BACKWARD;
	m_BackSleep     = 150;//
}

UNS32 CLocalFilePlayer::GetCurTime()
{
	return m_PESPack.GetCurTime();
}

UNS32 CLocalFilePlayer::GetCurPlayTime()
{
	return m_PESPack.m_pesPackHead.GetCurLtime();
}

UNS32 CLocalFilePlayer::GetCurDate()
{
	return m_PESPack.GetCurDate();
}

UNSL32 __attribute__((stdcall)) LocalFilePlayer(LPVOID lpvoid)
{
	CLocalFilePlayer *player = (CLocalFilePlayer*)lpvoid;
	S8 *pPCM = (S8*)malloc(MAX_AUDIO_OUTPUT_BUF);
	if (pPCM == NULL)
	{
		return 0;
	}
	
	bool StopFlag = false; //chengzhigang 主要用来标识停止时当前包是否到转到了首帧
	bool bSendPlayEof = false;

	player->m_PESPack.MovePsfileOffset(0);	
	UNS32 chID = 0;//标注播放的通道
	//标注播放的视频类型
	UNS32 vopType = 0;
	UNS32 avType  = 0;

	while(1)
	{
		//播放器是否需要退出
		if (player->m_bExit)
		{
			break;
		}

		if (player->m_NeedRenderChn == 0 || player->m_nOldLocalPlayMode == -1)
		{
			continue;
		}

        //是否播放完
		if (player->m_PESPack.IsEof() && (player->m_PlayDirect != PLAY_FIRST))
		{
			if (player->m_bMove == true) 
			{
				player->m_SeekSection.Lock();
				player->m_PESPack.m_CurIndexItemNum = player->m_PESPack.m_CurIndexItemNum;// - 100;//?
				player->m_PESPack.MovePsfileOffset(player->m_PESPack.m_CurIndexItemNum);
				player->m_bMove = false;
				player->m_SeekSection.Unlock();
			}else
			{
				Sleep(40);
				if (player->m_PlayDirect != PLAY_BACKWARD)
				{
					//如播放到最后一个包，且此时也不是倒退播放模式
					//则退出此次循环，让最后画面继续停留在该窗口上
					if (bSendPlayEof == false)
					{
						Sleep(5);
	    			    bSendPlayEof = true;
					}
					continue;
				}
			}
		}

		player->m_SeekSection.Lock();
		//如果此时是倒退模式		
		if (player->m_PlayDirect == PLAY_BACKWARD)
		{
			if (player->m_PESPack.IsFirstPESPack())
			{
				Sleep(10);
				if (bSendPlayEof == false)
				{
					bSendPlayEof = true;
				}
				player->m_SeekSection.Unlock();
				continue;
			}
			do{
				player->m_PESPack.MovePreIPESPack();
				if (player->m_PESPack.IsFirstPESPack())
				{
					break;
				}
				chID = player->m_PESPack.GetChnID();
			} while (!((1<<chID) & player->m_NeedRenderChn));//ku 03-13
		}
		else if (player->m_PlayDirect == PLAY_FORWARD && player->IsPlayIFrame())//按I帧来前进播放
		{			
			do//Here should uncomment the code if pesfile realized
			{
				player->m_PESPack.MoveNextIPESPack();
				if (player->m_PESPack.IsEof()) 
				{
					break;
				}
				chID = player->m_PESPack.GetChnID();
			} while (!((1<<chID) & player->m_NeedRenderChn));//ku 03-13
		}
		else if (player->m_PlayDirect == PLAY_FIRST)
		{
			player->m_PESPack.MoveFirstIPESPack();
			chID = player->m_PESPack.GetChnID();
			while(!((1<<chID) & player->m_NeedRenderChn)) 
			{
				if (!player->m_PESPack.MoveNextIPESPack())
				{
					continue;
				}
				chID = player->m_PESPack.GetChnID();
			}
			player->m_PlayDirect = PLAY_FORWARD;
			StopFlag = true;
		}else//正常来播放
		{
			do{
				if (!player->m_PESPack.MoveNextPESPack())
				{
					continue;
				}
				if (player->m_PESPack.IsEof())
				{
      				break;
				}
				chID = player->m_PESPack.GetChnID();
				avType = player->m_PESPack.GetAVType();
				if (avType == AUDIO_FRAME)
				{
					break;
				}
			} while (!((1<<chID) & player->m_NeedRenderChn));//ku 03-13
		}
		bSendPlayEof = false;
		player->m_SeekSection.Unlock();
		
		chID    = player->m_PESPack.GetChnID();
		vopType = player->m_PESPack.GetVopType();
		avType  = player->m_PESPack.GetAVType();		
		if ((avType == VIDEO_FRAME) && (vopType == I_PICTURE) && (player->m_PESPack.FindIFrame[chID] == false))
		{
			//主要用来判断当前通道是否已经又I帧出现，是否可以用来显示
			player->m_PESPack.FindIFrame[chID] = true;
			player->m_bNeedReset[chID] = true;
		}
        
		if (player->m_PESPack.GetAVType() == AUDIO_FRAME)
		{
			if (player->m_AudioChnnl == -1) 
			{
				continue;
			}
			else 
			{
				player->m_biAudio = player->m_PESPack.GetBiAudio(player->m_AudioChnnl);
				if (player->m_biAudio == 0xff || chID != player->m_biAudio)
				{
					continue;
				}
			}

			player->m_SeekSection.Lock();
			S8* curMP4Buf = (S8*)player->m_PESPack.GetBuf();
			S32 curMp4peslen = player->m_PESPack.GetPESLength();
			player->m_SeekSection.Unlock();
			//Audio call back
			if (player->m_cbLocalData.cbLocalFunc)
			{
				UNS32 type = LOCAL_AUDIO;
				player->m_cbLocalData.cbLocalFunc(type, (UNS8)chID, curMP4Buf, curMp4peslen, player->m_cbLocalData.dwUser);
			}
		}

		if (avType == VIDEO_FRAME && player->m_PESPack.FindIFrame[chID])
		{
			//解码视频
			player->m_PESPack.m_Section.Lock();
			S8 *curMP4Buf = (S8*)player->m_PESPack.GetBuf();
			S32 curMp4peslen = player->m_PESPack.GetPESLength();
			player->m_PESPack.m_Section.Unlock();
			//Local call back
			if (player->m_cbLocalData.cbLocalFunc)
			{
				UNS32 type = LOCAL_VIDEO;
				player->m_cbLocalData.cbLocalFunc(type, (UNS8)chID, curMP4Buf, curMp4peslen, player->m_cbLocalData.dwUser);
			}

			//如果是暂停播放模式，使线程停止在这里
			if ((player->m_bPause || player->m_bStepPlay) && (player->m_PESPack.GetAVType() == VIDEO_FRAME))
			{
				do
				{
					if (WAIT_OBJECT_0 == player->m_hStepEvent.WaitForMilliSeconds(40))
					{
						break;
					}
				} while (!player->m_bExit);
			}
		}
		//用来控制快退
		if (player->m_PlayDirect == PLAY_BACKWARD)
		{
			Sleep(player->m_BackSleep - 2);
		}	
		/////////////////////////////
		//原来移帧在这里		
		/////////////////////////		
	}
	
	if (pPCM != NULL)
	{
		free(pPCM);
		pPCM = NULL;
	}

	return 0;
}

void CLocalFilePlayer::PlayAudio(UNS8 chnnl)
{
	if (m_AudioChnnl != chnnl)
	{
		m_AudioChnnl = chnnl;
	}
}

void CLocalFilePlayer::StopAudio()
{
	m_AudioSection.Lock();
	m_AudioChnnl = -1;
	m_AudioSection.Unlock();
}

void CLocalFilePlayer::SetNeedRenderChn(UNS32 Chn/*按bit*/)
{
	//原界面每个显示通道进行对比，看是不是需要进行重新显示
	m_NeedRenderChn = Chn & (m_PESPack.m_psFileHead->RecordChannel&0xffff);
	for (S32 i=0; i<MAX_SOLO_CHANNEL_16; i++)
	{
		S32 oldState = (m_NeedRenderChn & (1<<i));
		S32 newState = (Chn & (1<<i));
		if (newState > oldState)
		{//原来没有显示，现在显示了
			//设置第一个I帧的标志值
			m_PESPack.FindIFrame[i] = false;
		}
	}
}

bool CLocalFilePlayer::IsEnd()
{
	return m_PESPack.IsEof();
}

void CLocalFilePlayer::SetLocalDataCallBack(fDataCallBack cbLocalFunc, UNS32 dwUser)
{
	if (cbLocalFunc)
	{
		m_cbLocalData.cbLocalFunc = cbLocalFunc;
		m_cbLocalData.dwUser = dwUser;
	}else
	{
		memset(&m_cbLocalData, 0, sizeof(CbLocalStruct));
	}
}
