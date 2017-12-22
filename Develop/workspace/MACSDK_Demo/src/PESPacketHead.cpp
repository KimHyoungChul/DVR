// PESPacketHead.cpp: implementation of the CPESPacketHead class.
//
//////////////////////////////////////////////////////////////////////
#include "macro.h"
#include "PESPacketHead.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPESPacketHead::CPESPacketHead()
{
	memset(&m_pesPackHead, 0, sizeof(m_pesPackHead));
	m_pesPackHead.cur_pes_packet_length = 1;
	m_pesPackHead.cur_time = 0;
	m_pesPackHead.cur_date = 0;
}

CPESPacketHead::~CPESPacketHead()
{
}

void CPESPacketHead::InitPESPacket()
{
	m_pesPackHead.cur_pes_packet_length = 1;
	m_pesPackHead.cur_time = 0;
	m_pesPackHead.cur_date = 0;
}

void CPESPacketHead::SetPESBuf(S8 *pBuf)
{
	m_pBuf = pBuf;
	ParsePESPackHead(&m_pesPackHead, m_pBuf);
}

time_t CPESPacketHead::GetCurLtime()
{
	return m_pesPackHead.ltime;
}

UNS32 CPESPacketHead::GetPTS()
{
	return (UNS32)(m_pesPackHead.pts);
}

UNS32 CPESPacketHead::GetAVType()
{
	return m_pesPackHead.av_type;
}

UNS32 CPESPacketHead::GetCurTime()
{
	UNS32 tmp = m_pesPackHead.cur_time;
	return tmp;
}

UNS32 CPESPacketHead::GetCurDate()       
{
	UNS32 tmp = m_pesPackHead.cur_date;
	return tmp;
}

void CPESPacketHead::ParsePESPackHead(PES_PACKET_HEAD *pesPackHead, S8 *pes)
{
    PES_PACKET_HEADER* pPacketHeader = (PES_PACKET_HEADER*)pes;
	if (pPacketHeader == NULL || pesPackHead == NULL)
	{
		return;
	}
	pesPackHead->chn_id = pPacketHeader->channel_id;
	pesPackHead->cur_pes_packet_length = pPacketHeader->len;
    pesPackHead->av_type = pPacketHeader->av_type;
	pesPackHead->vop_type = pPacketHeader->avInfo.VideoInfo.vop_type;
	pesPackHead->bdeinterlace = pPacketHeader->deinterlace;
	pesPackHead->pts = pPacketHeader->pts;
	pesPackHead->vsize = 0;
	pesPackHead->hsize = 0;
	pesPackHead->ltime = pPacketHeader->ltime;
	pesPackHead->nAudioChannels = pPacketHeader->avInfo.VideoInfo.BindAudioChn;
	
	struct tm* pCurPlayTime = NULL;
	localtime_s(pCurPlayTime, (const time_t*)&pPacketHeader->ltime);
	if (pCurPlayTime != NULL)
	{
		pesPackHead->cur_time = (pCurPlayTime->tm_hour<<16)|(pCurPlayTime->tm_min<<8)|(pCurPlayTime->tm_sec);
		pesPackHead->cur_date = ((pCurPlayTime->tm_year+1900)<<16)|((pCurPlayTime->tm_mon+1)<<8)|(pCurPlayTime->tm_mday);
	}
}

UNS8 CPESPacketHead::GetBiAudio()
{
	return (UNS8)m_pesPackHead.nAudioChannels;
}
