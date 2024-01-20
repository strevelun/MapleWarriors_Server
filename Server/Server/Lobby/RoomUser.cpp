#include "RoomUser.h"

RoomUser::RoomUser() :
	m_eState(eRoomUserState::None), m_pConn(nullptr)
{
}

RoomUser::~RoomUser()
{
}

void RoomUser::Init(Connection& _conn)
{
	m_pConn = &_conn;
	m_eState = eRoomUserState::Ready;

}

void RoomUser::Clear()
{
	m_pConn = nullptr;
	m_eState = eRoomUserState::None;
}

void RoomUser::Send(const Packet& _pkt)
{
	if (m_pConn) m_pConn->Send(_pkt);
}
