#include "LobbyUser.h"
#include "../NetworkCore/Connection.h"
#include "../User.h"

LobbyUser::LobbyUser() :
	m_pConn(nullptr), m_pNickname(nullptr), m_eSceneState(eSceneState::None)
{
}

LobbyUser::~LobbyUser()
{
}

void LobbyUser::Init(Connection& _pConn)
{
	m_pConn = &_pConn;
	User* pUser = _pConn.GetUser();
	m_pNickname = pUser->GetNickname();
	m_eSceneState = _pConn.GetSceneState();
}

void LobbyUser::Clear()
{
	m_pNickname = nullptr;
	m_eSceneState = eSceneState::None;
	m_pConn = nullptr;
}

void LobbyUser::Send(const Packet& _pkt)
{
	m_pConn->Send(_pkt);
}
