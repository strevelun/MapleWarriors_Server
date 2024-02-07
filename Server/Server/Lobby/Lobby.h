#pragma once

#include "../NetworkCore/Connection.h"
#include "../CSLock.h"
#include "LobbyUser.h"
#include "../Room/RoomManager.h"

// 로비에서 채팅을 하면 로비에 있는 유저들에게 전부 보냄

struct _stLobbyUser
{
	uint32 lobbyID;
	uint32 connID;

	_stLobbyUser(uint32 _lobbyID, uint32 _connID) : lobbyID(_lobbyID), connID(_connID) {}
	bool operator< (const _stLobbyUser& _other) const { return connID < _other.connID; }
};

class Lobby
{
private:
	RoomManager								m_roomManager;

private:
	CSLock m_lock;
	std::vector<uint32>						m_vecUnusedUserIDs;
	std::set<_stLobbyUser>			m_setAllLobbyUser;
	std::unordered_set<uint32>				m_usetUserInLobby;
	
	std::array<LobbyUser, USER_LOBBY_MAX>	m_arrUser;

	uint32 m_userCount;

public:
	Lobby();
	~Lobby();

	void Enter(Connection& _conn, User* _pUser);
	LobbyUser* Find(uint32 _lobbyID, uint32 _connID);
	void Leave(uint32 _lobbyID, uint32 _connID);

	uint32 GetUserCount() const { return m_userCount; }
	RoomManager* GetRoomManager() { return &m_roomManager; }

	void PacketUserListPage(uint32 _page, Packet& _pkt);
	
	void Send(const Packet& _pkt, uint32 _userID);
	void SendAllInLobby(const Packet& _pkt);

	Room* CreateRoom(Connection& _conn, User* _pUser, const wchar_t* _pTitle);
	eEnterRoomResult EnterRoom(Connection& _conn, User* _pUser, uint32 _roomID);
	uint32 LeaveRoom(User* _pUser, uint32 _roomID, uint32& _prevOwnerIdx, uint32& _newOwnerIdx);

	void SendRoom(const Packet& _pkt, uint32 _roomID, uint32 _exceptID = USER_NOT_IN_THE_ROOM);

private:
	//Room* FindRoom(uint32 _roomID);
};

