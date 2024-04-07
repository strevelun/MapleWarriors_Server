#include "ServerApp.h"

#include "NetworkCore/ConnectionManager.h"

ServerApp::ServerApp() :
	m_bIsRunning(true)
{

}

ServerApp::~ServerApp()
{
}

bool ServerApp::Init(const int8* _ip, uint16 _port, int32 _backlog)
{
	setlocale(LC_ALL, "Korean");

	WSADATA  wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;
	if (!m_engine.Init()) return false;
	if (!m_acceptor.Start(_ip, _port)) return false;

	m_acceptor.Bind();
	m_acceptor.Listen(_backlog);
	
	return true;
}

void ServerApp::Run()
{
	while (m_bIsRunning)
	{
		tAcceptedClient* acceptedClient = m_acceptor.Accept();
		if (acceptedClient->clientSocket == INVALID_SOCKET)
		{
			printf("socket 수락 실패 : %d\n", WSAGetLastError());
			continue;
		}

		Connection* pConn = ConnectionManager::GetInst()->Create(acceptedClient);

		printf("id[%d], socket[%d], IP[%s], Port[%d]		연결됨			(현재 접속자 수 : %d)\n", 
			pConn->GetId(), (int32)acceptedClient->clientSocket, acceptedClient->ipAddr, acceptedClient->port, ConnectionManager::GetInst()->GetCount());

		m_engine.OnConnected(pConn);
	}
}

void ServerApp::Shutdown()
{
	WSACleanup();
}
