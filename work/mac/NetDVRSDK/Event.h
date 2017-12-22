#pragma once
#ifndef _MYEVENT_H_
#define _MYEVENT_H_

#include "Global.h"

class MyEvent
{
public:
	MyEvent();
	~MyEvent();

	void CreateUnsignaledEvent( const char * inName, const int inStartingCount );
	void SetSignaledEvent();
	void WaitForInfinite();
	UNSL32 WaitForMilliSeconds(UNS32 milliSeconds);
    
    //add by wuzy 
    sem_t * CreateSemaphore( const char * inName, const int inStartingCount );
    bool DestroySemaphore( sem_t * inSemaphore );
    void SignalSemaphore( sem_t * inSemaphore );
    void WaitSemaphore();
    bool TryWaitSemaphore( sem_t * inSemaphore );
    bool ClearSemaphore( const char * inName);

private:
#ifdef WIN32
	HANDLE m_hSignalEvent;
#else
	sem_t* m_sem;
	struct timespec delay;
    char m_name[100];
#endif
};

#endif
