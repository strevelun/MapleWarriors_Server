#pragma once

#include "../NetworkCore/Connection.h"
#include "../CSLock.h"
#include "LobbyUser.h"
#include "Room.h"

// 로비에서 채팅을 하면 로비에 있는 유저들에게 전부 보냄

class Lobby
{
private:
	CSLock m_userLock;
	CSLock m_roomLock;
	std::vector<uint32>						m_vecUnusedUserIDs;
	std::set<uint32>						m_setLobbyUser;

	std::vector<uint32>						m_vecUnusedRoomIDs;
	std::set<uint32>						m_setRoom;
	
	std::array<LobbyUser, USER_LOBBY_MAX>	m_arrUser; 
	std::array<Room, USER_LOBBY_MAX>		m_arrRoom;

	uint32 m_userCount;
	uint32 m_roomCount;

public:
	Lobby();
	~Lobby();

	void Enter(Connection& _conn);
	LobbyUser* Find(uint32 _lobbyID);
	void Leave(uint32 _lobbyID);

	uint32 GetUserCount() const { return m_userCount; }
	uint32 GetRoomCount() const { return m_roomCount; }
	void PacketUserListPage(uint32 _page, Packet& _pkt);
	void PacketRoomListPage(uint32 _page, Packet& _pkt);
	
	void SendAll(const Packet& _pkt);

	Room* CreateRoom(Connection& _conn, const wchar_t* _pTitle);
	Room* FindRoom(uint32 _roomID);
	void LeaveRoom(uint32 _roomID);
};

