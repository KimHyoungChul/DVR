#pragma once
#ifndef _MYTHREAD_H_
#define _MYTHREAD_H_

#include "Global.h"

typedef void* (*tc_thread_func)(void*);

class MyThread
{
public:
	MyThread();
	~MyThread();

	bool CreateAThread(LPThreadStartAddr startAddr, LPVOID lpvoid, UNSL32 size = 0);
	void ExitAThread();
	bool ThreadIsNULL();

private:
#ifdef WIN32
	HANDLE m_threadHandle;
#else
	pthread_t m_threadID;
	pthread_attr_t m_threadAttr;
#endif
};

#endif
