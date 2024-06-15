#pragma once

#include "../Packet/Packet.h"
#include "../NetworkCore/Connection.h"
#include "../User/User.h"

enum class eRoomUserState
{
	None,
	Ready,
	Standby
};

class RoomUser
{
private:
	Connection*				m_pConn;
	User*					m_pUser;
	eRoomUserState			m_eState;
	bool					m_bOwner;
	eCharacterChoice		m_eCharacter;

public:
	RoomUser();
	~RoomUser();

	void Init(Connection& _conn, User* _pUser, bool _bIsOwner = false);
	void Clear();

	const wchar_t* GetNickname() const { return m_pUser ? m_pUser->GetNickname() : nullptr; }
	uint32 GetConnectionID() const { return m_pConn ? m_pConn->GetId() : 0;  }
	const int8* GetIP() const { return m_pConn ? m_pConn->GetIP() : nullptr; }
	const uint8* GetPrivateIP() const { return m_pConn ? m_pConn->GetPrivateIP() : nullptr; }
	SOCKET GetSocket() const { return m_pConn ? m_pConn->GetSocket() : (SOCKET)0; }
	uint16 GetUDPPort() const { return m_pConn ? m_pConn->GetUDPPort() : 0; }
	eRoomUserState GetState() const { return m_eState; }
	bool			IsOwner() const { return m_bOwner; }
	eCharacterChoice GetCharacterChoice() const { return m_eCharacter; }

	void SetState(eRoomUserState _eState) { m_eState = _eState; }
	void SetOwner(bool _bIsOwner) { m_bOwner = _bIsOwner; }
	void SetCharacterChoice(eCharacterChoice _eChoice) { m_eCharacter = _eChoice; }

	void Send(const Packet& _pkt);
	
	void GameOver() { if(m_pUser) m_pUser->GameOver(); }
	void GameStart() { if(m_pUser) m_pUser->GameStart(); }
};

