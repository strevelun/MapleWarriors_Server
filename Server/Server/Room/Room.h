#pragma once

#include "../Defines.h"
#include "../NetworkCore/Connection.h"
#include "RoomUser.h"

enum class eRoomState
{
	None,
	Standby,
	InGame,
};

class Room
{
private:
	CSLock					m_lock;

private:
	std::array<RoomUser, ROOM_USER_MAX> m_arrUser;

	uint32					m_id;
	wchar_t					m_title[ROOMTITLE_LEN];
	const wchar_t*			m_pOwnerNickname;
	uint32					m_ownerIdx;
	uint32					m_numOfUser;
	eRoomState				m_eState;

public:
	Room();
	~Room();

	void Init(Connection& _conn, const wchar_t* _pTitle, uint32 _id);
	void Clear();

	uint32 GetId() const { return m_id; }
	const wchar_t* GetTitle() const { return m_title; }
	const wchar_t* GetOwner() const { return m_pOwnerNickname; }
	uint32 GetOwnerIdx() const { return m_ownerIdx; }
	uint32 GetNumOfUser() const { return m_numOfUser; }
	eRoomState GetState() const { return m_eState; }

	bool SetMemberState(uint32 _idx, eRoomUserState _eState);

	void PacketRoomUserSlotInfo(uint32 _roomID, Packet& _pkt);

	bool Enter(Connection& _conn, User* _pUser);
	uint32 Leave(User* _pUser, uint32& _prevOwnerIdx, uint32& _newOwnerIdx);

	void SendAll(const Packet& _pkt, uint32 _exceptID = USER_NOT_IN_THE_ROOM);

private:
	uint32 FindNextOwner(uint32 _prevOwnerIdx);
};

