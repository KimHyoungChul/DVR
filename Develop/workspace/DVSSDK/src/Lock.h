#pragma once
#ifndef LOCK_H_
#define LOCK_H_

#include "Global.h"

class CMyMutex
{
public:
	CMyMutex();
	~CMyMutex();

	void Lock();
	void Unlock();
	bool TryLock();

private:
#ifndef WIN32
	pthread_mutex_t m_tMutex;
	pthread_mutexattr_t m_tAttr;
#else
	HANDLE m_hMyMutex;
#endif
};

class CMyCriticalSection
{
public:
	CMyCriticalSection();
	~CMyCriticalSection();

	void Lock();
	void Unlock();
	bool TryLock();

private:
#ifndef WIN32
	pthread_mutex_t m_tMutex;
	pthread_mutexattr_t m_tAttr;
#else
	CRITICAL_SECTION m_CriticalSection;
#endif
};

#endif
