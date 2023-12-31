#pragma once

#include "../Defines.h"

class Connection;

class Acceptor
{
private:
	SOCKET					m_serverSocket;

public:
	Acceptor();
	~Acceptor();

	bool Start(const char* _ip, unsigned short _port, int _backlog);
	SOCKET Accept();
};

