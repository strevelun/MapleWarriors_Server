#include "User.h"
#include "../Lobby/LobbyManager.h"

User::User(const wchar_t* _pNickname) : 
	m_eLoginState(eLoginState::Logout),
	m_eSceneState(eSceneState::Login),
	m_connectionId(USER_NOT_CONNECTED),
	m_loginCount(0), 
	m_killCount(0)
{
	uint32 len = (uint32)wcslen(_pNickname);
	if (len > NICKNAME_LEN)			len = NICKNAME_LEN;
	wcsncpy_s(m_nickname, _pNickname, len);
}

User::~User()
{
}

void User::Leave()
{
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();

	switch (m_eSceneState)
	{
	case eSceneState::Room:
	{
		uint32 roomID = m_roomID;
		uint32 roomUserIdx = m_roomUserIdx;
		uint32 prevOwnerID = USER_NOT_IN_THE_ROOM;
		uint32 nextOwnerID = USER_NOT_IN_THE_ROOM;
		Packet pktNotifyRoomUserExit;
		pktNotifyRoomUserExit
			.Add<PacketType>((PacketType)eServer::NotifyRoomUserExit)
			.Add<char>(m_roomUserIdx); // 나간 유저 

		uint32 leftNum = pLobby->LeaveRoom(this, m_roomID, prevOwnerID, nextOwnerID);

		pktNotifyRoomUserExit.Add<char>(prevOwnerID);
		pktNotifyRoomUserExit.Add<char>(nextOwnerID);
		printf("%d, %d", prevOwnerID, nextOwnerID);

		if (leftNum != 0 && leftNum != ROOM_ID_NOT_FOUND)
		{
			pLobby->SendRoom(pktNotifyRoomUserExit, roomID, roomUserIdx);
			printf("User::Leave::SendRoom\n");
		}


		printf("%d : User::Leave\n", leftNum);
	}
	case eSceneState::Lobby:
		pLobby->Leave(m_lobbyID);
		break;
	}

	SetState(eLoginState::Logout);
	wprintf(L"[%s] 님 접속종료\n", m_nickname);
}
