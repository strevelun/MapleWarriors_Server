#include "Room.h"
#include "../User/UserManager.h"

Room::Room() :
	m_id(), m_title{}, m_pOwnerNickname(nullptr), m_ownerIdx(0), m_numOfUser(0), m_eState(eRoomState::None), m_eMap(eGameMap::Map0)
{
}

Room::~Room()
{
}

void Room::Init(Connection& _conn, User* _pUser, const wchar_t* _pTitle, uint32 _id)
{
	m_id = _id;

	uint32 len = (uint32)wcslen(_pTitle);
	if (len > ROOMTITLE_LEN)			len = ROOMTITLE_LEN;
	wcsncpy_s(m_title, _pTitle, len);

	// 데드락 발생
	// User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());

	m_pOwnerNickname = _pUser->GetNickname();
	m_ownerIdx = 0;
	m_arrUser[0].Init(_conn, _pUser, true);
	_pUser->SetRoomUserIdx(0);
	m_numOfUser = 1;
	m_eState = eRoomState::Standby;
	m_eMap = eGameMap::Map0;
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
		bSuccess = true;
	}
	m_lock.Leave();
	return bSuccess;
}

bool Room::StartGame()
{
	bool bSuccess = false;
	m_lock.Enter();
	if (CheckAllReady())
	{
		m_eState = eRoomState::InGame;
		for (RoomUser& user : m_arrUser)
		{
			if (user.GetState() != eRoomUserState::None)
			{
				user.SetUserSceneState(eSceneState::InGame);
				user.SetState(eRoomUserState::Standby);
			}
		}
		bSuccess = true;
		//m_readyCnt = 1;
	}
	m_lock.Leave();

	return bSuccess;
}

void Room::GameOver()
{
	for (RoomUser user : m_arrUser)
	{
		if (user.GetState() == eRoomUserState::None) continue;

		user.SetUserSceneState(eSceneState::Room);
	}
}

void Room::PacketRoomUserSlotInfo(Packet& _pkt)
{
	m_lock.Enter();
	_pkt.Add<int8>(m_ownerIdx);
	_pkt.Add<int8>(m_numOfUser);

	uint32 idx = 0;
	for (RoomUser& user : m_arrUser)
	{
		if (user.GetState() != eRoomUserState::None)
		{
			_pkt.Add<uint16>(user.GetConnectionID());
			_pkt.Add<int8>(idx);
			_pkt.Add<bool>(user.IsOwner());
			_pkt.AddWString(user.GetNickname());
			_pkt.Add<int8>((int8)user.GetState());
			_pkt.Add<int8>((int8)user.GetCharacterChoice());
		}
		++idx;
	}
	m_lock.Leave();
}

void Room::PacketStartGameReqInitInfo(Packet& _pkt, uint32 _roomUserIdx)
{
	m_lock.Enter();
	_pkt.Add<int8>(m_numOfUser);

	std::string seg;
	std::string myIP = m_arrUser[_roomUserIdx].GetIP();
	std::istringstream myIPStream(myIP);

	const uint8* myPrivateIPBytes = m_arrUser[_roomUserIdx].GetPrivateIP();
	uint8 myIPBytes[4] = { 0 };
	uint32 i = 0;
	while (std::getline(myIPStream, seg, '.'))
	{
		myIPBytes[i++] = std::stoi(seg);
	}

	uint32 idx = 0;
	const uint8* otherPrivateIP = nullptr;
	for (RoomUser& user : m_arrUser)
	{
		if (user.GetState() != eRoomUserState::None)
		{
			_pkt.Add<uint16>(user.GetConnectionID());
			_pkt.Add<int8>(idx);
			_pkt.AddWString(user.GetNickname());
			_pkt.Add<int8>((int8)user.GetCharacterChoice());
			_pkt.Add<uint16>(user.GetUDPPort());
			
			std::string ip = user.GetIP();
			std::istringstream ipStream(ip);

			uint8 otherIPBytes[4] = { 0 };
			i = 0;
			while (std::getline(ipStream, seg, '.'))
			{
				otherIPBytes[i++] = std::stoi(seg);
			}

			i = 0;
			if (HasSameIP(myIPBytes, otherIPBytes))
			{
				if (_roomUserIdx == idx)
				{
					while (i < 4) _pkt.Add<uint8>(myIPBytes[i++]);
				}
				else
				{
					const uint8* otherPrivateIP = user.GetPrivateIP();
					while (i < 4) _pkt.Add<uint8>(otherPrivateIP[i++]);
				}
			}
			else
			{
				while (i < 4) _pkt.Add<uint8>(otherIPBytes[i++]);
			}
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

		m_arrUser[idx].Clear();
		_pUser->SetRoomUserIdx(USER_NOT_IN_THE_ROOM);
		_pUser->SetRoomID(USER_NOT_IN_THE_ROOM);
		_pUser->SetSceneState(eSceneState::Lobby);
		
		--m_numOfUser;
	}
	m_lock.Leave();
	return m_numOfUser;
}

void Room::SendAll(const Packet& _pkt, uint32 _exceptIdx)
{
	m_lock.Enter();
	if(m_numOfUser != 0)
	{
		uint32 idx = 0;
		for (RoomUser& user : m_arrUser)
		{
			if (user.GetState() != eRoomUserState::None && idx != _exceptIdx)
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

bool Room::CheckAllReady()
{
	for (RoomUser user : m_arrUser)
	{
		if (!user.IsOwner() && user.GetState() == eRoomUserState::Standby) return false;
	}

	return true;
}

bool Room::HasSameIP(const uint8* _myIP, const uint8* _otherIP)
{
	if (_myIP == nullptr || _otherIP == nullptr) return false;

	for (int i = 0; i < 4; ++i)
	{
		if (_myIP[i] != _otherIP[i]) return false;
	}

	return true;
}

