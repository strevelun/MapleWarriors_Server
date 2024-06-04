#include "LobbyUser.h"
#include "../NetworkCore/Connection.h"
#include "../User/User.h"

LobbyUser::LobbyUser() :
	m_pConn(nullptr), m_pUser(nullptr)
{
}

LobbyUser::~LobbyUser()
{
}

void LobbyUser::Init(Connection& _conn, User* _pUser)
{
	m_pConn = &_conn;
	m_pUser = _pUser;
}

void LobbyUser::Clear()
{
	m_lock.Enter();
	m_pUser = nullptr;
	m_pConn = nullptr;
	m_lock.Leave();
}

void LobbyUser::Send(const Packet& _pkt)
{
	m_lock.Enter();
	if (!m_pConn)
	{
		m_lock.Leave();
		return;
	}
	m_pConn->Send(_pkt);
	m_lock.Leave();
}
