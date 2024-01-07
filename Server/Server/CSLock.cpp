#include "CSLock.h"

CSLock::CSLock(bool _bTest) :
	m_bTest(_bTest), m_signal{}, m_waitThreads(0)
{
	InitializeCriticalSection(&m_lock);
	if(_bTest) m_signal = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CSLock::~CSLock()
{
	DeleteCriticalSection(&m_lock);
}

void CSLock::Enter()
{
	if (m_bTest)
	{
		wprintf(L"Enter\n");
		InterlockedIncrement(&m_waitThreads);
		if (m_waitThreads == TEST_LOCK_THREADS_MAX)		SetEvent(m_signal);
		else						WaitForSingleObject(m_signal, INFINITE);

		printf("flush!\n");
	}
	EnterCriticalSection(&m_lock);
}

void CSLock::Leave()
{
	LeaveCriticalSection(&m_lock);
	if (m_bTest)
	{
		wprintf(L"Left\n");
		--m_waitThreads;
		if(m_waitThreads == 0)		ResetEvent(&m_signal);
	}
}
