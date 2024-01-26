#include "LobbyUser.h"
#include "../NetworkCore/Connection.h"
#include "../User/User.h"

LobbyUser::LobbyUser() :
	m_pConn(nullptr), m_pNickname(nullptr), m_eSceneState(eSceneState::None)
{
}

LobbyUser::~LobbyUser()
{
}

void LobbyUser::Init(Connection& _pConn, User* _pUser)
{
	m_pConn = &_pConn;
	m_pNickname = _pUser->GetNickname();
	m_eSceneState = _pUser->GetSceneState();
}

void LobbyUser::Clear()
{
	m_pNickname = nullptr;
	m_eSceneState = eSceneState::None;
	m_pConn = nullptr;
}

void LobbyUser::Send(const Packet& _pkt)
{
	if (!m_pConn) return;
	m_pConn->Send(_pkt);
}
