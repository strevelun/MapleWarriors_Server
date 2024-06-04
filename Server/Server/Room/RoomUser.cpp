#include "RoomUser.h"
#include "../User/User.h"

RoomUser::RoomUser() :
	m_pConn(nullptr), m_pUser(nullptr), m_eState(eRoomUserState::None), m_bOwner(false), m_eCharacter(eCharacterChoice::None)
{
}

RoomUser::~RoomUser()
{
}

void RoomUser::Init(Connection& _conn, User* _pUser, bool _bIsOwner)
{
	m_pConn = &_conn;
	m_pUser = _pUser;
	m_eState = eRoomUserState::Standby;
	m_bOwner = _bIsOwner;
	m_eCharacter = eCharacterChoice::Player1;
}

void RoomUser::Clear()
{
	m_pConn = nullptr;
	m_eState = eRoomUserState::None;
	m_pUser = nullptr;
	m_bOwner = false;
	m_eCharacter = eCharacterChoice::None;
}

void RoomUser::Send(const Packet& _pkt)
{
	if (m_pConn) m_pConn->Send(_pkt);
}
