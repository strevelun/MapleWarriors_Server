#include "Acceptor.h"
#include "Connection.h"
#include "../Defines.h"

Acceptor::Acceptor() :
	m_serverSocket{ 0 }, m_servAddr{ 0 }, m_clientAddr {0}, m_clientAddrSize(0)
{
	
}

Acceptor::~Acceptor()
{
	if (m_serverSocket)
	{
		::closesocket(m_serverSocket);
		m_serverSocket = INVALID_SOCKET;
	}
}

bool Acceptor::Start(const int8* _ip, uint16 _port)
{
	m_clientAddrSize = sizeof(SOCKADDR_IN);

	m_serverSocket = ::WSASocketW(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED); 
	if (m_serverSocket == INVALID_SOCKET)
	{
		printf("서버소켓 생성 에러 : %d\n", ::WSAGetLastError());
		return false;
	}

	::memset(&m_servAddr, 0, sizeof(m_servAddr));
	m_servAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, _ip, &m_servAddr.sin_addr);
	m_servAddr.sin_port = ::htons(_port);

	return true;
}

bool Acceptor::Bind()
{
	if (::bind(m_serverSocket, reinterpret_cast<SOCKADDR*>(&m_servAddr), sizeof(m_servAddr)) == SOCKET_ERROR)
	{
		printf("bind 에러 : %d\n", ::WSAGetLastError());
		return false;
	}
	return true;
}

bool Acceptor::Listen(int32 _backlog)
{
	if (::listen(m_serverSocket, _backlog) == SOCKET_ERROR)
	{
		printf("listen 에러 : %d\n", ::WSAGetLastError());
		return false;
	}
	return true;
}

tAcceptedClient* Acceptor::Accept()
{
	SOCKET clientSocket = ::accept(m_serverSocket, reinterpret_cast<SOCKADDR*>(&m_clientAddr), &m_clientAddrSize);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("accept 에러 : %d\n", ::WSAGetLastError());
		return nullptr;
	}

	tAcceptedClient* pClient = new tAcceptedClient;
	::inet_ntop(AF_INET, &m_clientAddr.sin_addr, pClient->ipAddr, sizeof(pClient->ipAddr));

	// 테스트
#ifdef _DEBUG	
	if(pClient->ipAddr[0] == '1' && pClient->ipAddr[1] == '9' && pClient->ipAddr[2] == '2')
		strcpy_s(pClient->ipAddr, SERVER_EXTERNAL_IP);
#endif

	pClient->clientSocket = clientSocket;

	return pClient;
}