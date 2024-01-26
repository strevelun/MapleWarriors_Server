#include "Room.h"
#include "../User/UserManager.h"

Room::Room() :
	m_title{  }, m_pOwnerNickname(nullptr), m_numOfUser(0), m_eState(eRoomState::None)
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

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());

	m_pOwnerNickname = pUser->GetNickname();
	m_ownerID = 0;
	m_arrUser[0].Init(_conn, pUser, true);
	pUser->SetRoomUserIdx(0);
	m_numOfUser = 1;
	m_eState = eRoomState::Standby;
}

void Room::Clear()
{
	m_title[0] = L'\0';
	m_pOwnerNickname = nullptr;
	m_ownerID = 0;
	m_numOfUser = 0;
	m_eState = eRoomState::None;

	m_lock.Enter();
	for (RoomUser& user : m_arrUser)
		if (user.GetState() != eRoomUserState::None)
			user.Clear();
	m_lock.Leave();
}

void Room::PacketRoomUserSlotInfo(uint32 _roomID, Packet& _pkt)
{
	m_lock.Enter();
	_pkt.Add<char>(m_numOfUser);

	uint32 idx = 0;
	for (RoomUser& user : m_arrUser)
	{
		if (user.GetState() != eRoomUserState::None)
		{
			// TODO : 캐릭터 선택 정보 추가하기
			_pkt.Add<char>(idx);
			_pkt.Add<bool>(user.IsOwner());
			_pkt.AddWString(user.GetNickname());
		}
		++idx;
	}
	m_lock.Leave();
}

bool Room::Enter(Connection& _conn, User* _pUser)
{
	m_lock.Enter();
	{
		if (m_numOfUser < ROOM_USER_MAX)
		{
			uint32 i = 0;
			for (; i < ROOM_USER_MAX; ++i)
			{
				if (m_arrUser[i].GetState() == eRoomUserState::None)
					break;
			}

			if (i == ROOM_USER_MAX)
			{
				m_lock.Leave();
				return false; // 뭔가 잘못됨
			}

			m_arrUser[i].Init(_conn, _pUser);
			_pUser->SetRoomUserIdx(i);
			++m_numOfUser;
			m_lock.Leave();
			return true;
		}
	}
	m_lock.Leave();
	return false;
}

uint32 Room::Leave(User* _pUser, uint32& _prevOwnerID, uint32& _newOwnerID)
{
	printf("Leave : %d\n", m_numOfUser);
	if (m_numOfUser == 0) return USER_NOT_IN_THE_ROOM;
	
	uint32 idx = _pUser->GetRoomUserIdx();
	if (idx >= ROOM_USER_MAX) return USER_NOT_IN_THE_ROOM;

	m_lock.Enter();
	{
		if (m_arrUser[idx].IsOwner())
		{
			_prevOwnerID = idx;
			_newOwnerID = FindNextOwner(idx);
			if (_newOwnerID != USER_NOT_IN_THE_ROOM) // 방에 방장 혼자가 아닌 경우
			{
				m_pOwnerNickname = m_arrUser[_newOwnerID].GetNickname();
				m_ownerID = _newOwnerID;
				m_arrUser[_prevOwnerID].SetOwner(false);
				m_arrUser[_newOwnerID].SetOwner(true);
			}
		}
		m_arrUser[idx].Clear();
		_pUser->SetRoomUserIdx(USER_NOT_IN_THE_ROOM);
		_pUser->SetRoomID(USER_NOT_IN_THE_ROOM);
		_pUser->SetSceneState(eSceneState::Lobby);
		
		--m_numOfUser;
	}
	m_lock.Leave();
	return m_numOfUser;
}

void Room::SendAll(const Packet& _pkt, uint32 _exceptID)
{
	m_lock.Enter();
	if(m_numOfUser != 0)
	{
		uint32 idx = 0;
		for (RoomUser& user : m_arrUser)
		{
			if (user.GetState() != eRoomUserState::None && idx != _exceptID)
				user.Send(_pkt);
			++idx;
		}
	}
	m_lock.Leave();
}

uint32 Room::FindNextOwner(uint32 _prevOwnerIdx)
{
	uint32 idx = 0;
	for (RoomUser& user : m_arrUser)
	{
		if (user.GetState() != eRoomUserState::None && idx != _prevOwnerIdx)
			return idx;
		++idx;
	}
	return USER_NOT_IN_THE_ROOM;
}

