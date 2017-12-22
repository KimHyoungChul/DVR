#pragma once
#ifndef _STREAMFIFO_H_
#define _STREAMFIFO_H_

#include "Lock.h"

class StreamFifo
{
public:
	StreamFifo(S32 size);
	~StreamFifo();

	UNS32 PushFrame(S8 *pData, UNS32 length);
	UNS32 PopFrame(S8 *buf, UNS32 max);
	void Reset();

private:
	UNS32 m_totalData;
	UNS32 m_readPointer;
	UNS32 m_writePointer;
	UNS32 m_maxSize;
	CMyCriticalSection m_section;
	S8 *m_buf;
};

#endif
