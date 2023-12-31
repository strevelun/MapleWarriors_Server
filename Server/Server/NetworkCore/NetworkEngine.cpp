#include "NetworkEngine.h"
#include "../Defines.h"

NetworkEngine::NetworkEngine() 
{
}

NetworkEngine::~NetworkEngine()
{
}

bool NetworkEngine::Init()
{
	HANDLE hCPObject = m_iocp.CreateIOCP();
	if (hCPObject == nullptr) 
	{
		printf("IOCP Creation failed\n");
		return false;
	}

	SYSTEM_INFO		si;
	GetSystemInfo(&si);
	if (!m_iocp.CreateWorkerThread(si.dwNumberOfProcessors * 2)) return false;
	
	return true;
}

void NetworkEngine::OnConnected(Connection* _pConn)
{
	if (!m_iocp.ConnectIOCP(_pConn->GetSocket(), _pConn) || !_pConn->RecvWSA())
	{
		printf("IOCP Connection Failed");
		ConnectionManager::GetInst()->DeleteConnection(_pConn->GetId());
		return;
	}
}
