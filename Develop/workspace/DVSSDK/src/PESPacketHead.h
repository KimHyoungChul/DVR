// PESPacketHead.h: interface for the CPESPacketHead class.
//
// 这个类是用来专门解析pes包头的
// 有好多地方都应该使用这个类，由于是后来才写的，要以后在整合到其他程序中去
// 可以使用这个类的地方有 CPSFile 和 李工 写的好多类
//
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PESPACKETHEAD_H__47348D9D_8EB7_48D0_BBF1_D55DBE725543__INCLUDED_)
#define AFX_PESPACKETHEAD_H__47348D9D_8EB7_48D0_BBF1_D55DBE725543__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Lock.h"

#define MAX_PLAYBACK_BUF_SIZE	(512 * 1024)//512KB = 4Mbits
#define MAX_DECODE_FRAMESIZE	(512 * 1024)
#define MAX_AUDIO_OUTPUT_BUF	(2 * 1024)
#define PESHEADER_SIZE			24
#define TARGET_RESOLUTION		2
#define VIDEO_FRAME				2
#define AUDIO_FRAME				3

class CPESPacketHead  
{
public:
	CPESPacketHead();
	virtual ~CPESPacketHead();

	void SetPESBuf(S8 *pBuf);
	void InitPESPacket();
	UNS32 GetPESLength()		{ return m_pesPackHead.cur_pes_packet_length; }
	UNS32 GetChnID()			{ return m_pesPackHead.chn_id;}
	UNS32 GetPTS();
	UNS32 GetVopType()		{ return m_pesPackHead.vop_type; }
	UNS32 GetCurTime();
	UNS32 GetCurDate();
	UNS32 GetPESInterlace()   { return m_pesPackHead.interlace;}
	time_t GetCurLtime();
	UNS32 GetAVType();
	UNS8 GetBiAudio();
	
private:
	void ParsePESPackHead(PES_PACKET_HEAD *pesPackHead, S8 *pes);

	PES_PACKET_HEAD m_pesPackHead;
	S8 *m_pBuf;
	CMyCriticalSection m_Section;
};

#endif // !defined(AFX_PESPACKETHEAD_H__47348D9D_8EB7_48D0_BBF1_D55DBE725543__INCLUDED_)


