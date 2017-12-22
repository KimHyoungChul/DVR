#pragma once
#ifndef _SPEECHSOCKET_H_
#define _SPEECHSOCKET_H_

#include "SpeechFifo.h"
#include "Thread.h"

class SpeechSocket
{
public:
	SpeechSocket();
	~SpeechSocket();

	S32 Init(const S8 *ip, UNS16 port, SpeechFifo *pFifo); //mode = 1, 被动模式， mode = 2, 主动模式;
	void UnInit();
	S32 SendData(S8* pData, S32 len);
	S32 ReadData(S8 *pData, S32 len);

private:	
	S32 BuildSocket(const S8* ip, UNS16 port);
	MyThread m_hDataRcv;

public:
	SOCKET m_dataSocket;
	bool   m_bExit;
	SpeechFifo *m_fifo;
};

#endif
