#pragma once

#include "../Defines.h"
#include "../Packet/Packet.h"
#include "../User/User.h"

class Connection;

class LobbyUser
{
private:
	Connection*				m_pConn;
	User*					m_pUser;

public:
	LobbyUser();
	~LobbyUser();

	void Init(Connection& _pConn, User* _pUser);
	void Clear();

	const wchar_t* GetNickname() const { return m_pUser->GetNickname(); }
	eSceneState GetSceneState() const { return m_pUser->GetSceneState(); }
	uint32		GetRoomID() const { return m_pUser->GetRoomId(); }

	void Send(const Packet& _pkt);
};