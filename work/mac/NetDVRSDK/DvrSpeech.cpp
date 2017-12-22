#include "DvrSpeech.h"
#include "Global.h"

//UNSL32 __attribute__((stdcall)) bsrAudioPlayer(LPVOID lpvoid);
//UNSL32 __attribute__((stdcall)) bsrSpeech(LPVOID lpvoid);
UNSL32  bsrAudioPlayer(LPVOID lpvoid);
UNSL32  bsrSpeech(LPVOID lpvoid);

#define MAX_FRAMESIZE		(512 * 1024)
DvrSpeech::DvrSpeech()
{
	m_pFifo = new SpeechFifo(MAX_FRAMESIZE);
	m_bExit = false;
	m_bEndSpeech = false;
}

DvrSpeech::~DvrSpeech()
{
	Destroy();
	if (m_pFifo != NULL)
	{
		delete m_pFifo;
		m_pFifo = NULL;
	}
}

S32 DvrSpeech::Create(const S8 *ip, UNS16 port, WAVEFORMATEX format, S8 *pTicket, S32 ticketlen)
{
	do
	{
		if (m_pFifo == NULL)
		{
			break;
		}

		format.nAvgBytesPerSec	= format.nChannels*format.nSamplesPerSec*format.wBitsPerSample/8;
		format.nBlockAlign		= format.nChannels*format.wBitsPerSample/8;
		format.cbSize			= 0;		
		memcpy(&m_AudioFormat, &format, sizeof(format));

		if (m_DataSocket.Init(ip, port, m_pFifo) != 0)
		{
			break;
		}

		if (pTicket != NULL)
		{
			m_DataSocket.SendData(pTicket, ticketlen);
		}

		m_bExit = false;
		if (!m_hPlayer.CreateAThread(bsrAudioPlayer, this))
			break;
		if (SpeechStart() != 0)
		{
			break;
		}

		return 0;
	} while (0);

	Destroy();
	return 1;
}

S32 DvrSpeech::Destroy()
{
	m_bExit = true;
	m_hPlayer.ExitAThread();
	SpeechStop();
	m_DataSocket.UnInit();

	if (NULL != m_pFifo)
	{
		m_pFifo->Reset();
	}

	return 0;
}

S32 DvrSpeech::SpeechStart()
{
	m_bEndSpeech = false;	
	if (!m_hSpeech.CreateAThread(bsrSpeech, this))
		return -1;

	return 0;
}

S32 DvrSpeech::SpeechStop()
{
	m_bEndSpeech = true;
	m_hSpeech.ExitAThread();

	return 0;
}

UNSL32 bsrAudioPlayer(LPVOID lpvoid)
{
	DvrSpeech* player = (DvrSpeech*)lpvoid;	
	if (player == NULL)
	{
		return 0;
	}

	S32 maxLen = player->m_AudioFormat.nChannels * player->m_AudioFormat.nSamplesPerSec * player->m_AudioFormat.wBitsPerSample / 32;	
	S8* pWav = (S8*)malloc(maxLen);	
	if (pWav == NULL)
	{
		return 0;
	}

	S32 len = 0;	
	//bool bStop = false;
	while (!player->m_bExit)
	{
		if (NULL == player->m_pFifo)
		{
			break;
		}

		len = player->m_pFifo->GetData(pWav, maxLen);
		if (len <= 0)
		{
			Sleep(10);
			continue;
		}

		//bStop = false;
		//Speech audio call back, not decode, get data from dvr
	}

	if (pWav)
	{
		free(pWav);
		pWav = NULL;
	}

	return 0;
}

#define SPEECH_ENCODE_SIZE 512
UNSL32 bsrSpeech(LPVOID lpvoid)
{
	DvrSpeech* player = (DvrSpeech*)lpvoid;	
	if (player == NULL)
	{
		return 0;
	}
	
	S16 RawFrameSize = 0;
	//S32 EncFrameSize = 0;	
	switch(player->m_AudioFormat.wFormatTag)
	{
	case AUDIO_CODEC_ADPCM:
		RawFrameSize = 320;
		break;
	case AUDIO_CODEC_G711:
		RawFrameSize = 256; //?
		break;
	case AUDIO_CODEC_G723:
		RawFrameSize = 256;
		break;
	case AUDIO_CODEC_G726:
		RawFrameSize = 256;
		break;
	case AUDIO_CODEC_G729:
		RawFrameSize = 256;
		break;
	default:
		return 0;
	}
	
	S8* pData = (S8*)malloc(2 * RawFrameSize);
	S8* pBuf = (S8*)malloc(RawFrameSize);	
	S8* pEncode = pBuf + sizeof(PES_PACKET_HEADER) + sizeof(UNS32);
	PES_PACKET_HEADER* pFrmHdr = (PES_PACKET_HEADER*)(pBuf + sizeof(UNS32));
	
	//S32 len = 0;
	//S32 temp = 0;
	//S32 ret = 0;
	UNS32 magic = MAGIC_START;
	memcpy(pBuf, &magic, sizeof(magic));
	
	pFrmHdr->channel_id = 0;
	pFrmHdr->deinterlace = 0;
	pFrmHdr->enc_format = 1;
	pFrmHdr->frame_num_inGOP = 0;
	pFrmHdr->av_type = DT_AUDIO;
	pFrmHdr->avInfo.VideoInfo.BindAudioChn = 0;
	pFrmHdr->avInfo.AudioInfo.sampleRate = SAMPLE_RATE_8KHZ;
	pFrmHdr->avInfo.AudioInfo.bitsPerSample = WORD_WIDTH_16BIT;
	pFrmHdr->avInfo.VideoInfo.vop_type = 0;
	
	do{
		pFrmHdr->pts = GetTickCount()*1000;
		pFrmHdr->ltime = (S32)time(0);
		//pData for voice encode pEncode call back
		//附加帧结束标示
		magic = MAGIC_END;
		memcpy(pEncode + pFrmHdr->len, &magic, sizeof(magic));		
		player->m_DataSocket.SendData(pBuf, pFrmHdr->len + sizeof(PES_PACKET_HEADER) + sizeof(magic) * 2);
	} while (!player->m_bEndSpeech);
	
	if (pData)
	{
		free(pData);
		pData = NULL;
	}
	
	if (pBuf)
	{
		free(pBuf);
		pBuf = NULL;
	}
	
	return 0;
}
