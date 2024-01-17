#include "Lobby.h"
#include "../User.h"

Lobby::Lobby() :
	m_vecUnusedIDs(USER_LOBBY_MAX), m_userCount(0)
{
	int j = USER_LOBBY_MAX - 1;
	for (uint32 i = 0; i < USER_LOBBY_MAX; ++i, --j)
		m_vecUnusedIDs[i] = j;
}

Lobby::~Lobby()
{
}

void Lobby::Enter(Connection& _conn)
{
	m_lock.Enter();
	if (m_userCount < USER_LOBBY_MAX && _conn.GetSceneState() == eSceneState::Login)
	{
		uint32 lobbyId = m_vecUnusedIDs.back();
		m_vecUnusedIDs.pop_back();
		m_setLobbyUser.insert(lobbyId);
		m_arrUser[lobbyId].Init(_conn);

		_conn.SetLobbyID(lobbyId);
		_conn.SetSceneState(eSceneState::Lobby);

		++m_userCount;
		printf("로비 입장!\n");
	}
	else
		printf("입장 거부됨 [현재 인원 : %d]\n", m_userCount);
	m_lock.Leave();
}

LobbyUser* Lobby::Find(uint32 _lobbyID)
{
	if (_lobbyID >= USER_LOBBY_MAX) return nullptr;

	m_lock.Enter();
	if (m_setLobbyUser.find(_lobbyID) == m_setLobbyUser.cend())
	{
		m_lock.Leave();
		return nullptr;
	}
	m_lock.Leave();
	return &m_arrUser[_lobbyID];
}

void Lobby::Leave(uint32 _lobbyID)
{
	if (!Find(_lobbyID)) return;

	m_lock.Enter();
	m_arrUser[_lobbyID].Clear();
	m_vecUnusedIDs.push_back(_lobbyID);
	m_setLobbyUser.erase(_lobbyID);
	--m_userCount;
	m_lock.Leave();
}

// 0~9 / 10~19 / 20~29 / 30~39 / 40~49
void Lobby::PacketUserListPage(uint32 _page, Packet& _pkt)
{
	m_lock.Enter();

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

	if(_page != 0)		std::advance(iter, startPos);

	std::set<uint32>::iterator iterEnd = m_setLobbyUser.end();
	for (; iter != iterEnd; ++count, ++iter)
	{
		if (count >= result) break;

		lobbyID = *iter;
		_pkt.AddWString(m_arrUser[lobbyID].GetNickname());
		_pkt.Add<char>((char)m_arrUser[lobbyID].GetSceneState());
	}

	//printf("%d\n", result);
		
	m_lock.Leave();
}

void Lobby::SendAll(const Packet& _pkt)
{
	m_lock.Enter();
	std::set<uint32>::iterator iter = m_setLobbyUser.begin();
	std::set<uint32>::iterator iterEnd = m_setLobbyUser.end();
	for (; iter != iterEnd; ++iter)
	{
		m_arrUser[*iter].Send(_pkt);
	}
	m_lock.Leave();
}

void Lobby::Clear(uint32 _lobbyID)
{
}

