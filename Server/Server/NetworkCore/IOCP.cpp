#include "IOCP.h"
#include "../User/UserManager.h"
//#include <chrono>

IOCP::IOCP() :
    m_hCPObject(nullptr),
	m_numOfThread(0),
	m_arrThreadHandle(nullptr),
	m_workerRunning(true)
{

}

IOCP::~IOCP()
{
	if (m_arrThreadHandle)
	{
		for (uint32 i = 0; i < m_numOfThread; ++i)
		{
			CloseHandle(m_arrThreadHandle[i]);
		}
		m_numOfThread = 0;
		delete[] m_arrThreadHandle;
	}
    if(m_hCPObject) CloseHandle(m_hCPObject);
}

HANDLE IOCP::CreateIOCP()
{
	m_hCPObject = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    return m_hCPObject;
}

bool IOCP::AssociateIOCP(Connection* _completionKey)
{
	if (m_hCPObject == nullptr) return false;

	HANDLE h = CreateIoCompletionPort((HANDLE)_completionKey->GetSocket(), m_hCPObject, (ULONG_PTR)_completionKey, 0);
	if (h == nullptr) return false;

	return true;
}


bool IOCP::CreateWorkerThread(uint32 _numOfThread)
{
	m_numOfThread = _numOfThread;
	m_arrThreadHandle = new HANDLE[_numOfThread];

	for (uint32 i = 0; i < _numOfThread; ++i)
	{
		m_arrThreadHandle[i] = (HANDLE)_beginthreadex(nullptr, 0, &CallWorkerThread, (void*)this, 0, nullptr);
		if (m_arrThreadHandle[i] == 0)
		{
			printf("thread 생성 실패\n");
			return false;
		}
	}
	return true;
}

uint32 __stdcall IOCP::CallWorkerThread(void* _pArgs)
{
	IOCP* pIocp = (IOCP*)_pArgs;
	pIocp->Worker();
	return 0;
}

void IOCP::Worker()
{
	DWORD			bytesTransferred = 0;
	WSAOVERLAPPED* pOverlapped = nullptr;
	Connection* pConn = nullptr;
	bool result;

	while (1)
	{
		result = GetQueuedCompletionStatus(m_hCPObject, &bytesTransferred, (PULONG_PTR)&pConn, (LPOVERLAPPED*)&pOverlapped, INFINITE);

		if (!m_workerRunning) break;

		if (!result || bytesTransferred == 0)
		{
			UserManager::GetInst()->Disconnect(pConn->GetId());
			ConnectionManager::GetInst()->Delete(pConn->GetId());
			continue;
		}

		pConn->OnRecv(bytesTransferred);
	}
}
