#pragma once

#include "../Defines.h"

#include "../Types.h"
#include "Connection.h"

class ConnectionManager
{
private:
	uint32 m_connectionId;
	std::unordered_map<uint32, Connection*> m_mapConnection;

public:
	Connection* CreateConnection(SOCKET _socket);

	bool DeleteConnection(uint32 _id);

#pragma region Singleton
private:
	static ConnectionManager* m_pInst;

public:
	static ConnectionManager* GetInst()
	{
		if (!m_pInst)
			m_pInst = new ConnectionManager;
		return m_pInst;
	}

	static void DestInst()
	{
		if (m_pInst)
			delete m_pInst;
		m_pInst = nullptr;
	}

private:
	ConnectionManager();
	~ConnectionManager();
#pragma endregion 
};

