#pragma once
#ifndef _STREAMSOCKET_H_
#define _STREAMSOCKET_H_

#include "StreamFifo.h"
#include "Thread.h"

class StreamSocket
{
public:
	StreamSocket();
	~StreamSocket();

	S32 Init(UNSL32 ip, UNS16 port, StreamFifo *pFifo);
	void UnInit();
	S32 SendData(S8* pData, S32 len);
	S32 ReadData(S8 *pData, S32 len);
	bool Start();

private:
	S32 BuildSocket(UNSL32 ip, UNS16 port);
	MyThread m_hDataRcv;

public:
	StreamFifo	*m_fifo;
	SOCKET	m_dataSocket;
	bool	m_bExit;
};

#endif
