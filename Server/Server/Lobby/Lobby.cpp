#include "Lobby.h"
#include "../User/User.h"

Lobby::Lobby() :
	m_vecUnusedUserIDs(USER_LOBBY_MAX), m_userCount(0)
{
	uint32 j = USER_LOBBY_MAX - 1;
	for (uint32 i = 0; i < USER_LOBBY_MAX; ++i, --j)
		m_vecUnusedUserIDs[i] = j;
}

Lobby::~Lobby()
{
}

void Lobby::Enter(Connection& _conn, User* _pUser)
{
	m_lock.Lock(eLockType::Writer);
	{
		if (m_userCount < USER_LOBBY_MAX && _pUser->GetSceneState() == eSceneState::Login)
		{
			uint32 lobbyId = m_vecUnusedUserIDs.back();
			m_vecUnusedUserIDs.pop_back();
			m_setAllLobbyUser.insert(_stLobbyUser(lobbyId, _conn.GetId()));
			m_usetUserInLobby.insert(lobbyId);
			m_arrUser[lobbyId].Init(_conn, _pUser);

			_pUser->EnterLobby(lobbyId);

			++m_userCount;
			//printf("로비 입장! : %d명\n", m_userCount);
		}
		else
			printf("입장 거부됨 [현재 인원 : %d]\n", m_userCount);
	}
	m_lock.UnLock(eLockType::Writer);
}

LobbyUser* Lobby::Find(uint32 _lobbyID, uint32 _connID)
{
	if (_lobbyID >= USER_LOBBY_MAX) return nullptr;

	LobbyUser* pUser = nullptr;

	if (m_setAllLobbyUser.find(_stLobbyUser(_lobbyID, _connID)) != m_setAllLobbyUser.cend())
	{
		pUser = &m_arrUser[_lobbyID];
	}
	return pUser;
}

void Lobby::Leave(uint32 _lobbyID, uint32 _connID)
{
	m_lock.Lock(eLockType::Writer); 
	{
		if (Find(_lobbyID, _connID))
		{
			m_arrUser[_lobbyID].Clear();
			m_vecUnusedUserIDs.push_back(_lobbyID);
			m_setAllLobbyUser.erase(_stLobbyUser(_lobbyID, _connID));
			m_usetUserInLobby.erase(_lobbyID);
			--m_userCount;
		}
	}
	m_lock.UnLock(eLockType::Writer);
}

uint32 Lobby::GetUserCount()
{
	m_lock.Lock(eLockType::Reader);
	uint32 cnt = m_userCount;
	m_lock.UnLock(eLockType::Reader);
	return cnt;
}

void Lobby::PacketUserListPage(uint32 _page, Packet& _pkt)
{
	_pkt.Add<PacketType>((PacketType)eServer::LobbyUpdateInfo_UserList);

	m_lock.Lock(eLockType::Reader);
	{
		// 1페이지라면 처음부터 10개
		// 3페이지 : 31~40

		uint32 result;
		if (m_userCount % LOBBY_USERLIST_PAGE == 0)
		{
			if (_page > 0 && _page >= m_userCount / LOBBY_USERLIST_PAGE)
				_page = m_userCount / LOBBY_USERLIST_PAGE - 1;
			result = LOBBY_USERLIST_PAGE;
		}
		else
		{
			if (_page > 0 && _page > m_userCount / LOBBY_USERLIST_PAGE)
				_page = m_userCount / LOBBY_USERLIST_PAGE;
			result = (m_userCount / LOBBY_USERLIST_PAGE == _page) ? m_userCount % LOBBY_USERLIST_PAGE : LOBBY_USERLIST_PAGE;
		}

		int32 startPos = LOBBY_USERLIST_PAGE * _page;

		_pkt.Add<int8>(_page);
		_pkt.Add<int8>(result);

		uint32 lobbyID = 0;
		uint32 count = 0;
		std::set<_stLobbyUser>::iterator iter = m_setAllLobbyUser.begin();

		if (_page != 0)		std::advance(iter, startPos);

		eSceneState eState;
		std::set<_stLobbyUser>::iterator iterEnd = m_setAllLobbyUser.end();
		for (; iter != iterEnd; ++count, ++iter)
		{
			if (count >= result) break;

			lobbyID = iter->lobbyID;
			_pkt.AddWString(m_arrUser[lobbyID].GetNickname());
			eState = m_arrUser[lobbyID].GetSceneState();
			_pkt.Add<int8>((int8)eState);
			if (eState == eSceneState::Room || eState == eSceneState::InGame)
			{
				_pkt.Add<int8>(m_arrUser[lobbyID].GetRoomID());
			}
		}

		//printf("%d\n", result);
	}
	m_lock.UnLock(eLockType::Reader);
}

void Lobby::Send(const Packet& _pkt, uint32 _userID)
{
	if (_userID >= USER_LOBBY_MAX) return;

	m_arrUser[_userID].Send(_pkt);
}

void Lobby::SendAllInLobby(const Packet& _pkt)
{
	m_lock.Lock(eLockType::Reader);
	std::unordered_set<uint32> usetLobbyUser = m_usetUserInLobby;
	m_lock.UnLock(eLockType::Reader);

	for (uint32 id : usetLobbyUser)
		m_arrUser[id].Send(_pkt);
}

Room* Lobby::CreateRoom(Connection& _conn, User* _pUser, const wchar_t* _pTitle)
{
	Room* pRoom = m_roomManager.Create(_conn, _pUser, _pTitle);

	if (pRoom)
	{
		_pUser->CreateRoom(pRoom->GetId());

		m_lock.Lock(eLockType::Writer);
		m_usetUserInLobby.erase(_pUser->GetLobbyId());
		m_lock.UnLock(eLockType::Writer);
	}

	return pRoom;
}

eEnterRoomResult Lobby::EnterRoom(Connection& _conn, User* _pUser, uint32 _roomID)
{
	uint32 myRoomSlotIdx = USER_NOT_IN_THE_ROOM;
	eEnterRoomResult eResult = m_roomManager.Enter(_conn, _pUser, _roomID, OUT myRoomSlotIdx);

	if (eResult == eEnterRoomResult::Success)
	{
		_pUser->EnterRoom(_roomID, myRoomSlotIdx);

		m_lock.Lock(eLockType::Writer);
		m_usetUserInLobby.erase(_pUser->GetLobbyId());
		m_lock.UnLock(eLockType::Writer);
	}
	return eResult;
}

uint32 Lobby::LeaveRoom(User* _pUser, uint32 _roomID, OUT uint32& _prevOwnerIdx, OUT uint32& _newOwnerIdx)
{
	if (_roomID >= USER_LOBBY_MAX) return 0;

	uint32 result = m_roomManager.Leave(_pUser->GetRoomUserIdx(), _roomID, OUT _prevOwnerIdx, OUT _newOwnerIdx);

	if (result != ROOM_ID_NOT_FOUND)
	{
		_pUser->LeaveRoom();

		m_lock.Lock(eLockType::Writer);
		m_usetUserInLobby.insert(_pUser->GetLobbyId());
		m_lock.UnLock(eLockType::Writer);
	}

	return result;
}

// EnterRoom하는 유저가 SendRoom다 할때까지 기다리기 x
// TODO : 제거
void Lobby::SendRoom(const Packet& _pkt, uint32 _roomID, uint32 _exceptID)
{
	if (_roomID >= USER_LOBBY_MAX) return;

	m_roomManager.Send(_pkt, _roomID, _exceptID);
}