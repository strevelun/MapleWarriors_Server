#pragma once

#include "../Packet/Packet.h"
#include "../NetworkCore/Connection.h"
#include "../User/User.h"
#include "../SRWLock.h"

enum class eRoomUserState
{
	None,
	Ready,
	Standby
};

class RoomUser
{
private:
	SRWLock					m_lock;

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

	eRoomUserState GetState() const { return m_eState; }
	bool			IsOwner() const { return m_bOwner; }
	const wchar_t* GetNickname() const { return m_pUser->GetNickname(); }
	uint32 GetConnectionID() const { return m_pConn->GetId(); }
	eCharacterChoice GetCharacterChoice() const { return m_eCharacter; }
	const int8* GetIP() const { return m_pConn->GetIP(); }
	const uint8* GetPrivateIP() const { return m_pConn->GetPrivateIP(); }
	SOCKET GetSocket() const { return m_pConn->GetSocket(); }
	uint16 GetUDPPort() const { return m_pConn->GetUDPPort(); }

	void SetOwner(bool _bIsOwner) { m_bOwner = _bIsOwner; }
	void SetState(eRoomUserState _eState) { m_eState = _eState; }
	void SetUserSceneState(eSceneState _eState) { m_pUser->SetSceneState(_eState); }
	void SetCharacterChoice(eCharacterChoice _eChoice) { m_eCharacter = _eChoice; }

	void Send(const Packet& _pkt);
};

