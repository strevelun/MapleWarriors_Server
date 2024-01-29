#include "RoomUser.h"
#include "../User/User.h"

RoomUser::RoomUser() :
	m_eState(eRoomUserState::None), m_bOwner(false), m_pNickname(nullptr), m_pConn(nullptr)
{
}

RoomUser::~RoomUser()
{
}

void RoomUser::Init(Connection& _conn, User* _pUser, bool _bIsOwner)
{
	m_pConn = &_conn;
	m_eState = eRoomUserState::Standby;
	m_pNickname = _pUser->GetNickname();
	m_bOwner = _bIsOwner;

}

void RoomUser::Clear()
{
	m_pConn = nullptr;
	m_eState = eRoomUserState::None;
	m_pNickname = nullptr;
	m_bOwner = false;
}

void RoomUser::Send(const Packet& _pkt)
{
	if (m_pConn) m_pConn->Send(_pkt);
}
