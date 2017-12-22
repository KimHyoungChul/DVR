// PSFile.h: interface for the CPSFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PSFILE_H__373D92B7_8A33_486B_BB16_FE54AED9CD84__INCLUDED_)
#define AFX_PSFILE_H__373D92B7_8A33_486B_BB16_FE54AED9CD84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PESPacketHead.h"

#define EXE_FILE_MAGIC	"LianYouShiChuang"     //the magic word between player.exe and backup file header
#define PSFILE_KEY		0x22

class CPSFile  
{
public:
	CPSFile();
	virtual ~CPSFile();

	bool OpenPSFile(S8 *filename);
	void ClosePSFile();
	bool ParsePSFile();

	UNS8 GetBiAudio(UNS8 chnnl);
	UNS32 GetPSFileID() { return m_psFileHead->FileID; }

	bool MoveFirstPESPack();
	bool MoveFirstIPESPack();
	bool MoveNextPESPack();
	bool MoveNextIPESPack();
	bool MovePreIPESPack();
	bool MoveToEx(UNS64 Time);
	void MovePsfileOffset(UNS32 index);
	bool IsEof() {return (m_CurIndexItemNum >= m_psFileHead->TotalIndex-1);}
	bool IsFirstPESPack() {return (m_CurIndexItemNum == 0);}

	UNS32 GetPESLength();
	UNS32 GetChnID();
	S8* GetBuf();
	UNS32 GetPTS();
	UNS32 GetAVType();
	UNS32 GetVopType();
	UNS32 GetCurTime();
	UNS32 GetCurDate();

	PS_FILE_HDR* GetpsFileHead(){ return m_psFileHead;}
	void ReadPSFileHead(PS_FILE_HDR* psFileHead);
	bool ReadPESPack();
	UNS32 GetIndexItemNumByTimeEx(UNS64 Time);

	S8 m_PSFileName[MAX_PATH];
	S8 m_pBuf[MAX_PLAYBACK_BUF_SIZE];
	FILE *m_pLocalFile;
	PS_FILE_HDR* m_psFileHead;
	PS_INDEX_RECORD* m_psIndexRecord;

	CMyCriticalSection m_Section;
	CPESPacketHead m_pesPackHead;
	UNS32 m_PSFileStartOffset;//文件起始偏移
	UNS32 m_CurIndexItemNum;
	UNS64 m_PsFileOffset;
	UNS8 m_biAudio[MAX_SOLO_CHANNEL_16];
	bool  FindIFrame[MAX_SOLO_CHANNEL_16];
};

#endif // !defined(AFX_PSFILE_H__373D92B7_8A33_486B_BB16_FE54AED9CD84__INCLUDED_)

