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

	void OnConnected(tAcceptedClient* _pAcceptedClient);
};
