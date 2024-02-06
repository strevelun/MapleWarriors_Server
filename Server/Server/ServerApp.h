#pragma once

#include "NetworkCore/Acceptor.h"
#include "NetworkCore/NetworkEngine.h"

class ServerApp
{
private:
	NetworkEngine		m_engine;
	Acceptor			m_acceptor;

	bool m_bIsRunning;

public:
	ServerApp();
	~ServerApp();

	bool Init(const int8* _ip, unsigned short _port, int32 _backlog);
	void Run();
	void Shutdown();
};

