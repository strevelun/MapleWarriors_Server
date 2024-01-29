#include "Room.h"
#include "../User/UserManager.h"

Room::Room() :
	m_id(), m_title{}, m_pOwnerNickname(nullptr), m_ownerIdx(0), m_numOfUser(0), m_eState(eRoomState::None), m_readyCnt(0), m_eMap(eGameMap::None)
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
	m_ownerIdx = 0;
	m_arrUser[0].Init(_conn, pUser, true);
	pUser->SetRoomUserIdx(0);
	m_numOfUser = 1;
	m_eState = eRoomState::Standby;
	m_readyCnt = 1; // 항상 방장때문에 1로 초기화
	m_eMap = eGameMap::Test;
}

void Room::Clear()
{
	m_title[0] = L'\0';
	m_pOwnerNickname = nullptr;
	m_ownerIdx = 0;
	m_numOfUser = 0;
	m_eState = eRoomState::None;

	m_lock.Enter();
	for (RoomUser& user : m_arrUser)
		if (user.GetState() != eRoomUserState::None)
			user.Clear();
	m_lock.Leave();
}


bool Room::SetMemberState(uint32 _idx, eRoomUserState _eState)
{
	if (_idx >= ROOM_USER_MAX) return false;

	bool bSuccess = false;

	m_lock.Enter();
	if (m_eState == eRoomState::Standby &&
		m_ownerIdx != _idx && 
		m_arrUser[_idx].GetState() != eRoomUserState::None)
	{
		m_arrUser[_idx].SetState(_eState);
		if (_eState == eRoomUserState::Ready) ++m_readyCnt;
		else if (_eState == eRoomUserState::Standby) --m_readyCnt;
		bSuccess = true;
	}
	m_lock.Leave();
	return bSuccess;
}

bool Room::StartGame()
{
	bool bSuccess = false;
	m_lock.Enter();
	if (m_readyCnt == m_numOfUser)
	{
		m_eState = eRoomState::InGame;
		for (RoomUser& user : m_arrUser)
		{
			if (user.GetState() != eRoomUserState::None)
			{
				user.SetUserSceneState(eSceneState::InGame);
			}
		}
		bSuccess = true;
	}
	m_lock.Leave();

	return bSuccess;
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
			_pkt.Add<uint16>(user.GetConnectionID());
			_pkt.Add<char>(idx);
			_pkt.Add<bool>(user.IsOwner());
			_pkt.AddWString(user.GetNickname());
			_pkt.Add<char>((char)user.GetState());
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

uint32 Room::Leave(User* _pUser, uint32& _prevOwnerIdx, uint32& _newOwnerIdx)
{
	printf("Leave : %d\n", m_numOfUser);
	if (m_numOfUser == 0) return USER_NOT_IN_THE_ROOM;
	
	uint32 idx = _pUser->GetRoomUserIdx();
	if (idx >= ROOM_USER_MAX) return USER_NOT_IN_THE_ROOM;

	m_lock.Enter();
	{
		if (m_arrUser[idx].IsOwner())
		{
			_prevOwnerIdx = idx;
			_newOwnerIdx = FindNextOwner(idx);
			if (_newOwnerIdx != USER_NOT_IN_THE_ROOM) // 방에 방장 혼자가 아닌 경우
			{
				m_pOwnerNickname = m_arrUser[_newOwnerIdx].GetNickname();
				m_ownerIdx = _newOwnerIdx;
				m_arrUser[_prevOwnerIdx].SetOwner(false);
				m_arrUser[_newOwnerIdx].SetOwner(true);
				m_arrUser[_newOwnerIdx].SetState(eRoomUserState::Ready);
			}
		}
		if (m_arrUser[idx].GetState() == eRoomUserState::Ready) --m_readyCnt;

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

