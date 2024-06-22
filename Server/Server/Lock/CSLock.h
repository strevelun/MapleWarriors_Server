#pragma once

#include "../Defines.h"

class CSLock 
{
private:
	CRITICAL_SECTION			m_lock;

public:
	CSLock();
	~CSLock();

	void Enter();
	void Leave();
};

