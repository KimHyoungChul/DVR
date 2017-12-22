#pragma once
#ifndef _MYEVENT_H_
#define _MYEVENT_H_

#include "Global.h"

class MyEvent
{
public:
	MyEvent();
	~MyEvent();

	void CreateUnsignaledEvent();
	void SetSignaledEvent();
	void WaitForInfinite();
	UNSL32 WaitForMilliSeconds(UNS32 milliSeconds);

private:
#ifdef WIN32
	HANDLE m_hSignalEvent;
#else
	sem_t m_sem;
	struct timespec delay;
#endif
};

#endif
