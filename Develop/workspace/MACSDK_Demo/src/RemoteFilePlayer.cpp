
// RemoteFilePlayer.cpp: implementation of the RemoteFilePlayer class.
//
//////////////////////////////////////////////////////////////////////
#include "Event.h"
#include "RemotePSFile.h"
#include "RemoteFilePlayer.h"

//////////////play direct//////////////
#define PLAY_BACKWARD	1//back
#define PLAY_FORWARD	2//forward
#define PLAY_FIRST		3//first
#define PLAY_STOP		-1//stop
///////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UNSL32 __attribute__((stdcall)) RemoteFilePlayer(LPVOID lpvoid);

CRemoteFilePlayer::CRemoteFilePlayer()
{
	m_bPause = false;
	m_bExit	  = false;
	m_bStepPlay = false;
	m_hStepEvent.CreateUnsignaledEvent();
	m_PlayDirect = PLAY_STOP;
	m_bFastPlay	 = false;
	m_bKeyFrame		= false;
	m_RateScale		= 1;
	m_BackSleep     = 40;
	m_NeedRenderChn = 0xFFFF;//ku 12-26
	m_AudioChnnl = -1;
	memset(m_bFindIFrame, 0, sizeof(m_bFindIFrame));
	for (S32 i = 0; i < MAX_SOLO_CHANNEL_16; i++)
	{
		m_bNeedReset[i] = true;
	}
	memset(&m_cbRemoteData, 0, sizeof(CbRemoteStruct));
}

CRemoteFilePlayer::~CRemoteFilePlayer()
{
	ClosePlayer();
}

bool CRemoteFilePlayer::InitPlayer(UNS8 playCounts)
{
	UNS8 nMode = 0;
	UNS8 nChResolution = 0;
	UNS8 i = 0;
	nMode = m_RemotePSFile.GetpsFileHead()->VGAMode;
	UNS8 nVideoNum = 0;
	for (i=0; i<MAX_SOLO_CHANNEL_16; i++)
	{
		if((m_RemotePSFile.GetpsFileHead()->AVNum>>i)&0x01)
		{
			nVideoNum++;
		}
	}

	nVideoNum = (nVideoNum > playCounts)?playCounts:nVideoNum;
	for (i=0; i<nVideoNum; i++) 
	{
		nChResolution = m_RemotePSFile.GetpsFileHead()->ChResolution[i];
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

	for (i=0; i<MAX_SOLO_CHANNEL_16; i++)
	{
		m_bFindIFrame[i] = false;			
	}
	
	m_bStepPlay	= true;
	m_hStepEvent.SetSignaledEvent();
	m_bExit = false;
	m_RemotePSFile.m_RecvPESPacket = true;
	m_hPlayer.CreateAThread(RemoteFilePlayer, this);
	if (m_hPlayer.ThreadIsNULL())
	{
		return false;
	}

	return true;
}

void CRemoteFilePlayer::SetPlayRate(S32 rate)
{
    m_RateScale = rate;
	for (S32 i = 0; i < MAX_SOLO_CHANNEL_16; i++)
	{
		m_bNeedReset[i] = true;
	}
}

void CRemoteFilePlayer::ClosePlayer()
{
	m_bExit = true;
	m_hStepEvent.SetSignaledEvent();
	m_hPlayer.ExitAThread();
	m_RemotePSFile.CloseRemotePSFile();
	m_RemotePSFile.m_hRecvPESPacketMutex.Unlock();
}

bool CRemoteFilePlayer::Seek(UNS64 pos)
{
	m_SeekSection.Lock();
	for (S32 i=0; i<MAX_SOLO_CHANNEL_16; i++)
	{
		m_bFindIFrame[i] = false;			
	}

	m_RemotePSFile.m_RecvPESPacket = true;
	m_RemotePSFile.MoveTo(pos);//ku 12-26
	for (S32 i = 0; i < MAX_SOLO_CHANNEL_16; i++)
	{
		m_bNeedReset[i] = true;
	}

	if (true == m_bPause || true == m_bStepPlay) 
	{
		m_hStepEvent.SetSignaledEvent();
		PlayAllVideo();
		SendPlayStatus(PLAY_PLUS_0);
		Sleep(100);
		m_bPause = true;
		m_bStepPlay = true;
		m_hStepEvent.SetSignaledEvent();
	}	
	m_SeekSection.Unlock();

	return true;
}

void CRemoteFilePlayer::PlayAllVideo()
{
	m_bPause = false;
	m_bStepPlay = false;
	m_bFastPlay = false;
	for (S32 i = 0; i < MAX_SOLO_CHANNEL_16; i++)
	{
		m_bNeedReset[i] = true;
	}
	m_PlayDirect = PLAY_FORWARD;
	m_RemotePSFile.m_RecvPESPacket = true;
	m_hStepEvent.SetSignaledEvent();
}

void CRemoteFilePlayer::Pause()
{
	m_bPause = true;
}

void CRemoteFilePlayer::Stop()
{
	m_hStepEvent.SetSignaledEvent();
	SendPlayStatus(STOP);//SendPlayStatus(PAUSE);
	for (S32 i = 0; i < MAX_SOLO_CHANNEL_16; i++)
	{
		m_bNeedReset[i] = true;
	}
	m_PlayDirect = PLAY_FIRST;
}

void CRemoteFilePlayer::StepNext()
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
		SetPlayIFrame(0);
	}
	m_bStepPlay = true;
}

void CRemoteFilePlayer::StepBack()
{
	m_PlayDirect = PLAY_BACKWARD;
	if (m_bStepPlay)
	{
		m_hStepEvent.SetSignaledEvent();
	}
	m_bStepPlay = true;
}

void CRemoteFilePlayer::FastBackward()
{
	m_bPause = false;
	m_bStepPlay = false;
	m_hStepEvent.SetSignaledEvent();
	m_bFastPlay		= true;
	m_PlayDirect	= PLAY_BACKWARD;
	m_BackSleep     = 40;
}

void CRemoteFilePlayer::Backward()
{
	m_bPause = false;
	m_bStepPlay = false;
	m_hStepEvent.SetSignaledEvent();
	m_bFastPlay	= true;
	m_PlayDirect = PLAY_BACKWARD;
	m_BackSleep = 150;//
}

UNSL32 __attribute__((stdcall)) RemoteFilePlayer(LPVOID lpvoid)
{
	CRemoteFilePlayer *player = (CRemoteFilePlayer*)lpvoid;
	S8 *pPCM = (S8*)malloc(MAX_AUDIO_OUTPUT_BUF);//李工说用最大的就行07-05-13
	if (pPCM == NULL)
	{
		return 0;
	}

	UNS32 chID = 0;
	UNS32 vopType = 10;
	UNS32 avType = 10;
	bool bSendPlayEof = false;

	while(1)
	{
		if (player->m_bExit)
		{
			break;
		}

		if (player->m_NeedRenderChn == 0)
		{
			continue;
		}

		if (player->m_RemotePSFile.IsEof() && player->m_PlayDirect != PLAY_FIRST)
		{
			Sleep(40);
			if (player->m_PlayDirect != PLAY_BACKWARD)
			{
				if (bSendPlayEof == false)
				{
					Sleep(5);
					bSendPlayEof = true;
				}
				continue;
			}
		}

		//如果没有点击任何播放按钮，此处循环
		if (player->m_PlayDirect == PLAY_STOP)
		{
			Sleep(10);
			continue;
		}

		player->m_SeekSection.Lock();
		if (player->m_PlayDirect == PLAY_BACKWARD)
		{
			if (player->m_RemotePSFile.IsFirstPESPack()) 
			{
				if (bSendPlayEof == false)
				{
					bSendPlayEof = true;
				}
				player->m_SeekSection.Unlock();
				continue;
			}

			while(!player->m_RemotePSFile.m_bRecvFileOffset)
			{
				Sleep(1);
			}
			player->m_RemotePSFile.m_isNormalplay = false;

			do
			{
				player->m_RemotePSFile.MovePreIPESPack();
				if (player->m_RemotePSFile.IsFirstPESPack())
				{
					break;
				}
				chID = player->m_RemotePSFile.GetChnID();
				if (chID >= DVS_MAX_SOLO_CHANNEL_16)
				{
					break;
				}
			} while (!((1<<chID) & player->m_NeedRenderChn));//ku 03-13
		}
		else if (player->m_PlayDirect == PLAY_FORWARD && player->IsPlayIFrame())
		{
			//Here should uncomment the code if pesfile realized 
			while(!player->m_RemotePSFile.m_bRecvFileOffset)
			{
				Sleep(1);
			}
			player->m_RemotePSFile.m_isNormalplay = false;

			do
			{
				player->m_RemotePSFile.MoveNextIPESPack();
				if (player->m_RemotePSFile.IsEof()) 
				{
					break;
				}
				chID = player->m_RemotePSFile.GetChnID();
				if (chID >= DVS_MAX_SOLO_CHANNEL_16)
				{
					break;
				}
			} while (!((1<<chID) & player->m_NeedRenderChn));//ku 03-13
			player->m_RemotePSFile.m_isNormalplay = true;
		}
		else if (player->m_PlayDirect == PLAY_FIRST)
		{
			player->m_RemotePSFile.m_isNormalplay = false;
			player->m_RemotePSFile.MoveFirstIPESPack();
			player->m_bNeedReset[chID] = true;
			player->m_PlayDirect = PLAY_FORWARD;
		}else
		{
			player->m_RemotePSFile.MoveNextPESPack();
			player->m_RemotePSFile.m_isNormalplay = true;
		}

		bSendPlayEof = false;
		player->m_SeekSection.Unlock();

		while(1) 
		{
			if (player->m_RemotePSFile.GetRecvPESPacket() || player->m_bExit)
			{
				if (player->m_RemotePSFile.m_bTimeNeedReset) 
				{
					player->m_bNeedReset[chID] = true;
					player->m_RemotePSFile.m_bTimeNeedReset = false;
				}
				break;
			}
			Sleep(1);
		}

		chID = player->m_RemotePSFile.GetChnID();
		vopType = player->m_RemotePSFile.GetVopType();
		avType = player->m_RemotePSFile.GetAVType();
		//当包长为0时，表示但前用户，无察看该通道权限
		if (player->m_RemotePSFile.GetPESLength() == 0 || player->m_RemotePSFile.GetPESLength() > MAX_PLAYBACK_BUF_SIZE || chID >= DVS_MAX_SOLO_CHANNEL_16) 
		{
			continue;
		}

		if ((avType == VIDEO_FRAME) && (player->m_bFindIFrame[chID] == false) && (vopType == I_PICTURE))
		{
			player->m_bFindIFrame[chID] = true;
			player->m_bNeedReset[chID] = true;
		}

		if (AUDIO_FRAME == avType)
		{
			if (-1 == player->m_AudioChnnl)
			{
				continue;
			}
			S8 *curMP4Buf = (S8*)player->m_RemotePSFile.GetBuf() + sizeof(PES_PACKET_HEADER);
			S32 curMp4peslen = player->m_RemotePSFile.GetPESLength();
			//Audio call back
			if (player->m_cbRemoteData.cbRemoteFunc)
			{
				UNS32 type = REMOTE_AUDIO;
				player->m_cbRemoteData.cbRemoteFunc(type, (UNS8)chID , curMP4Buf, curMp4peslen, player->m_cbRemoteData.dwUser);
			}
		}

		if (VIDEO_FRAME == avType && player->m_bFindIFrame[chID])
		{
			////////////////////////////
			//原来单步无限等待在这里，移到了该if的最后面
			//////////////////////////////////
			if (player->m_bExit)
			{
				break;
			}

			player->m_SeekSection.Lock();
			S8 *curMP4Buf = (S8*)player->m_RemotePSFile.GetBuf() + sizeof(PES_PACKET_HEADER);
			S32 curMp4peslen = player->m_RemotePSFile.GetPESLength();
			player->m_SeekSection.Unlock();
			//Remote call back
			if (player->m_cbRemoteData.cbRemoteFunc)
			{
				UNS32 type = REMOTE_VIDEO;
				player->m_cbRemoteData.cbRemoteFunc(type, (UNS8)chID , curMP4Buf, curMp4peslen, player->m_cbRemoteData.dwUser);
			}

			if ((player->m_bPause || player->m_bStepPlay) && (player->m_RemotePSFile.GetAVType() == VIDEO_FRAME))
			{
				do
				{
					if (WAIT_OBJECT_0 == player->m_hStepEvent.WaitForMilliSeconds(40))
					{
						break;
					}
				}while(!player->m_bExit);
			}

			if (player->m_PlayDirect == PLAY_BACKWARD)
			{
				 Sleep(80);//修改sleep时间
			}
		}
	}

	if (pPCM != NULL)
	{
		free(pPCM);
		pPCM = NULL;
	}

	return 0;
}

void CRemoteFilePlayer::PlayAudio(UNS8 chnnl)
{
	if (m_AudioChnnl != chnnl)
	{
		m_AudioChnnl = chnnl;
		NET_PLAY_STATUS_CHANGE StatusChange;
		memset(&StatusChange, 0, sizeof(NET_PLAY_STATUS_CHANGE));
		StatusChange.PlayStatus = AUDIO_CHANGE;
		StatusChange.SecondData = m_AudioChnnl + 1;
		if (m_RemotePSFile.m_DVRSocket && m_RemotePSFile.m_DVRSocket->m_bOnLine)
		{
			m_RemotePSFile.m_DVRSocket->RequestOperation(S_PLAY_STATUS_CHANGE, &StatusChange, m_RemotePSFile.m_DVRSocket->m_CmdSocket);
		}
	}
}

void CRemoteFilePlayer::StopAudio()
{
	m_AudioSection.Lock();
	NET_PLAY_STATUS_CHANGE StatusChange;
	memset(&StatusChange, 0, sizeof(NET_PLAY_STATUS_CHANGE));
	StatusChange.PlayStatus = AUDIO_CHANGE;
	StatusChange.SecondData = m_AudioChnnl + 1;
	if (m_RemotePSFile.m_DVRSocket && m_RemotePSFile.m_DVRSocket->m_bOnLine)
	{
		m_RemotePSFile.m_DVRSocket->RequestOperation(S_PLAY_STATUS_CHANGE, &StatusChange, m_RemotePSFile.m_DVRSocket->m_CmdSocket);
	}
	m_AudioChnnl = -1;
	m_AudioSection.Unlock();
}


void CRemoteFilePlayer::SendPlayStatus(UNS32 playstatus)
{
	NET_PLAY_STATUS_CHANGE StatusChange;
	memset(&StatusChange, 0, sizeof(NET_PLAY_STATUS_CHANGE));
	switch(playstatus)
	{
	case PLAY_PLUS_0:
		StatusChange.PlayStatus = PLAY_PLUS_0;
		StatusChange.SecondData = 0;
		break;
	case STOP:
		StatusChange.PlayStatus = STOP;
		StatusChange.SecondData = 0;
        break;
	case PAUSE:
		StatusChange.PlayStatus = PAUSE;
		StatusChange.SecondData = 0;
        m_RemotePSFile.m_bRecvFileOffset = false;
		break;
	}
	if ((playstatus == PLAY_PLUS_0 || playstatus == STOP || playstatus == PAUSE) 
		&& m_RemotePSFile.m_DVRSocket && m_RemotePSFile.m_DVRSocket->m_bOnLine)
	{
		m_RemotePSFile.m_DVRSocket->RequestOperation(S_PLAY_STATUS_CHANGE, &StatusChange, m_RemotePSFile.m_DVRSocket->m_CmdSocket);
	}
}

void CRemoteFilePlayer::SetNeedRenderChn(UNS32 Chn/*按bit*/)
{
	m_SeekSection.Lock();
	m_NeedRenderChn = Chn & (m_RemotePSFile.m_psFileHead.RecordChannel & 0xffff);
	for (S32 i=0; i<DVS_MAX_SOLO_CHANNEL_16; i++)
	{
		S32 oldState = (m_NeedRenderChn & (1<<i));
		S32 newState = (Chn & (1<<i));
		if (newState > oldState) 
		{
			m_bFindIFrame[i] = false;
		}
	}
	m_RemotePSFile.SetCurPlayChn(m_NeedRenderChn);
	for (S32 i = 0; i < MAX_SOLO_CHANNEL_16; i++)
	{
		m_bNeedReset[i] = true;
	}
	m_SeekSection.Unlock();
}

bool CRemoteFilePlayer::IsEnd()
{
	return m_RemotePSFile.IsEof();
}

void CRemoteFilePlayer::SetRemoteDataCallBack(fDataCallBack cbRemoteFunc, UNS32 dwUser)
{
	if (cbRemoteFunc)
	{
		m_cbRemoteData.cbRemoteFunc = cbRemoteFunc;
		m_cbRemoteData.dwUser = dwUser;
	}else
	{
		memset(&m_cbRemoteData, 0, sizeof(CbRemoteStruct));
	}
}
