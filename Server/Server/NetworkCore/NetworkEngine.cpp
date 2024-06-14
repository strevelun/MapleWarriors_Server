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

void NetworkEngine::OnConnected(tAcceptedClient* _pAcceptedClient)
{
	std::shared_ptr<Connection> conn = ConnectionManager::GetInst()->Create(_pAcceptedClient);

	if (!m_iocp.AssociateIOCP(conn->GetSocket(), conn->GetId()))
	{
		printf("IOCP Association Failed");
		ConnectionManager::GetInst()->Delete(conn->GetId());
		return;
	}
	
	conn->RecvWSA();

	Packet pkt;
	pkt
		.Add<PacketType>(static_cast<PacketType>(eServer::ConnectionID))
		.Add<uint32>(conn->GetId());
	conn->Send(pkt);

	if (!conn)
	{
		printf("[%d], IP[%s]		연결실패			(현재 접속자 수 : %d)\n", static_cast<int32>(_pAcceptedClient->clientSocket), _pAcceptedClient->ipAddr, ConnectionManager::GetInst()->GetCount());
	}
	else
	{
		printf("id[%u], socket[%d], IP[%s]		연결됨			(현재 접속자 수 : %d)\n",
			conn->GetId(), static_cast<int32>(_pAcceptedClient->clientSocket), _pAcceptedClient->ipAddr, ConnectionManager::GetInst()->GetCount());
	}
}
