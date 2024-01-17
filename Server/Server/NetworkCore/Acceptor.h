#pragma once

#include "../Defines.h"

class Connection;

class Acceptor
{
private:
	SOCKET					m_serverSocket;
	SOCKADDR_IN				m_clientAddr;
	int						m_clientAddrSize;

public:
	Acceptor();
	~Acceptor();

	bool Start(const char* _ip, unsigned short _port, int _backlog);
	SOCKET Accept();
};

