#pragma once
#ifndef _SPEECHFIFO_H_
#define _SPEECHFIFO_H_

#include "Lock.h"

#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef struct tWAVEFORMATEX
{
    UNS16		wFormatTag;        /* format type */
    UNS16		nChannels;         /* number of channels (i.e. mono, stereo...) */
    UNSL32	nSamplesPerSec;    /* sample rate */
    UNSL32	nAvgBytesPerSec;   /* for buffer estimation */
    UNS16		nBlockAlign;       /* block size of data */
    UNS16		wBitsPerSample;    /* Number of bits per sample of mono data */
    UNS16		cbSize;            /* The count in bytes of the size of
                                    extra information (after cbSize) */

}WAVEFORMATEX;
#endif /* _WAVEFORMATEX_ */

class SpeechFifo
{
public:
	SpeechFifo(S32 size);
	~SpeechFifo();

	S32 AddData(S8 *pData, UNS32 length);
	S32 GetData(S8 *buf, UNS32 max);
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
