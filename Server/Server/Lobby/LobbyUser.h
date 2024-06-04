#pragma once

#include "../Defines.h"
#include "../Packet/Packet.h"
#include "../User/User.h"

class Connection;

class LobbyUser
{
private:
	CSLock									m_lock;

private:
	Connection*								m_pConn;
	User*									m_pUser;

public:
	LobbyUser();
	~LobbyUser();

	void Init(Connection& _conn, User* _pUser);
	void Clear();

	const wchar_t* GetNickname() const { return m_pUser ? m_pUser->GetNickname() : nullptr; }
	eSceneState GetSceneState() const { return m_pUser ? m_pUser->GetSceneState() : eSceneState::None; }
	uint32		GetRoomID() const { return m_pUser ? m_pUser->GetRoomId() : ROOM_ID_NOT_FOUND; }

	void Send(const Packet& _pkt);
};