// RemotePSFile.h: interface for the CRemotePSFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOTEPSFILE_H__F2E1894B_F1F9_48E6_90D5_915D67743010__INCLUDED_)
#define AFX_REMOTEPSFILE_H__F2E1894B_F1F9_48E6_90D5_915D67743010__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PESPacketHead.h"
#include "DVRSocket.h"
#include "Lock.h"

class CRemotePSFile  
{
public:
	CRemotePSFile();
	virtual ~CRemotePSFile();

	void SetDVRSocket(DVRSocket *pSocket){ m_DVRSocket = pSocket; }
	void SetCurPlayChn(UNS32 chn);
	void CloseRemotePSFile();
	void RecvRemoteFileInfo();
	bool RecvRemoteFileIndex();
	void RecvRemoteFileOffset();
	void WantRemoteFilePESPacket();
	void RecvRemoteFilePESPacket();
	bool GetRemoteFileInfo(S8 *pchFileName);
	bool GetRemoteFileIndex();
	bool MoveFirstPESPack();
	bool MoveFirstIPESPack();
	bool MoveNextPESPack();
	bool MoveNextIPESPack();
	bool MovePrePESPack();
	bool MovePreIPESPack();
	bool MoveTo(UNS64 Time);
	bool  IsEof();
	bool  IsFirstPESPack() {return (m_CurIndexItemNum == 0);}
	UNS32 GetPESLength();
	UNS32 GetChnID();
	S8* GetBuf();
	UNS32 GetPTS();
	UNS32 GetAVType();
	UNS32 GetVopType();
	UNS64 GetStartTime();
	UNS32 GetCurLTime();
	UNS32 GetCurDate();
	PS_FILE_HDR* GetpsFileHead(){ return &m_psFileHead;}
	bool GetRecvPESPacket();//接收数据包
	UNS32 GetIndexItemNumByFileoffset(std64_net_t offset);
	UNS32 GetIndexItemNumByTime(UNS64 Time);
	void SetEof(bool bIsEof);

	CMyMutex m_hRecvPESPacketMutex;
	PS_FILE_HDR m_psFileHead;
	PS_INDEX_RECORD* m_psIndexRecord;

	CMyCriticalSection m_Section;
	CPESPacketHead m_pesPackHead;
	UNS32 m_CurIndexItemNum;
	UNS32 m_CurPlayChn;

	DVRSocket *m_DVRSocket;
	S8 m_PESBuf[MAX_PLAYBACK_BUF_SIZE];
	bool m_RecvPESPacket;
	bool m_isNormalplay;

public:
	bool m_isMoveTo;
	bool m_bTimeNeedReset;
	bool m_bRecvFileOffset;
	bool m_bIsEof;
};

#endif // !defined(AFX_REMOTEPSFILE_H__F2E1894B_F1F9_48E6_90D5_915D67743010__INCLUDED_)

