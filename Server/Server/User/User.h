#pragma once

#include "../Defines.h"
#include "../Types.h"
#include "../NetworkCore/Connection.h"
#include "../SRWLock.h"

class User
{
private:
	SRWLock			m_lock;

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

	void Connect(uint32 _connectionId);
	bool IsLogin();
	void Leave();
	void LeaveRoom();
	void EnterRoom(uint32 _roomID, uint32 _myRoomSlotIdx);
	void CreateRoom(uint32 _roomID);
	void EnterLobby(uint32 _lobbyID);
	void GameOver();
	void GameStart();
};

