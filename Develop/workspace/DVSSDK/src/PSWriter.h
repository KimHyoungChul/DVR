#pragma once
#ifndef _PSWRITER_H_
#define _PSWRITER_H_

#include "common_dump.h"
#include "Lock.h"

class PSWriter
{
public:
	PSWriter();
	~PSWriter();

	/*create local record file or download backup file.*/
	bool Create(const S8 *pathname, void *pInfo, S32 infoSize, bool bReal = true, UNS32 chnnlMask = 0);
	bool PushPacket(S8 *packet, S32 len);
	void Destroy(bool bDelFile = false);

private:
	bool SaveIndex();

	PS_INDEX_RECORD	*m_pIndex;
	BlockInfo		m_blockInfo;
	FILE			*m_pRWFile;
	CMyCriticalSection m_indexSection;
	
	LARGE_INTEGER	m_offset;
	UNS32	m_indexNum;
	UNS32	m_blockStartTime;
	UNS32	m_totalIndex;
	UNS32	m_startTime;
	UNS32	m_lastTime;
	UNS32	m_exeFileLen;
	UNS32	m_iRecordChnnel;

	bool	m_bFirstWrite;
	bool	m_bRealStream;
	bool	m_bFirstPacket;
	bool	m_bEXEFileBk;

	S8		m_cFileName[MAX_PATH + 1];
};

#endif
