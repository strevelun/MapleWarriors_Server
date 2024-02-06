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
	m_lock.Enter();
	{
		if (m_userCount < USER_LOBBY_MAX && _pUser->GetSceneState() == eSceneState::Login)
		{
			uint32 lobbyId = m_vecUnusedUserIDs.back();
			m_vecUnusedUserIDs.pop_back();
			m_setAllLobbyUser.insert(lobbyId);
			m_usetUserInLobby.insert(lobbyId);
			m_arrUser[lobbyId].Init(_conn, _pUser);

			_pUser->SetLobbyID(lobbyId);
			_pUser->SetSceneState(eSceneState::Lobby);

			++m_userCount;
			printf("로비 입장! : %d명\n", m_userCount);
		}
		else
			printf("입장 거부됨 [현재 인원 : %d]\n", m_userCount);
	}
	m_lock.Leave();
}

LobbyUser* Lobby::Find(uint32 _lobbyID)
{
	if (_lobbyID >= USER_LOBBY_MAX) return nullptr;

	LobbyUser* pUser = nullptr;

	//m_userLock.Enter();
	if (m_setAllLobbyUser.find(_lobbyID) != m_setAllLobbyUser.cend())
	{
		pUser = &m_arrUser[_lobbyID];
	}
	//m_userLock.Leave();
	return pUser;
}

void Lobby::Leave(uint32 _lobbyID)
{
	m_lock.Enter();
	{
		if (Find(_lobbyID))
		{
			m_arrUser[_lobbyID].Clear();
			m_vecUnusedUserIDs.push_back(_lobbyID);
			m_setAllLobbyUser.erase(_lobbyID);
			m_usetUserInLobby.erase(_lobbyID);
			--m_userCount;
		}
	}
	m_lock.Leave();
}

void Lobby::PacketUserListPage(uint32 _page, Packet& _pkt)
{
	_pkt.Add<PacketType>((PacketType)eServer::LobbyUpdateInfo_UserList);

	m_lock.Enter();
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
		std::set<uint32>::iterator iter = m_setAllLobbyUser.begin();

		if (_page != 0)		std::advance(iter, startPos);

		eSceneState eState;
		std::set<uint32>::iterator iterEnd = m_setAllLobbyUser.end();
		for (; iter != iterEnd; ++count, ++iter)
		{
			if (count >= result) break;

			lobbyID = *iter;
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
	m_lock.Leave();
}

void Lobby::Send(const Packet& _pkt, uint32 _userID)
{
	if (_userID >= USER_LOBBY_MAX) return;

	m_arrUser[_userID].Send(_pkt);
}

void Lobby::SendAllInLobby(const Packet& _pkt)
{
	m_lock.Enter();
	auto usetLobbyUser = m_usetUserInLobby;
	m_lock.Leave();

	for (uint32 id : usetLobbyUser)
		m_arrUser[id].Send(_pkt);
}

Room* Lobby::CreateRoom(Connection& _conn, User* _pUser, const wchar_t* _pTitle)
{
	Room* pRoom = m_roomManager.Create(_conn, _pUser, _pTitle);

	if (pRoom)
	{
		m_lock.Enter();
		{
			m_usetUserInLobby.erase(_pUser->GetLobbyId());
		}
		m_lock.Leave();
	}

	return pRoom;
}

eEnterRoomResult Lobby::EnterRoom(Connection& _conn, User* _pUser, uint32 _roomID)
{
	eEnterRoomResult eResult = m_roomManager.Enter(_conn, _pUser, _roomID);

	if (eResult == eEnterRoomResult::Success)
	{
		m_lock.Enter();
		{
			m_usetUserInLobby.erase(_pUser->GetLobbyId());
		}
		m_lock.Leave();
	}
	return eResult;
}

// _pUser의 LobbyID가 초기화된 후 LeaveRoom이 호출되는 경우 포착됨
uint32 Lobby::LeaveRoom(User* _pUser, uint32 _roomID, uint32& _prevOwnerIdx, uint32& _newOwnerIdx)
{
	if (_roomID >= USER_LOBBY_MAX) return 0;

	uint32 result = m_roomManager.Leave(_pUser, _roomID, _prevOwnerIdx, _newOwnerIdx);

	if (result != ROOM_ID_NOT_FOUND)
	{
		m_lock.Enter();
		m_usetUserInLobby.insert(_pUser->GetLobbyId());
		m_lock.Leave();
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