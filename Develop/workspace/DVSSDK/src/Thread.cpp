#include "Thread.h"

MyThread::MyThread()
{
#ifdef WIN32
	m_threadHandle = NULL;
#else
	m_threadID = 0;
	pthread_attr_init(&m_threadAttr);
#endif
}

MyThread::~MyThread()
{
	if (!ThreadIsNULL())
	{
		ExitAThread();
	}
}

bool MyThread::CreateAThread(LPThreadStartAddr startAddr, LPVOID lpvoid, UNSL32 size /* = 0 */)
{
#ifdef WIN32
	m_threadHandle = CreateThread(0, size, startAddr, lpvoid, 0, 0);
	if (m_threadHandle == NULL)
	{
		return false;
	}
#else
	S32 err = 0;
	if (size != 0)
	{
		pthread_attr_setstacksize(&m_threadAttr, size);
		err = pthread_create(&m_threadID, &m_threadAttr, (tc_thread_func)startAddr, 0);
	}else
	{
		err = pthread_create(&m_threadID, 0, (tc_thread_func)startAddr, 0);
	}
	if (err != 0)
	{
		return false;
	}
#endif

	return true;
}

void MyThread::ExitAThread()
{
#ifdef WIN32
	if (m_threadHandle != NULL)
	{
		WaitForSingleObject(m_threadHandle, INFINITE);
		CloseHandle(m_threadHandle);
		m_threadHandle = NULL;
	}
#else
	if (m_threadID != 0)
	{
		pthread_attr_destroy(&m_threadAttr);
		HANDLE exitCode;
		pthread_join(m_threadID, &exitCode);
		m_threadID = 0;
	}
#endif
}

bool MyThread::ThreadIsNULL()
{
#ifdef WIN32
	return (m_threadHandle == NULL);
#else
	return (m_threadID == 0);
#endif
}