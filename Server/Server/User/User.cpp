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
	case eSceneState::InGame:
	{
		uint32 roomID = m_roomID;
		uint32 roomUserIdx = m_roomUserIdx;
		uint32 prevOwnerID = USER_NOT_IN_THE_ROOM;
		uint32 nextOwnerID = USER_NOT_IN_THE_ROOM;
		// 인게임에서 나가면 로비에서도 나가고, 룸에서도 나가는 처리 해야
	   // 나간놈이 방장이면 다음 놈에게 양도해야
		Packet pkt;
		pkt
			.Add<PacketType>((PacketType)eServer::InGameExit)
			.Add<int8>(m_roomUserIdx); // 나간 유저 

		uint32 leftNum = pLobby->LeaveRoom(this, m_roomID, prevOwnerID, nextOwnerID);
		pkt.Add<int8>(nextOwnerID);

		if (leftNum != 0 && leftNum != ROOM_ID_NOT_FOUND)
		{
			pLobby->SendRoom(pkt, roomID, roomUserIdx);
			printf("InGameExit sent\n");
		}

		pLobby->Leave(m_lobbyID, m_connectionId);

		break;
	}
	case eSceneState::Room:
	{
		uint32 roomID = m_roomID;
		uint32 roomUserIdx = m_roomUserIdx;
		uint32 prevOwnerID = USER_NOT_IN_THE_ROOM;
		uint32 nextOwnerID = USER_NOT_IN_THE_ROOM;
		Packet pktNotifyRoomUserExit;
		pktNotifyRoomUserExit
			.Add<PacketType>((PacketType)eServer::NotifyRoomUserExit)
			.Add<int8>(m_roomUserIdx); // 나간 유저 

		uint32 leftNum = pLobby->LeaveRoom(this, m_roomID, prevOwnerID, nextOwnerID);

		pktNotifyRoomUserExit.Add<int8>(prevOwnerID);
		pktNotifyRoomUserExit.Add<int8>(nextOwnerID);
		printf("%d, %d", prevOwnerID, nextOwnerID);

		if (leftNum != 0 && leftNum != ROOM_ID_NOT_FOUND)
		{
			pLobby->SendRoom(pktNotifyRoomUserExit, roomID, roomUserIdx);
			printf("User::Leave::SendRoom\n");
		}


		printf("%d : User::Leave\n", leftNum);
	}
	case eSceneState::Lobby:
		pLobby->Leave(m_lobbyID, m_connectionId);
		break;
	}

	SetState(eLoginState::Logout);
	wprintf(L"[%s] 님 접속종료\n", m_nickname);
}
