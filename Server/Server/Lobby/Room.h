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
	std::array<RoomUser, ROOM_USER_MAX> m_arrUser;

	uint32			m_id;
	wchar_t			m_title[ROOMTITLE_LEN];
	const wchar_t* m_pOwner;
	uint32				m_numOfUser;
	eRoomState				m_eState;

public:
	Room();
	~Room();

	void Init(Connection& _conn, const wchar_t* _pTitle, uint32 _id);
	void Clear();

	uint32 GetId() const { return m_id; }
	const wchar_t* GetTitle() const { return m_title; }
	const wchar_t* GetOwner() const { return m_pOwner; }
	uint32 GetNumOfUser() const { return m_numOfUser; }
	eRoomState GetState() const { return m_eState; }

};

