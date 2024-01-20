#include "Lobby.h"
#include "../User.h"

Lobby::Lobby() :
	m_vecUnusedUserIDs(USER_LOBBY_MAX), m_vecUnusedRoomIDs(USER_LOBBY_MAX), m_userCount(0), m_roomCount(0)
{
	int j = USER_LOBBY_MAX - 1;
	for (uint32 i = 0; i < USER_LOBBY_MAX; ++i, --j)
		m_vecUnusedUserIDs[i] = j;

	j = USER_LOBBY_MAX - 1;
	for (uint32 i = 0; i < USER_LOBBY_MAX; ++i, --j)
	{
		m_vecUnusedRoomIDs[i] = j;
	}
}

Lobby::~Lobby()
{
}

void Lobby::Enter(Connection& _conn)
{
	m_userLock.Enter();
	if (m_userCount < USER_LOBBY_MAX && _conn.GetSceneState() == eSceneState::Login)
	{
		uint32 lobbyId = m_vecUnusedUserIDs.back();
		m_vecUnusedUserIDs.pop_back();
		m_setLobbyUser.insert(lobbyId);
		m_arrUser[lobbyId].Init(_conn);

		_conn.SetLobbyID(lobbyId);
		_conn.SetSceneState(eSceneState::Lobby);

		++m_userCount;
		printf("로비 입장! : %d명\n", m_userCount);
	}
	else
		printf("입장 거부됨 [현재 인원 : %d]\n", m_userCount);
	m_userLock.Leave();
}

LobbyUser* Lobby::Find(uint32 _lobbyID)
{
	if (_lobbyID >= USER_LOBBY_MAX) return nullptr;

	LobbyUser* pUser = nullptr;

	m_userLock.Enter();
	if (m_setLobbyUser.find(_lobbyID) != m_setLobbyUser.cend())
	{
		pUser = &m_arrUser[_lobbyID];
	}
	m_userLock.Leave();
	return pUser;
}

void Lobby::Leave(uint32 _lobbyID)
{
	if (!Find(_lobbyID)) return;

	m_userLock.Enter();
	m_arrUser[_lobbyID].Clear();
	m_vecUnusedUserIDs.push_back(_lobbyID);
	m_setLobbyUser.erase(_lobbyID);
	--m_userCount;
	m_userLock.Leave();
}

void Lobby::PacketUserListPage(uint32 _page, Packet& _pkt)
{
	m_userLock.Enter();

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

	int startPos = LOBBY_USERLIST_PAGE * _page;

	_pkt.Add<char>(_page);
	_pkt.Add<char>(result);

	uint32 lobbyID = 0;
	int count = 0;
	std::set<uint32>::iterator iter = m_setLobbyUser.begin();

	if (_page != 0)		std::advance(iter, startPos);

	std::set<uint32>::iterator iterEnd = m_setLobbyUser.end();
	for (; iter != iterEnd; ++count, ++iter)
	{
		if (count >= result) break;

		lobbyID = *iter;
		_pkt.AddWString(m_arrUser[lobbyID].GetNickname());
		_pkt.Add<char>((char)m_arrUser[lobbyID].GetSceneState());
	}

	//printf("%d\n", result);

	m_userLock.Leave();
}

void Lobby::PacketRoomListPage(uint32 _page, Packet& _pkt)
{
	m_roomLock.Enter();

	// 1페이지라면 처음부터 10개
	// 3페이지 : 31~40

	uint32 result;
	if (m_roomCount > 0 && m_roomCount % LOBBY_ROOMLIST_PAGE == 0)
	{
		if (_page > 0 && _page >= m_roomCount / LOBBY_ROOMLIST_PAGE)
			_page = m_roomCount / LOBBY_ROOMLIST_PAGE - 1;
		result = LOBBY_ROOMLIST_PAGE;
	}
	else
	{
		if (_page > 0 && _page > m_roomCount / LOBBY_ROOMLIST_PAGE)
			_page = m_roomCount / LOBBY_ROOMLIST_PAGE;
		result = (m_roomCount / LOBBY_ROOMLIST_PAGE == _page) ? m_roomCount % LOBBY_ROOMLIST_PAGE : LOBBY_ROOMLIST_PAGE;
	}

	int startPos = LOBBY_ROOMLIST_PAGE * _page;

	_pkt.Add<char>(_page);
	_pkt.Add<char>(result);

	uint32 roomID = 0;
	int count = 0;
	std::set<uint32>::iterator iter = m_setRoom.begin();

	if (_page != 0)		std::advance(iter, startPos);

	std::set<uint32>::iterator iterEnd = m_setRoom.end();
	for (; iter != iterEnd; ++count, ++iter)
	{
		if (count >= result) break;

		roomID = *iter;
		_pkt.AddWString(m_arrRoom[roomID].GetTitle());
		_pkt.AddWString(m_arrRoom[roomID].GetOwner());
		_pkt.Add<char>(roomID);
	}

	//printf("%d\n", result);

	m_roomLock.Leave();
}

void Lobby::SendAll(const Packet& _pkt)
{
	// for문에 진입할때 m_arrUser[id]가 Clear됨. nullptr에 접근

	//m_userLock.Enter();
	//printf("before loop\n");

	for (uint32 id : m_setLobbyUser)
	{
		m_arrUser[id].Send(_pkt);
	}
	//printf("after loop\n");
	
	//std::set<uint32>::iterator iter = m_setLobbyUser.begin();
	//std::set<uint32>::iterator iterEnd = m_setLobbyUser.end(); 	Server.exe!Connection::Send(const Packet & _packet) 줄 86	C++

	//for (; iter != iterEnd; ++iter)
	//{
	//	m_arrUser[*iter].Send(_pkt);
	//}
	//m_userLock.Leave();
}

Room* Lobby::CreateRoom(Connection& _conn, const wchar_t* _pTitle)
{
	if (m_roomCount >= USER_LOBBY_MAX) return nullptr; // TODO : inside lock?
	
	m_roomLock.Enter();

	uint32 roomId = m_vecUnusedRoomIDs.back();
	m_vecUnusedRoomIDs.pop_back();
	m_setRoom.insert(roomId);
	m_arrRoom[roomId].Init(_conn, _pTitle, roomId);

	_conn.SetRoomID(roomId);

	++m_roomCount;
	m_roomLock.Leave();

	return &m_arrRoom[roomId];
}

Room* Lobby::FindRoom(uint32 _roomID)
{
	if (_roomID >= USER_LOBBY_MAX) return nullptr;

	Room* pRoom = nullptr;

	m_roomLock.Enter();
	if (m_setRoom.find(_roomID) != m_setRoom.cend())
	{
		pRoom = &m_arrRoom[_roomID];
	}
	m_roomLock.Leave();
	return pRoom;
}

void Lobby::LeaveRoom(uint32 _roomID)
{
	if (!FindRoom(_roomID)) return;

	m_roomLock.Enter();
	m_arrRoom[_roomID].Clear();
	m_vecUnusedRoomIDs.push_back(_roomID);
	m_setRoom.erase(_roomID);
	--m_roomCount;
	m_roomLock.Leave();
}