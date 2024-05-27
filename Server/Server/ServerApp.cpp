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
	::setlocale(LC_ALL, "Korean");

	WSADATA  wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)	return false;
	if (!m_engine.Init())							return false;
	if (!m_acceptor.Start(_ip, _port))				return false;
	if (!m_acceptor.Bind())							return false;
	if (!m_acceptor.Listen(_backlog))				return false;
	
	return true;
}

void ServerApp::Run()
{
	tAcceptedClient* pAcceptedClient = nullptr;
	Connection* pConn = nullptr;

	while (m_bIsRunning)
	{
		pAcceptedClient = m_acceptor.Accept();
		if (pAcceptedClient->clientSocket == INVALID_SOCKET)
		{
			printf("socket ���� ���� : %d\n", ::WSAGetLastError());
			continue;
		}

		pConn = m_engine.OnConnected(pAcceptedClient);
		if (!pConn)
		{
			printf("[%d], IP[%s]		�������			(���� ������ �� : %d)\n",
				(int32)pAcceptedClient->clientSocket, pAcceptedClient->ipAddr, ConnectionManager::GetInst()->GetCount());
			continue;
		}

		printf("id[%u], socket[%d], IP[%s]		�����			(���� ������ �� : %d)\n", 
			pConn->GetId(), (int32)pAcceptedClient->clientSocket, pAcceptedClient->ipAddr, ConnectionManager::GetInst()->GetCount());
	}
}

void ServerApp::Shutdown()
{
	m_bIsRunning = false;
	::WSACleanup();
}
