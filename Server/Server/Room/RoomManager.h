#pragma once

#include "Room.h"

class User;

class RoomManager
{
private:
	CSLock									m_lock;

	std::vector<uint32>						m_vecUnusedRoomIDs;
	std::set<uint32>						m_setRoom;
	std::array<Room, USER_LOBBY_MAX>		m_arrRoom;

	uint32									m_count;

public:
	RoomManager();
	~RoomManager();

	Room* Create(Connection& _conn, User* _pUser, const wchar_t* _pTitle);
	eEnterRoomResult Enter(Connection& _conn, User* _pUser, uint32 _roomID);
	uint32 Leave(User* _pUser, uint32 _roomID, uint32& _prevOwnerIdx, uint32 &_newOwnerIdx);
	Room* Find(uint32 _roomID);

	void MakePacketRoomListPage(uint32 _page, Packet& _pkt);
	void MakePacketUserSlotInfo(uint32 _roomID, Packet& _pkt);

	void Send(const Packet& _pkt, uint32 _roomID, uint32 _exceptID);
};

