#include "User.h"
#include "../Lobby/LobbyManager.h"

User::User(const wchar_t* _pNickname) : 
	m_lobbyID(0),
	m_roomID(0),
	m_roomUserIdx(0),
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

void User::Connect(uint32 _connectionId)
{
	m_lock.Lock(eLockType::Writer);
	SetConnectionId(_connectionId);
	SetState(eLoginState::Login);
	SetSceneState(eSceneState::Login);
	m_lock.UnLock(eLockType::Writer);
}

bool User::IsLogin()
{
	bool login;
	m_lock.Lock(eLockType::Reader);
	login = GetState() == eLoginState::Login;
	m_lock.UnLock(eLockType::Reader);
	return login;
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

		Packet pkt;
		pkt
			.Add<PacketType>((PacketType)eServer::InGameExit)
			.Add<int8>(m_roomUserIdx); // 나간 유저 

		uint32 leftNum = pLobby->LeaveRoom(this, m_roomID, prevOwnerID, nextOwnerID);
		pkt.Add<int8>(nextOwnerID);

		if (leftNum != 0 && leftNum != ROOM_ID_NOT_FOUND)
		{
			pLobby->SendRoom(pkt, roomID, roomUserIdx);
			//printf("InGameExit sent\n");
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
		//printf("%d, %d", prevOwnerID, nextOwnerID);

		if (leftNum != 0 && leftNum != ROOM_ID_NOT_FOUND)
		{
			pLobby->SendRoom(pktNotifyRoomUserExit, roomID, roomUserIdx);
		}
		//printf("%d : User::Leave\n", leftNum);
	}
	case eSceneState::Lobby:
		pLobby->Leave(m_lobbyID, m_connectionId);
		break;
	}

	m_lock.Lock(eLockType::Writer);

	SetConnectionId(USER_NOT_CONNECTED);
	SetSceneState(eSceneState::None);
	SetLobbyID(USER_NOT_IN_THE_ROOM);
	SetRoomID(USER_NOT_IN_THE_ROOM);
	SetRoomUserIdx(USER_NOT_IN_THE_ROOM);
	SetState(eLoginState::Logout);

	m_lock.UnLock(eLockType::Writer);
	wprintf(L"[%s] 님 접속종료\n", m_nickname);
}

void User::LeaveRoom()
{
	SetRoomUserIdx(USER_NOT_IN_THE_ROOM);
	SetRoomID(USER_NOT_IN_THE_ROOM);
	SetSceneState(eSceneState::Lobby);
}

void User::EnterRoom(uint32 _roomID)
{
	SetRoomID(_roomID);
	SetSceneState(eSceneState::Room);
}

void User::CreateRoom(uint32 _roomID)
{
	SetRoomID(_roomID);
	SetRoomUserIdx(0);
	SetSceneState(eSceneState::Room);
}

void User::GameOver()
{
	SetSceneState(eSceneState::Room);
}

void User::GameStart()
{
	SetSceneState(eSceneState::InGame);
}
