#pragma once

#include "../Defines.h"
#include "../Types.h"
#include "../NetworkCore/Connection.h"

class User
{
private:

private:
	eLoginState		m_eLoginState; 
	eSceneState     m_eSceneState;
	uint32			m_connectionId;

	uint32          m_lobbyID;
	uint32          m_roomID;
	uint32          m_roomUserIdx;

	wchar_t			m_nickname[NICKNAME_LEN];
	uint32			m_loginCount;
	uint32			m_killCount;

public:
	User(const wchar_t* _pNickname);
	~User();

	const wchar_t* GetNickname() const { return m_nickname; }
	eLoginState GetState() const { return m_eLoginState; }
	uint32 GetLobbyId() const { return m_lobbyID; }
	uint32 GetRoomId() const { return m_roomID; }
	uint32 GetRoomUserIdx() const { return m_roomUserIdx; }
	eSceneState GetSceneState() const { return m_eSceneState; }

	void SetConnectionId(uint32 _connectionId) { m_connectionId = _connectionId; }
	void SetState(eLoginState _eState) { m_eLoginState = _eState; }
	void SetSceneState(eSceneState _eState) { m_eSceneState = _eState; }
	void SetLobbyID(uint32 _id) { m_lobbyID = _id; }
	void SetRoomID(uint32 _id) { m_roomID = _id; }
	void SetRoomUserIdx(uint32 _id) { m_roomUserIdx = _id; }

	void Leave();
};

