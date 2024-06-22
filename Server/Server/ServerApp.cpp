#include "ServerApp.h"

#include "NetworkCore/ConnectionManager.h"
#include "DB/DBConnectionManager.h"

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

	if (!DBConnectionManager::GetInst()->Connect(L"Driver={MySQL ODBC 8.4 Unicode Driver};Server=127.0.0.1;Database=mydb;User=root;Password=root;CHARSET=utf8"))
	{
		printf("DBConnect ½ÇÆÐ\n");
		DBConnectionManager::GetInst()->Clear();
		return false;
	}

	return true;
}

void ServerApp::Run()
{
	tAcceptedClient* pAcceptedClient = nullptr;

	while (m_bIsRunning)
	{
		pAcceptedClient = m_acceptor.Accept();
		if (!pAcceptedClient) continue;

		m_engine.OnConnected(pAcceptedClient);
	}
}

void ServerApp::Shutdown()
{
	DBConnectionManager::GetInst()->Clear();
	m_bIsRunning = false;
	::WSACleanup();
}
