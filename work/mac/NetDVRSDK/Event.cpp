#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/sem.h>
#include "Event.h"
#include "macro.h"
#include <assert.h>


MyEvent::MyEvent()
{
#ifdef WIN32
	m_hSignalEvent = NULL;
#else
	m_sem = NULL; 
    memset(m_name,0x0,sizeof(m_name));
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
	//sem_destroy(m_sem);
    DestroySemaphore(m_sem);
    ClearSemaphore(m_name);
#endif
}

void MyEvent::CreateUnsignaledEvent( const char * inName, const int inStartingCount )
{
#ifdef WIN32
	m_hSignalEvent = CreateEvent(0, false, false, 0);
#else
	//sem_init(&m_sem,//handle to the event semaphore
	//	     0,//not shared
	//	     1);//initially set to non signaled state
    
    memcpy(m_name,inName,sizeof(m_name));
    m_sem = CreateSemaphore(inName,inStartingCount ); 
#endif
}

void MyEvent::SetSignaledEvent()
{
#ifdef WIN32
	SetEvent(m_hSignalEvent);
#else
	sem_post(m_sem);
#endif
}

void MyEvent::WaitForInfinite()
{
#ifdef WIN32
	WaitForSingleObject(m_hSignalEvent, INFINITE);
#else
	sem_wait(m_sem);
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
		delay.tv_sec = 5;
		delay.tv_nsec = 1000000;/* 1 milli sec */

		//Wait for the event be signaled
		S32 retCode = sem_wait(m_sem);//sem_trywait(m_sem);
        printf("WaitForSingleObject sem_trywait %d\n",retCode);
		if (!retCode)
		{
			/* Event is signaled */
			return WAIT_OBJECT_0;
		}else
		{
			/* check whether somebody else has the mutex */
			if (retCode == EPERM )
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

//add by wuzy 
sem_t * MyEvent::CreateSemaphore( const char * inName, const int inStartingCount )
{
	sem_t * semaphore = sem_open( inName, O_CREAT, 0644, inStartingCount ); //
	if( semaphore == SEM_FAILED )
	{
		switch( errno )
		{ 
            case EEXIST: 
                printf( "Semaphore with name '%s' already exists.\n", inName );
                break; 
            default: printf( "Unhandled error: %d.\n", errno ); 
                break;
		} //
        
		assert(false);
		return SEM_FAILED;
	} //
    
	return semaphore;
}//

bool MyEvent::DestroySemaphore( sem_t * inSemaphore )
{
	int retErr = sem_close( inSemaphore ); //
	if( retErr == -1 )
	{//
		switch( errno )
		{ 
            case EINVAL: 
                printf( "inSemaphore is not a valid sem_t object." );
                break; 
            default: 
                printf( "Unhandled error: %d.\n", errno ); 
                break;
		} //
		
		assert(false);
		return false;
	} //
	
	return true;
}//

void MyEvent::SignalSemaphore( sem_t * inSemaphore )
{
	sem_post( inSemaphore );
}//

void MyEvent::WaitSemaphore()
{
	sem_wait( m_sem );
}//

bool MyEvent::TryWaitSemaphore( sem_t * inSemaphore )
{
	int retErr = sem_trywait( inSemaphore ); //
	if( retErr == -1 )
	{//
		if( errno != EAGAIN )
		{
			printf( "Unhandled error: %d\n", errno );
			assert( false );
		} //
		
		return false;
	} //
	
	return true;
}

bool MyEvent::ClearSemaphore( const char * inName)
{
	int retErr = sem_unlink(inName); 
	if (retErr == -1) 
	{
		return false; 
	}
	
	return true;
}
