#include "SRWLock.h"

#pragma warning(disable : 26110)

SRWLock::SRWLock()
{
	InitializeSRWLock(&m_lock);
}

SRWLock::~SRWLock()
{
}

void SRWLock::Lock(eLockType _eType)
{
	switch (_eType)
	{
	case eLockType::Reader:
		AcquireSRWLockShared(&m_lock);
		break;
	case eLockType::Writer:
		AcquireSRWLockExclusive(&m_lock);
		break;
	}
}

void SRWLock::UnLock(eLockType _eType)
{
	switch (_eType)
	{
	case eLockType::Reader:
		ReleaseSRWLockShared(&m_lock);
		break;
	case eLockType::Writer:
		ReleaseSRWLockExclusive(&m_lock);
		break;
	}
}
