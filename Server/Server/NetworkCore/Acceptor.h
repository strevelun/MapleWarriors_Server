#pragma once

#include "../Defines.h"
#include "../Types.h"

class Connection;

class Acceptor
{
private:
	SOCKET					m_serverSocket;
	SOCKADDR_IN				m_servAddr;
	SOCKADDR_IN				m_clientAddr;
	int32					m_clientAddrSize;

public:
	Acceptor();
	~Acceptor();

	bool Start(const int8* _ip, uint16 _port);
	bool Bind();
	bool Listen(int32 _backlog);
	tAcceptedClient* Accept();
};

