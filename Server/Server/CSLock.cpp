#include "CSLock.h"

CSLock::CSLock(bool _bTest) :
	m_bTest(_bTest), m_signal{}, m_waitThreads(0), m_entered(0)
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
	++m_entered;
	//printf("entered : %d, LockCount : %d, RecursionCount : %d\n", GetEnteredCount(), m_lock.LockCount, m_lock.RecursionCount);
	EnterCriticalSection(&m_lock); // --LockCount
	//printf("after entered : %d, LockCount : %d, RecursionCount : %d\n", GetEnteredCount(), m_lock.LockCount, m_lock.RecursionCount);
}

void CSLock::Leave()
{
	//printf("before left : %d, LockCount : %d, RecursionCount : %d\n", GetEnteredCount(), m_lock.LockCount, m_lock.RecursionCount);
	LeaveCriticalSection(&m_lock);
	//printf("after left : %d, LockCount : %d, RecursionCount : %d\n", GetEnteredCount(), m_lock.LockCount, m_lock.RecursionCount);
	--m_entered;
	if (m_bTest)
	{
		wprintf(L"Left\n");
		--m_waitThreads;
		if(m_waitThreads == 0)		ResetEvent(&m_signal);
	}
}
