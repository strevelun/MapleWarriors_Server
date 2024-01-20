#pragma once

#include "../Packet/Packet.h"
#include "../NetworkCore/Connection.h"

enum class eRoomUserState
{
	None,
	Ready,
	Standby
};

class RoomUser
{
	eRoomUserState			m_eState;
	Connection*				m_pConn;

public:
	RoomUser();
	~RoomUser();

	void Init(Connection& _conn);
	void Clear();

	eRoomUserState GetState() const { return m_eState; }

	void Send(const Packet& _pkt);
};

