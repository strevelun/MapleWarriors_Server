#pragma once

#include "../Defines.h"
#include "../Packet/Packet.h"

class Connection;

class LobbyUser
{
private:
	Connection*				m_pConn;
	const wchar_t*			m_pNickname;
	eSceneState				m_eSceneState;

public:
	LobbyUser();
	~LobbyUser();

	void Init(Connection& _pConn);
	void Clear();

	const wchar_t* GetNickname() const { return m_pNickname; }
	eSceneState GetSceneState() const { return m_eSceneState; }

	void Send(const Packet& _pkt);
};