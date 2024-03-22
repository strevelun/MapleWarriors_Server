#pragma once

#include "../Defines.h"

#include "../Types.h"
#include "Connection.h"
#include "../CSLock.h"

class ConnectionManager
{
private:
	CSLock									m_lock;
	uint32									m_connectionId;
	uint32									m_count;
	std::unordered_map<uint32, Connection*> m_mapConnection;

public:
	Connection* Create(tAcceptedClient* _pAcceptedClient);
	void Delete(uint32 _id);

	uint32 GetCount() const { return m_count; }

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

