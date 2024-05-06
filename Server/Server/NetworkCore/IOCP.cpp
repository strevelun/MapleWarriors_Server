#include "IOCP.h"
#include "../User/UserManager.h"
//#include <chrono>

IOCP::IOCP() :
    m_hCPObject(nullptr),
	m_arrThreadHandle(nullptr)
{

}

IOCP::~IOCP()
{
    if(m_hCPObject) CloseHandle(m_hCPObject);
	if (m_arrThreadHandle) delete[] m_arrThreadHandle;
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
    return m_hCPObject == h;
}

bool IOCP::CreateWorkerThread(uint32 _numOfThread)
{
	m_arrThreadHandle = new HANDLE[_numOfThread];

	for (uint32 i = 0; i < _numOfThread; ++i)
	{
		m_arrThreadHandle[i] = (HANDLE)_beginthreadex(nullptr, 0, &Worker, (void*)m_hCPObject, 0, nullptr);
		if (m_arrThreadHandle[i] == 0)
		{
			printf("thread 생성 실패\n");
			return false;
		}
	}
	return true;
}

uint32 __stdcall IOCP::Worker(void* _pArgs)
{
	HANDLE hIOCP = (HANDLE)_pArgs;
	DWORD			bytesTransferred = 0;
	WSAOVERLAPPED* pOverlapped = nullptr;
	Connection* pConn = nullptr;

	while (1)
	{
		bool result = GetQueuedCompletionStatus(hIOCP, &bytesTransferred, (PULONG_PTR)&pConn, (LPOVERLAPPED*)&pOverlapped, INFINITE);
		if (!result)
		{
			User* pUser = UserManager::GetInst()->FindConnectedUser(pConn->GetId());
			if (pUser) pUser->Leave();
			UserManager::GetInst()->Disconnect(pConn->GetId());
			ConnectionManager::GetInst()->Delete(pConn->GetId());
			continue;
		}

		if (bytesTransferred == 0) 
		{
			User* pUser = UserManager::GetInst()->FindConnectedUser(pConn->GetId());
			if (pUser) pUser->Leave();
			UserManager::GetInst()->Disconnect(pConn->GetId());
			ConnectionManager::GetInst()->Delete(pConn->GetId());
			continue;
		}

		pConn->OnRecv(bytesTransferred);

		if (!pConn->RecvWSA())
		{
			User* pUser = UserManager::GetInst()->FindConnectedUser(pConn->GetId());
			if (pUser) pUser->Leave();
			UserManager::GetInst()->Disconnect(pConn->GetId());
			ConnectionManager::GetInst()->Delete(pConn->GetId());
			continue;
		}
	}
	return 0;
}