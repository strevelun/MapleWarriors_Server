#include "Lobby.h"

Lobby::Lobby() :
	m_userCount(0)
{
}

Lobby::~Lobby()
{
}

void Lobby::Add(Connection& _conn)
{
	m_lock.Enter();
	if (m_userCount >= USER_LOBBY_MAX) return;
	if (Find(_conn.GetId())) return;

	m_mapConn.insert({ _conn.GetId(), &_conn});
	++m_userCount;
	m_lock.Leave();
}

Connection* Lobby::Find(int32 _id)
{
	auto iter = m_mapConn.find(_id);
	if (iter == m_mapConn.cend()) return nullptr;

	return iter->second;
}

void Lobby::Delete(int32 _id)
{
	if (!Find(_id)) return;

	m_mapConn.erase(_id);
}

void Lobby::SendAll(const Packet& _pkt)
{
	m_lock.Enter();
	for (auto& conn : m_mapConn)
		conn.second->Send(_pkt);
	m_lock.Leave();
}
