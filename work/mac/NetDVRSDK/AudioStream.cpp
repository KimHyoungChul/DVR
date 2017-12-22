#include "AudioStream.h"
#include "PESPacketHead.h"
#include "macro.h"

extern PSWriter *m_pShareWriter;//for record audio
extern int errno;

UNSL32 AudioThread(LPVOID lpvoid)
{
	CAudioStream *pAudioStream = (CAudioStream*)lpvoid;
	if (pAudioStream != NULL)
	{
		pAudioStream->ReceiveAudio();
	}

	return 0;
}

CAudioStream::CAudioStream()
{
	m_bExitAudio = false;
	m_pAudioStream = NULL;	
	m_pAudioFifo = new StreamFifo(MAX_DECODE_FRAMESIZE);
	m_biAudio = 0;
	m_bSaveRealAudio = false;
	memset(&m_cbRealAudioData, 0, sizeof(CbRealAudioStruct));
}

CAudioStream::~CAudioStream()
{
	CloseAudioStream();
	DestroyAudioStream();

	if (m_pAudioFifo != NULL)
	{
		delete m_pAudioFifo;
		m_pAudioFifo = NULL;
	}
	
	if (m_pAudioStream != NULL)
	{
		delete m_pAudioStream;
		m_pAudioStream = NULL;
	}	
}

bool CAudioStream::OpenAudioStream(S8 *destIP, UNS16 port)
{
	do
	{
		if (destIP == NULL)
		{
			break;
		}
		
		if (m_pAudioFifo == NULL)
		{
			break;
		}
		
		S8 ip[DVS_MAX_DOMAINLEN + 1] = { 0, };		
		if (!GetDomainIP(destIP, ip))
		{
			printf("Get domain ip failed!\n");
			break;
		}

		UNSL32 addr = inet_addr(ip);		
		if (!CreateAudioStream(addr, port))
		{
			break;
		}
		
		m_bExitAudio = false;
		if (!m_hAudioThread.CreateAThread(AudioThread, this, STACK_SIZE_PARAM_IS_A_RESERVATION))
			break;
		
		return true;
	} while (0);
	
	return false;
}

bool CAudioStream::PlayRealAudio(UNS8 biAudio)
{
	m_SyncSection.Lock();
	do
	{
		if (biAudio != 0)
		{
			m_biAudio = biAudio;
		}
		
		if (!ChangeAudioStream(m_biAudio))
		{
			break;
		}		
		m_SyncSection.Unlock();
		
		return true;
	} while (0);
	m_SyncSection.Unlock();

	return false;
}

void CAudioStream::StopRealAudio()
{
	if (!m_bSaveRealAudio)
	{
		ChangeAudioStream(0);
	}
}

void CAudioStream::CloseAudioStream()
{
	ChangeAudioStream(0);
	m_bExitAudio = true;
	m_hAudioThread.ExitAThread();
	DestroyAudioStream();
}

bool CAudioStream::CreateAudioStream(UNSL32 ip, UNS16 port)
{
	NET_MEDIA_ACCESS media; 
	media.UserID      = m_userId;
	media.AccessCode  = m_accessId;
	media.StreamType  = AUDIO_STREAM;

	S32 ret = 0;
	S8 cmd[MAX_CMD_LEN + 1] = {0, };
	S32 cmdlen = 0;
	cmdlen = PackCommand(&media, sizeof(media), MIDEA_ACCESS, cmd, sizeof(cmd), m_userId);

	do
	{
		if (cmdlen == 0)
		{
			break;
		}

		if (m_pAudioStream == NULL)
		{
			m_pAudioStream = new StreamSocket();
			if (m_pAudioStream != NULL)
			{
				ret = m_pAudioStream->Init(ip, port, m_pAudioFifo);
			}
		}

		if (ret != 0)
		{
			break;
		}
		
		if (0 != m_pAudioStream->SendData(cmd, cmdlen))
		{
			break;
		}

		KTCMDHDR hdr = { 0, };
		cmdlen = m_pAudioStream->ReadData((S8*)&hdr, sizeof(hdr));
		if (cmdlen != sizeof(hdr))
		{
			break;
		}

		if (hdr.startCode != MAGIC_START)
		{
			break;
		}

		cmdlen = m_pAudioStream->ReadData(cmd, hdr.totalSize - sizeof(hdr));
		if (cmdlen != hdr.totalSize - sizeof(hdr))
		{
			break;
		}
		
		if (hdr.infoType != MIDEA_ACCESS_ACCEPT)
		{
			break;
		}
		
		if (m_pAudioStream->Start())
		{
			return true;
		}
	} while (0);

	DestroyAudioStream();
	return false;
}

void CAudioStream::DestroyAudioStream()
{
	if (m_pAudioStream != NULL)
	{
		delete m_pAudioStream;
		m_pAudioStream = NULL;
	}
}

bool CAudioStream::ChangeAudioStream(UNS8 chnnl)
{
	NET_PLAY_STATUS_CHANGE StatusChange;
	memset(&StatusChange, 0, sizeof(NET_PLAY_STATUS_CHANGE));
	StatusChange.PlayStatus = AUDIO_CHANGE;
	StatusChange.SecondData = chnnl;
	
	S8 cmd[MAX_CMD_LEN + 1] = {0, };
	S32 cmdlen = PackCommand(&StatusChange, sizeof(StatusChange), PLAY_STATUS_CHANGE, cmd, sizeof(cmd), m_userId);	
	if (m_pAudioStream != NULL)
	{
		if (0 == m_pAudioStream->SendData(cmd, cmdlen))
		{
			return true;
		}
	}

	return false;
}

void CAudioStream::ReceiveAudio()
{
	S8 *buf  = new S8[MAX_DECODE_FRAMESIZE];
	S8 *pPCM = new S8[MAX_AUDIO_OUTPUT_BUF];
	UNS32 length	= 0;
	PES_PACKET_HEADER *PESHead = NULL;
	S32 avtype			= VIDEO_FRAME;
	S32 chID			= 0;
	S8 *curMP4Buf		= NULL;

	while (!m_bExitAudio)
	{
		length = 0;
		if (m_pAudioFifo != NULL)
		{
			length = m_pAudioFifo->PopFrame((S8*)buf, MAX_DECODE_FRAMESIZE);
		}
		
		if (length <= 0)
		{
			Sleep(10);
			continue;
		}
		
		if (length > MAX_DECODE_FRAMESIZE)
		{
			continue;
		}
		
		PESHead = (PES_PACKET_HEADER*)buf;
		chID	= PESHead->channel_id;
		avtype  = PESHead->av_type;
		if (avtype == AUDIO_FRAME)
		{
			//判断是否是该通道的伴音
			if (chID + 1 != m_biAudio)
			{
				continue;
			}
			//for record audio
			PESHead->avInfo.VideoInfo.BindAudioChn = chID + 1;
			SaveRealAudioData((S8*)buf, length);

			curMP4Buf = (S8*)(buf + PESHEADER_SIZE);
			//Real audio call back
			if (m_cbRealAudioData.cbRealAudioFunc)
			{
				UNS32 type = REAL_AUDIO;
				m_cbRealAudioData.cbRealAudioFunc(type, m_biAudio, curMP4Buf, PESHead->len, m_cbRealAudioData.dwUser);
			}
		}
	}
	
	if (pPCM != NULL)
	{
		delete []pPCM;
		pPCM = NULL;
	}

	if (buf != NULL)
	{
		delete []buf;
		buf = NULL;
	}
}

bool CAudioStream::GetDomainIP(S8 *pDomain, S8 *ip, bool bFromIP)
{
	if (pDomain[0] == '\0' || ip == NULL)
	{
		return false;
	}

	if (!bFromIP)
	{
		struct hostent *host = NULL;
		//host = gethostbyname((LPCSTR)pDomain);
		host = gethostbyname((const char *)pDomain);
		if (host == NULL || host->h_addr == NULL)
		{
			//sprintf_s("Error code = %d\n", GetLastError());
			printf("Error code = %s\n", strerror(errno));
			return false;
		}
		sprintf(ip, "%s", (S8*)inet_ntoa(*(struct in_addr*)(host->h_addr)));
	}
	else
		strncpy(ip, pDomain, 16);

	return true;
}

void CAudioStream::SetUserInfo(UNS32 userId, UNS32 accessId)
{
	m_userId = userId;
	m_accessId = accessId;
}

//for record audio
void CAudioStream::SaveRealAudioData(S8 *ps, UNS32 len)
{	
	m_writeSection.Lock();
	if (m_pShareWriter != NULL)
	{
		m_bSaveRealAudio = true;
		m_pShareWriter->PushPacket(ps, len);
	}else
	{
		m_bSaveRealAudio = false;
	}
	m_writeSection.Unlock();
}

void CAudioStream::SetRealAudioCallBack(fDataCallBack cbRealAudioFunc, UNS32 dwUser)
{
	if(cbRealAudioFunc)
	{
		m_cbRealAudioData.cbRealAudioFunc = cbRealAudioFunc;
		m_cbRealAudioData.dwUser = dwUser;
	}
	else
	{
		memset(&m_cbRealAudioData, 0, sizeof(CbRealAudioStruct));
	}
}
