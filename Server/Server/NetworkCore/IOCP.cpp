#include "IOCP.h"
#include "../User/UserManager.h"
#include "UDPHandler.h"

IOCP::IOCP() :
    m_hCPObject(nullptr),
	m_numOfThread(0),
	m_arrThreadHandle(nullptr),
	m_workerRunning(true)
{

}

IOCP::~IOCP()
{
	m_workerRunning = false;

	for (uint32 i = 0; i < m_numOfThread; ++i)
	{
		PostQueuedCompletionStatus(m_hCPObject, 0, 0, NULL);
	}

	if (m_arrThreadHandle)
	{
		for (uint32 i = 0; i < m_numOfThread; ++i)
		{
			WaitForSingleObject(m_arrThreadHandle[i], INFINITE);
			::CloseHandle(m_arrThreadHandle[i]);
		}
		m_numOfThread = 0;
		delete[] m_arrThreadHandle;
	}
	if (m_hCPObject) ::CloseHandle(m_hCPObject);
}

HANDLE IOCP::CreateIOCP()
{
	m_hCPObject = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    return m_hCPObject;
}

bool IOCP::AssociateIOCP(SOCKET _socket, uint32 _completionKey)
{
	if (m_hCPObject == nullptr) return false;

	HANDLE h = ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(_socket), m_hCPObject, static_cast<ULONG_PTR>(_completionKey), 0);
	if (h == nullptr) return false;

	return true;
}

bool IOCP::AssociateIOCP(SOCKET _socket)
{
	if (m_hCPObject == nullptr) return false;

	HANDLE h = ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(_socket), m_hCPObject, 0, 0);
	if (h == nullptr) return false;

	return true;
}

bool IOCP::CreateWorkerThread(uint32 _numOfThread)
{
	m_numOfThread = _numOfThread;
	m_arrThreadHandle = new HANDLE[_numOfThread];

	for (uint32 i = 0; i < _numOfThread; ++i)
	{
		m_arrThreadHandle[i] = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, &CallWorkerThread, reinterpret_cast<void*>(this), 0, nullptr));
		if (m_arrThreadHandle[i] == 0)
		{
			printf("thread ���� ����\n");
			return false;
		}
	}
	return true;
}

uint32 __stdcall IOCP::CallWorkerThread(void* _pArgs)
{
	IOCP* pIocp = static_cast<IOCP*>(_pArgs);
	pIocp->Worker();
	return 0;
}

void IOCP::Worker()
{
	DWORD			bytesTransferred = 0;
	tWSAOVERLAPPED_EX* pOverlapped = nullptr;
	uint32 connID = 0;
	bool result;

	while (1)
	{
		result = ::GetQueuedCompletionStatus(m_hCPObject, &bytesTransferred, reinterpret_cast<PULONG_PTR>(&connID), reinterpret_cast<LPOVERLAPPED*>(&pOverlapped), INFINITE);

		if (!m_workerRunning) break;

		if (!result || bytesTransferred == 0)
		{
			UserManager::GetInst()->Disconnect(connID); 
			ConnectionManager::GetInst()->Release(connID);
			continue;
		}

		switch (pOverlapped->connType)
		{
		case eConnType::TCP:
		{
			Connection* pConn = ConnectionManager::GetInst()->Get(connID);
			if (pConn)	pConn->OnRecv(bytesTransferred);
			ConnectionManager::GetInst()->Release(connID);
			break;
		}
		case eConnType::UDP:
			UDPHandler::GetInst()->OnRecv(pOverlapped->udpIdx);
			break;
		}
	}
}
