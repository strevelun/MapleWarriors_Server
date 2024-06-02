#include "CSLock.h"

CSLock::CSLock()
{
	InitializeCriticalSection(&m_lock);
}

CSLock::~CSLock()
{
	DeleteCriticalSection(&m_lock);
}

void CSLock::Enter()
{
	EnterCriticalSection(&m_lock);
}

void CSLock::Leave()
{
	LeaveCriticalSection(&m_lock);
}
