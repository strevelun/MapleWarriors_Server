#pragma once

#include "Defines.h"
#include "Types.h"

class CSLock
{
private:
	CRITICAL_SECTION			m_lock;
	bool						m_bTest;
	HANDLE						m_signal;
	uint32						m_waitThreads;

public:
	CSLock(bool _bTest = false);
	~CSLock();

	void Enter();
	void Leave();
};

