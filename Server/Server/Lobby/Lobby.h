#pragma once

#include "../NetworkCore/Connection.h"
#include "../CSLock.h"
#include "LobbyUser.h"

// 로비에서 채팅을 하면 로비에 있는 유저들에게 전부 보냄

class Lobby
{
private:
	CSLock m_lock;
	std::vector<uint32>						m_vecUnusedIDs;
	std::set<uint32>						m_setLobbyUser;
	//std::unordered_set<uint32>				m_setRoomUser;
	
	std::array<LobbyUser, USER_LOBBY_MAX>	m_arrUser; 
	//std::array<Room, USER_LOBBY_MAX>		m_arrRoom;

	uint32 m_userCount;

public:
	Lobby();
	~Lobby();

	void Enter(Connection& _conn);
	LobbyUser* Find(uint32 _lobbyID);
	void Leave(uint32 _lobbyID);

	uint32 GetUserCount() const { return m_userCount; }
	void PacketUserListPage(uint32 _page, Packet& _pkt);
	
	void SendAll(const Packet& _pkt);
	void Clear(uint32 _lobbyID);
};

