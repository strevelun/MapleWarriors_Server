#include "Acceptor.h"
#include "Connection.h"
#include "../Defines.h"

Acceptor::Acceptor() :
	m_serverSocket{0}, m_clientAddr{0}, m_clientAddrSize(0)
{

}

Acceptor::~Acceptor()
{
	if (m_serverSocket) closesocket(m_serverSocket);
}

bool Acceptor::Start(const int8* _ip, uint16 _port, int32 _backlog)
{
	m_clientAddrSize = sizeof(SOCKADDR_IN);
	SOCKADDR_IN				m_servAddr;

	m_serverSocket = ::WSASocketW(PF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (m_serverSocket == INVALID_SOCKET)
	{
		printf("서버소켓 생성 에러\n");
		return false;
	}

	memset(&m_servAddr, 0, sizeof(m_servAddr));
	m_servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, _ip, &m_servAddr.sin_addr);
	m_servAddr.sin_port = htons(_port);

	if (::bind(m_serverSocket, (SOCKADDR*)&m_servAddr, sizeof(m_servAddr)) == SOCKET_ERROR)
	{
		printf("bind 에러 : %d\n", WSAGetLastError());
		return false;
	}
	if (::listen(m_serverSocket, _backlog) == SOCKET_ERROR)
	{
		printf("listen 에러 : %d\n", WSAGetLastError());
		return false;
	}
	
	return true;
}

tAcceptedClient* Acceptor::Accept()
{
	SOCKET clientSocket = accept(m_serverSocket, (SOCKADDR*)&m_clientAddr, &m_clientAddrSize);

	tAcceptedClient* pClient = new tAcceptedClient;
	inet_ntop(AF_INET, &m_clientAddr.sin_addr, pClient->ipAddr, sizeof(pClient->ipAddr));
	pClient->clientSocket = clientSocket;

	return pClient;
}