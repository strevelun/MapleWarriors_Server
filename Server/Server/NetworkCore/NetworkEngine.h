#pragma once

#include "IOCP.h"

class NetworkEngine
{
private:
	IOCP				m_iocp;

public:
	NetworkEngine();
	~NetworkEngine();
	
	bool Init();

	Connection* OnConnected(tAcceptedClient* _pAcceptedClient);
};
