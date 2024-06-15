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
	m_pUser = nullptr;
	m_pConn = nullptr;
}

void LobbyUser::Send(const Packet& _pkt)
{
	if (m_pConn) m_pConn->Send(_pkt);
}
