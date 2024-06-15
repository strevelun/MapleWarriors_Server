#pragma once

#include "Room.h"

class User;

class RoomManager
{
private:
	SRWLock									m_lock;

	std::vector<uint32>						m_vecUnusedRoomIDs;
	std::set<uint32>						m_setRoom;
	std::array<Room, ROOM_MAX>				m_arrRoom;

	uint32									m_count;

public:
	RoomManager();
	~RoomManager();

	Room* Create(Connection& _conn, User* _pUser, const wchar_t* _pTitle);
	eEnterRoomResult Enter(Connection& _conn, User* _pUser, uint32 _roomID, OUT uint32& _myRoomSlotIdx);
	uint32 Leave(uint32 _myRoomIdx, uint32 _roomID, OUT uint32& _prevOwnerIdx, OUT uint32 &_newOwnerIdx);
	const Room* Find(uint32 _roomID);

	void SetRoomState(uint32 _roomID, eRoomState _eState);

	void GameStart(OUT Packet& _pkt, uint32 _roomID, uint32 _roomUserIdx);
	void GameOver(uint32 _roomID);
	void RoomReady(uint32 _roomID, uint32 _roomUserIdx, uint32 _connID);
	void RoomStandby(uint32 _roomID, uint32 _roomUserIdx, uint32 _connID);
	void RoomMapChoice(uint32 _roomID, int8 _mapID);
	void RoomCharacterChoice(uint32 _roomID, uint32 _roomUserIdx, int8 _characterIdx);

	void MakePacketRoomListPage(uint32 _page, Packet& _pkt);
	void MakePacketUserSlotInfo(uint32 _roomID, Packet& _pkt);

	void Send(const Packet& _pkt, uint32 _roomID, uint32 _exceptID);
};

