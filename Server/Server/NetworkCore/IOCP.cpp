#include "IOCP.h"
#include "../User/UserManager.h"

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

bool IOCP::AssociateIOCP(SOCKET _socket, Connection* _completionKey)
{
    if (m_hCPObject == nullptr) return false;

    HANDLE h = CreateIoCompletionPort((HANDLE)_socket, m_hCPObject, (ULONG_PTR)_completionKey, 0);
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

unsigned int __stdcall IOCP::Worker(void* _pArgs)
{
	HANDLE hIOCP = (HANDLE)_pArgs;
	DWORD			bytesTransferred = 0;
	WSAOVERLAPPED* pOverlapped = nullptr;
	Connection* pConn = nullptr;

	while (1)
	{
		//if(pConn)
		//	("[%d] GetQUeuedCS 호출 전\n", (int)pConn->GetSocket());
		bool result = GetQueuedCompletionStatus(hIOCP, &bytesTransferred, (PULONG_PTR)&pConn, (LPOVERLAPPED*)&pOverlapped, INFINITE);
		if (!result)
		{
			printf("false returned : %d\n", WSAGetLastError()); 
	
			User* pUser = UserManager::GetInst()->FindConnectedUser(pConn->GetId());
			if (pUser) pUser->Leave();
			ConnectionManager::GetInst()->Delete(pConn->GetId());
			continue;
		}
		//printf("[%d] GetQUeuedCS 호출 후\n", (int)pConn->GetSocket());

		if (bytesTransferred == 0)
		{
			//printf("bytesTransferred : 0 (disconnect)\n");
			User* pUser = UserManager::GetInst()->FindConnectedUser(pConn->GetId());
			if (pUser) pUser->Leave();
			ConnectionManager::GetInst()->Delete(pConn->GetId());
			continue;
		}

		//printf("%d, ", (int)bytesTransferred);

		pConn->OnRecv(bytesTransferred);

		if (pConn->GonnaBeDeleted())
		{
			User* pUser = UserManager::GetInst()->FindConnectedUser(pConn->GetId());
			if(pUser) pUser->Leave();
			ConnectionManager::GetInst()->Delete(pConn->GetId());
			continue;
		}

		if (!pConn->RecvWSA()) 
		{
			User* pUser = UserManager::GetInst()->FindConnectedUser(pConn->GetId());
			if (pUser) pUser->Leave();
			ConnectionManager::GetInst()->Delete(pConn->GetId());
			continue;
		}
	}
	return 0;
}

