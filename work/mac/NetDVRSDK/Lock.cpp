#include "pthread.h"
#include "macro.h"
#include "Lock.h"

/* CMyMutexª•≥‚¡ø */
CMyMutex::CMyMutex()
{
#ifndef WIN32
	//pthread_mutexattr_settype(&m_tAttr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m_tMutex, NULL);
#else
	m_hMyMutex = CreateMutex(0, false, 0);
#endif
}

CMyMutex::~CMyMutex()
{
#ifndef WIN32
	//pthread_mutexattr_destroy(&m_tAttr);
	S32 iResult = pthread_mutex_destroy(&m_tMutex);
	if (iResult)
		printf("pthread_mutex_destroy error\n");
#else
	if (m_hMyMutex != NULL)
	{
		CloseHandle(m_hMyMutex);
		m_hMyMutex = NULL;
	}
#endif
}

void CMyMutex::Lock()
{
#ifndef WIN32
	S32 iResult = pthread_mutex_lock(&m_tMutex);
	if (iResult)
		printf("pthread_mutex_lock error\n");
#else
	WaitForSingleObject(m_hMyMutex, INFINITE);
#endif
}

bool CMyMutex::TryLock()
{
#ifndef WIN32
	S32 iResult = pthread_mutex_trylock(&m_tMutex);
	if (iResult)
	{
		/*if (iResult == EBUSY)*/
		printf("pthread_mutex_trylock error\n");
		return false;
		
	}
#endif
	return true;
}

void CMyMutex::Unlock()
{
#ifndef WIN32
	S32 iResult = pthread_mutex_unlock(&m_tMutex);
	if (iResult)
		printf("pthread_mutex_unlock error\n");
#else
	ReleaseMutex(m_hMyMutex);
#endif
}

/* CMyCriticalSection¡ŸΩÁ«¯ */
CMyCriticalSection::CMyCriticalSection()
{
#ifndef WIN32
	//pthread_mutexattr_settype(&m_tAttr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m_tMutex, NULL);
#else
	InitializeCriticalSection(&m_CriticalSection);
#endif
}

CMyCriticalSection::~CMyCriticalSection()
{
#ifndef WIN32
	//pthread_mutexattr_destroy(&m_tAttr);
	S32 iResult = pthread_mutex_destroy(&m_tMutex);
	if (iResult)
		printf("pthread_mutex_destroy error\n");
#else
	DeleteCriticalSection(&m_CriticalSection);
#endif
}

void CMyCriticalSection::Lock()
{
#ifndef WIN32
	S32 iResult = pthread_mutex_lock(&m_tMutex);
	if (iResult)
		printf("pthread_mutex_lock error\n");
#else
	EnterCriticalSection(&m_CriticalSection);
#endif
}

void CMyCriticalSection::Unlock()
{
#ifndef WIN32
	S32 iResult = pthread_mutex_unlock(&m_tMutex);
	if (iResult)
		printf("pthread_mutex_unlock error\n");
#else
	LeaveCriticalSection(&m_CriticalSection);
#endif
}

bool CMyCriticalSection::TryLock()
{
#ifndef WIN32
	S32 iResult = pthread_mutex_trylock(&m_tMutex);
	if (iResult)
	{
		/*if (iResult == EBUSY)*/
		printf("pthread_mutex_trylock error\n");
		return false;
		
	}
#endif
	return true;
}
