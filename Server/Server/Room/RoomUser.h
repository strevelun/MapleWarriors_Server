#pragma once

#include "../Packet/Packet.h"
#include "../NetworkCore/Connection.h"

class User;

enum class eRoomUserState
{
	None,
	Ready,
	Standby
};

class RoomUser
{
	eRoomUserState			m_eState;
	bool					m_bOwner;
	const wchar_t*			m_pNickname;
	Connection*				m_pConn;

public:
	RoomUser();
	~RoomUser();

	void Init(Connection& _conn, User* _pUser, bool _bIsOwner = false);
	void Clear();

	eRoomUserState GetState() const { return m_eState; }
	bool			IsOwner() const { return m_bOwner; }
	const wchar_t* GetNickname() const { return m_pNickname; }
	uint32 GetConnectionID() const { return m_pConn->GetId(); }

	void SetOwner(bool _bIsOwner) { m_bOwner = _bIsOwner; }
	void SetState(eRoomUserState _eState) { m_eState = _eState; }

	void Send(const Packet& _pkt);
};

