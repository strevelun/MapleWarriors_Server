#include "ServerApp.h"

#include "NetworkCore/ConnectionManager.h"

ServerApp::ServerApp() :
	m_bIsRunning(true)
{

}

ServerApp::~ServerApp()
{
}

bool ServerApp::Init(const char* _ip, unsigned short _port, int _backlog)
{
	setlocale(LC_ALL, "Korean");

	WSADATA  wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;
	if (!m_engine.Init()) return false;
	if (!m_acceptor.Start(_ip, _port, _backlog)) return false;

	return true;
}

void ServerApp::Run()
{
	while (m_bIsRunning)
	{
		SOCKET clientSocket = m_acceptor.Accept();
		if (clientSocket == INVALID_SOCKET)
		{
			printf("socket ���� ���� : %d\n", WSAGetLastError());
			continue;
		}

		Connection* pConn = ConnectionManager::GetInst()->Create(clientSocket);

		printf("%d ����� [�������Ӽ� : %d]\n", (int)clientSocket, ConnectionManager::GetInst()->GetCount());


		m_engine.OnConnected(pConn);
	}
}

void ServerApp::Shutdown()
{
	WSACleanup();
}
