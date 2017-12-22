#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/sem.h>
#include "Event.h"
#include "macro.h"
MyEvent::MyEvent()
{
#ifdef WIN32
	m_hSignalEvent = NULL;
#else
	//m_sem = NULL; 
#endif
}

MyEvent::~MyEvent()
{
#ifdef WIN32
	if (m_hSignalEvent != NULL)
	{
		CloseHandle(m_hSignalEvent);
		m_hSignalEvent = NULL;
	}
#else
	sem_destroy(&m_sem);
#endif
}

void MyEvent::CreateUnsignaledEvent()
{
#ifdef WIN32
	m_hSignalEvent = CreateEvent(0, false, false, 0);
#else
	sem_init(&m_sem,//handle to the event semaphore
		0,//not shared
		0);//initially set to non signaled state
#endif
}

void MyEvent::SetSignaledEvent()
{
#ifdef WIN32
	SetEvent(m_hSignalEvent);
#else
	sem_post(&m_sem);
#endif
}

void MyEvent::WaitForInfinite()
{
#ifdef WIN32
	WaitForSingleObject(m_hSignalEvent, INFINITE);
#else
	sem_wait(&m_sem);
#endif
}

UNSL32 MyEvent::WaitForMilliSeconds(UNS32 milliSeconds)
{
#ifdef WIN32
	return WaitForSingleObject(m_hSignalEvent, milliSeconds);
#else
	S32 timeout = 0;
	while (timeout < milliSeconds)
	{
		delay.tv_sec = 0;
		delay.tv_nsec = 1000000;/* 1 milli sec */

		//Wait for the event be signaled
		S32 retCode = sem_trywait(&m_sem);
		if (!retCode)
		{
			/* Event is signaled */
			return WAIT_OBJECT_0;
		}else
		{
			/* check whether somebody else has the mutex */
			//if (retCode == EPERM )
			if (retCode == EINVAL ||
				retCode == EINTR  ||
				retCode == EAGAIN)
			{
				/* sleep for delay time */
				nanosleep(&delay, NULL);
				timeout++;
			}else
			{
				/* error */
				printf("sem_trywait error\n");
				return WAIT_OBJECT_0 + 1;
			}
		}
	}

	return WAIT_OBJECT_0 + 2;
#endif
}
