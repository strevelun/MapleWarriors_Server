#include "NetworkEngine.h"
#include "UDPHandler.h"
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

	UDPHandler* pHandlerInst = UDPHandler::GetInst();

	SYSTEM_INFO		si;
	::GetSystemInfo(&si);
	if (!m_iocp.CreateWorkerThread(si.dwNumberOfProcessors * 2))	return false;
	if (!pHandlerInst->Init(si.dwNumberOfProcessors * 2))			return false;
	if (!pHandlerInst->Bind())										return false;
	if (!pHandlerInst->RecvReady())									return false;
	if (!m_iocp.AssociateIOCP(pHandlerInst->GetSocket()))			return false;

	return true;
}

Connection* NetworkEngine::OnConnected(tAcceptedClient* _pAcceptedClient)
{
	Connection* pConn = ConnectionManager::GetInst()->Create(_pAcceptedClient);

	if (!m_iocp.AssociateIOCP(pConn))
	{
		printf("IOCP Association Failed");
		ConnectionManager::GetInst()->Delete(pConn->GetId());
		return nullptr;
	}
	
	pConn->RecvWSA();

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::ConnectionID)
		.Add<uint32>(pConn->GetId());
	pConn->Send(pkt);

	return pConn;
}
