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

	uint32 len = static_cast<uint32>(wcslen(_pTitle));
	if (len > ROOMTITLE_LEN)			len = ROOMTITLE_LEN;
	wcsncpy_s(m_title, _pTitle, len);

	m_pOwnerNickname = _pUser->GetNickname();
	m_ownerIdx = 0;
	m_arrUser[0].Init(_conn, _pUser, true);
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

	for (RoomUser& user : m_arrUser)
		if (user.GetState() != eRoomUserState::None)
			user.Clear();
}


bool Room::SetMemberState(uint32 _idx, eRoomUserState _eState)
{
	if (_idx >= ROOM_USER_MAX) return false;

	bool bSuccess = false;

	m_lock.Lock(eLockType::Writer);
	if (m_eState == eRoomState::Standby &&
		m_ownerIdx != _idx && 
		m_arrUser[_idx].GetState() != eRoomUserState::None)
	{
		m_arrUser[_idx].SetState(_eState);
		bSuccess = true;
	}
	m_lock.UnLock(eLockType::Writer);
	return bSuccess;
}

bool Room::StartGame()
{
	bool bSuccess = false;
	m_lock.Lock(eLockType::Writer);
	if (CheckAllReady())
	{
		m_eState = eRoomState::InGame;
		for (RoomUser& user : m_arrUser)
		{
			if (user.GetState() != eRoomUserState::None)
			{
				user.GameStart();
				user.SetState(eRoomUserState::Standby);
			}
		}
		bSuccess = true;
	}
	m_lock.UnLock(eLockType::Writer);

	return bSuccess;
}

void Room::GameOver()
{
	m_lock.Lock(eLockType::Reader);
	for (RoomUser user : m_arrUser)
	{
		if (user.GetState() == eRoomUserState::None) continue;

		user.GameOver();
	}
	m_lock.UnLock(eLockType::Reader);
}

void Room::PacketRoomUserSlotInfo(Packet& _pkt)
{
	m_lock.Lock(eLockType::Reader);
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
			_pkt.Add<int8>(static_cast<int8>(user.GetState()));
			_pkt.Add<int8>(static_cast<int8>(user.GetCharacterChoice()));
		}
		++idx;
	}
	m_lock.UnLock(eLockType::Reader);
}

void Room::PacketStartGameReqInitInfo(Packet& _pkt, uint32 _roomUserIdx)
{
	m_lock.Lock(eLockType::Reader);
	_pkt.Add<int8>(m_numOfUser);

	const int8* ip = nullptr;
	const uint8* privateIP = nullptr;

	uint32 idx = 0;
	for (RoomUser& user : m_arrUser)
	{
		if (user.GetState() != eRoomUserState::None)
		{
			ip = user.GetIP();
			privateIP = user.GetPrivateIP();

			if (!ip || !privateIP) continue;

			_pkt.Add<int8>(idx);
			_pkt.AddWString(user.GetNickname());
			_pkt.Add<int8>(static_cast<int8>(user.GetCharacterChoice()));
			_pkt.Add<uint16>(user.GetUDPPort());
			MakePacketIP(_pkt, user.GetIP());
			MakePacketIP(_pkt, user.GetPrivateIP());
		}
		++idx;
	}
	m_lock.UnLock(eLockType::Reader);
}

uint32 Room::Enter(Connection& _conn, User* _pUser)
{
	m_lock.Lock(eLockType::Writer);
	{
		if (m_numOfUser < ROOM_USER_MAX)
		{
			uint32 i = 0;
			for (; i < ROOM_USER_MAX; ++i)
			{
				if (m_arrUser[i].GetState() == eRoomUserState::None)
					break;
			}
			
			if (i >= ROOM_USER_MAX)
			{
				m_lock.UnLock(eLockType::Writer);
				return USER_NOT_IN_THE_ROOM;
			}

			m_arrUser[i].Init(_conn, _pUser);
			++m_numOfUser;
			m_lock.UnLock(eLockType::Writer);
			return i;
		}
	}
	m_lock.UnLock(eLockType::Writer);
	return USER_NOT_IN_THE_ROOM;
}

uint32 Room::Leave(uint32 _myRoomIdx, OUT uint32& _prevOwnerIdx, OUT uint32& _newOwnerIdx)
{
	//printf("Leave : %d\n", m_numOfUser);
	m_lock.Lock(eLockType::Reader);
	if (m_numOfUser == 0) // 나 혼자 있을 때 방을 나간 경우
	{
		m_lock.UnLock(eLockType::Reader);
		return _myRoomIdx;
	}
	m_lock.UnLock(eLockType::Reader);

	uint32 numOfUser;
	m_lock.Lock(eLockType::Writer);
	{
		if (m_arrUser[_myRoomIdx].IsOwner())
		{
			_prevOwnerIdx = _myRoomIdx;
			_newOwnerIdx = FindNextOwner(_myRoomIdx);
			if (_newOwnerIdx != USER_NOT_IN_THE_ROOM) // 방에 방장 혼자가 아닌 경우
			{
				m_pOwnerNickname = m_arrUser[_newOwnerIdx].GetNickname();
				m_ownerIdx = _newOwnerIdx;
				m_arrUser[_prevOwnerIdx].SetOwner(false);
				m_arrUser[_newOwnerIdx].SetOwner(true);
				m_arrUser[_newOwnerIdx].SetState(eRoomUserState::Ready);
			}
		}

		m_arrUser[_myRoomIdx].Clear();
		numOfUser = --m_numOfUser;
	}
	m_lock.UnLock(eLockType::Writer);
	return numOfUser;
}

void Room::SendAll(const Packet& _pkt, uint32 _exceptIdx)
{
	m_lock.Lock(eLockType::Reader);
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
	m_lock.UnLock(eLockType::Reader);
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

void Room::MakePacketIP(Packet& _pkt, const int8* _ip)
{
	std::string seg;
	std::string ip = _ip;
	std::istringstream ipStream(ip);
	while (std::getline(ipStream, seg, '.'))
	{
		_pkt.Add<uint8>(std::stoi(seg));
	}
}

void Room::MakePacketIP(Packet& _pkt, const uint8* _ip)
{
	int i = 0;
	while (i < 4)
	{
		_pkt.Add<uint8>(_ip[i++]);
	}
}