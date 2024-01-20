#include "Room.h"
#include "../User.h"

Room::Room() :
	m_title{  }, m_pOwner(nullptr), m_numOfUser(0), m_eState(eRoomState::None)
{
}

Room::~Room()
{
}

void Room::Init(Connection& _conn, const wchar_t* _pTitle, uint32 _id)
{
	m_id = _id;

	uint32 len = (uint32)wcslen(_pTitle);
	if (len > ROOMTITLE_LEN)			len = ROOMTITLE_LEN;
	wcsncpy_s(m_title, _pTitle, len);

	m_pOwner = _conn.GetUser()->GetNickname();
	m_arrUser[0].Init(_conn);
	m_numOfUser = 1;
	m_eState = eRoomState::Standby;
}

void Room::Clear()
{
	m_title[0] = L'\0';
	m_pOwner = nullptr;
	m_numOfUser = 0;
	m_eState = eRoomState::None;

	for (RoomUser& user : m_arrUser)
		user.Clear();
}
