#pragma once

#include "../Defines.h"
#include "ConnectionManager.h"

class IOCP
{
private:
	HANDLE* m_arrThreadHandle;
	uint32 m_numOfThread;
	HANDLE m_hCPObject;
	bool m_workerRunning;

public:
	IOCP();
	~IOCP();

	HANDLE CreateIOCP();
	bool AssociateIOCP(Connection* _completionKey);
	bool CreateWorkerThread(uint32 _numOfThread );

private:
	static uint32 __stdcall CallWorkerThread(void* _pArgs);
	void Worker();
};

