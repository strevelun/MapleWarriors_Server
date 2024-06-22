#pragma once

#include "../Defines.h"

enum class eLockType
{
	None,
	Reader,
	Writer
};

class SRWLock
{
private:
	SRWLOCK				m_lock;

public:
	SRWLock();
	~SRWLock();

	void Lock(eLockType _eType);
	void UnLock(eLockType _eType);
};

