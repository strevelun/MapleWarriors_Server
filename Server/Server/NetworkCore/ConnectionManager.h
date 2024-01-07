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
	Connection* Create(SOCKET _socket);
	bool Delete(uint32 _id);

#pragma region Singleton
private:
	static ConnectionManager* s_pInst;

public:
	static ConnectionManager* GetInst()
	{
		if (!s_pInst)
			s_pInst = new ConnectionManager;
		return s_pInst;
	}

	static void DestInst()
	{
		if (s_pInst)
			delete s_pInst;
		s_pInst = nullptr;
	}

private:
	ConnectionManager();
	~ConnectionManager();
#pragma endregion 
};

