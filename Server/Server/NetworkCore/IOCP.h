#pragma once

#include "../Defines.h"
#include "ConnectionManager.h"

class IOCP
{
private:
	HANDLE* m_arrThreadHandle;
	HANDLE m_hCPObject;

public:
	IOCP();
	~IOCP();

	HANDLE CreateIOCP();
	bool AssociateIOCP(SOCKET _socket, Connection* _completionKey);
	bool CreateWorkerThread(uint32 _numOfThread);

private:
	static unsigned int __stdcall Worker(void* _pArgs);
};

