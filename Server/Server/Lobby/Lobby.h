#pragma once

#include "../NetworkCore/Connection.h"
#include "../CSLock.h"

class Lobby
{
private:
	CSLock m_lock;
	std::unordered_map<int32, Connection*> m_mapConn;

	uint32 m_userCount;

public:
	Lobby();
	~Lobby();

	void Add(Connection& _conn);
	Connection* Find(int32 _id);
	void Delete(int32 _id);
	
	void SendAll(const Packet& _pkt);
};

