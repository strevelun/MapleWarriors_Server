#pragma once

#include "IOCP.h"

#include <vector>

class NetworkEngine
{
private:
	IOCP			m_iocp;

public:
	NetworkEngine();
	~NetworkEngine();
	
	bool Init();

	void OnConnected(Connection* _pConn);
};
