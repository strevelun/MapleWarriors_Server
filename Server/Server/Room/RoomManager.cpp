#include "RoomManager.h"
#include "../User/UserManager.h"

RoomManager::RoomManager() :
	m_vecUnusedRoomIDs(ROOM_MAX), m_count(0)
{
	uint32 j = ROOM_MAX - 1;
	for (uint32 i = 0; i < ROOM_MAX; ++i, --j)
		m_vecUnusedRoomIDs[i] = j;
}

RoomManager::~RoomManager()
{
}

Room* RoomManager::Create(Connection& _conn, User* _pUser, const wchar_t* _pTitle)
{
	Room* pRoom = nullptr;
	uint32 roomId = ROOM_ID_NOT_FOUND;
	m_lock.Lock(eLockType::Writer);
	{
		if (m_count < ROOM_MAX)
		{
			roomId = m_vecUnusedRoomIDs.back();
			m_vecUnusedRoomIDs.pop_back();
			m_setRoom.insert(roomId);
			m_arrRoom[roomId].Init(_conn, _pUser, _pTitle, roomId);
			pRoom = &m_arrRoom[roomId];
			++m_count;
		}
	}
	m_lock.UnLock(eLockType::Writer);

	return pRoom;
}

eEnterRoomResult RoomManager::Enter(Connection& _conn, User* _pUser, uint32 _roomID, OUT uint32& _myRoomSlotIdx)
{
	m_lock.Lock(eLockType::Reader);
	eEnterRoomResult eResult = eEnterRoomResult::None;
	auto iter = m_setRoom.find(_roomID);
	if (iter == m_setRoom.end()) eResult = eEnterRoomResult::NoRoom;
	else if (m_arrRoom[*iter].GetState() == eRoomState::InGame)	eResult = eEnterRoomResult::InGame;

	if (eResult != eEnterRoomResult::None)
	{
		m_lock.UnLock(eLockType::Reader);
		return eResult;
	}

	_myRoomSlotIdx = m_arrRoom[*iter].Enter(_conn, _pUser);
	m_lock.UnLock(eLockType::Reader);

	if (_myRoomSlotIdx != USER_NOT_IN_THE_ROOM) eResult = eEnterRoomResult::Success;
	else eResult = eEnterRoomResult::Full;

	return eResult;
}

uint32 RoomManager::Leave(uint32 _myRoomIdx, uint32 _roomID, OUT uint32& _prevOwnerIdx, OUT uint32 &_newOwnerIdx)
{
	uint32 leftNum = ROOM_ID_NOT_FOUND;

	m_lock.Lock(eLockType::Reader);
	if (m_setRoom.find(_roomID) == m_setRoom.end())
	{
		m_lock.UnLock(eLockType::Reader);
		return leftNum;
	}
	m_lock.UnLock(eLockType::Reader);

	m_lock.Lock(eLockType::Writer);
	if (m_arrRoom[_roomID].GetNumOfUser() == 1)
	{
		m_arrRoom[_roomID].Clear();
		m_vecUnusedRoomIDs.push_back(_roomID);
		m_setRoom.erase(_roomID);
		--m_count;
	}
	m_lock.UnLock(eLockType::Writer);

	m_lock.Lock(eLockType::Reader);
	leftNum = m_arrRoom[_roomID].Leave(_myRoomIdx, OUT _prevOwnerIdx, OUT _newOwnerIdx);
	m_lock.UnLock(eLockType::Reader);

    return leftNum;
}

const Room* RoomManager::Find(uint32 _roomID)
{
	if (_roomID >= ROOM_MAX) return nullptr;

	Room* pRoom = nullptr;
	m_lock.Lock(eLockType::Reader);
	auto iter = m_setRoom.find(_roomID);
	if (iter != m_setRoom.end())
	{
		pRoom = &m_arrRoom[_roomID];
	}
	m_lock.UnLock(eLockType::Reader);

	return pRoom;
}

void RoomManager::SetRoomState(uint32 _roomID, eRoomState _eState)
{
	if (_roomID >= ROOM_MAX) return;

	m_lock.Lock(eLockType::Reader);
	auto iter = m_setRoom.find(_roomID);
	if (iter != m_setRoom.end())
	{
		m_arrRoom[_roomID].SetState(_eState);
	}
	m_lock.UnLock(eLockType::Reader);
}

void RoomManager::GameStart(OUT Packet& _pkt, uint32 _roomID, uint32 _roomUserIdx)
{
	m_lock.Lock(eLockType::Reader);
	bool bSuccess = m_arrRoom[_roomID].StartGame();
	m_lock.UnLock(eLockType::Reader);

	if (bSuccess)
	{
		m_lock.Lock(eLockType::Reader);
		eGameMap mapID = m_arrRoom[_roomID].GetMapID();

		_pkt.Add<PacketType>(static_cast<PacketType>(eServer::StartGame_Success))
			.Add<int8>(static_cast<int8>(mapID));
		m_arrRoom[_roomID].PacketStartGameReqInitInfo(_pkt, _roomUserIdx);
		m_arrRoom[_roomID].SendAll(_pkt);
		m_lock.UnLock(eLockType::Reader);
	}
	else
	{
		_pkt.Add<PacketType>(static_cast<PacketType>(eServer::StartGame_Fail));
	}
}

void RoomManager::GameOver(uint32 _roomID)
{
	if (_roomID >= ROOM_MAX) return;

	m_lock.Lock(eLockType::Reader);

	auto iter = m_setRoom.find(_roomID);
	if (iter == m_setRoom.end())
	{
		m_lock.UnLock(eLockType::Reader);
		return;
	}

	m_arrRoom[_roomID].SetState(eRoomState::Standby);
	m_arrRoom[_roomID].GameOver();

	Packet pkt;
	pkt.
		Add<PacketType>(static_cast<PacketType>(eServer::GameOver));
	m_arrRoom[_roomID].SendAll(pkt);
	m_lock.UnLock(eLockType::Reader);
}

void RoomManager::RoomReady(uint32 _roomID, uint32 _roomUserIdx, uint32 _connID)
{
	if (_roomID >= ROOM_MAX) return;

	m_lock.Lock(eLockType::Reader);

	Packet pkt;
	auto iter = m_setRoom.find(_roomID);
	if (iter == m_setRoom.end())
	{
		m_lock.UnLock(eLockType::Reader);
		return;
	}

	if (!m_arrRoom[_roomID].SetMemberState(_roomUserIdx, eRoomUserState::Ready))
	{
		pkt.Add<PacketType>(static_cast<PacketType>(eServer::RoomReady_Fail));
	}
	else
	{
		pkt.Add<PacketType>(static_cast<PacketType>(eServer::RoomReady))
			.Add<int8>(_roomUserIdx)
			.Add<uint16>(_connID);
	}

	m_arrRoom[_roomID].SendAll(pkt);
	m_lock.UnLock(eLockType::Reader);
}

void RoomManager::RoomStandby(uint32 _roomID, uint32 _roomUserIdx, uint32 _connID)
{
	if (_roomID >= ROOM_MAX) return;

	m_lock.Lock(eLockType::Reader);

	Packet pkt;
	auto iter = m_setRoom.find(_roomID);
	if (iter == m_setRoom.end())
	{
		m_lock.UnLock(eLockType::Reader);
		return;
	}

	if (!m_arrRoom[_roomID].SetMemberState(_roomUserIdx, eRoomUserState::Standby))
	{
		pkt.Add<PacketType>(static_cast<PacketType>(eServer::RoomStandby_Fail));
	}
	else
	{
		pkt.Add<PacketType>(static_cast<PacketType>(eServer::RoomStandby))
			.Add<int8>(_roomUserIdx)
			.Add<uint16>(_connID);
	}

	m_arrRoom[_roomID].SendAll(pkt);
	m_lock.UnLock(eLockType::Reader);
}

void RoomManager::RoomMapChoice(uint32 _roomID, int8 _mapID)
{
	if (_roomID >= ROOM_MAX) return;

	m_lock.Lock(eLockType::Reader);
	auto iter = m_setRoom.find(_roomID);
	if (iter == m_setRoom.end())
	{
		m_lock.UnLock(eLockType::Reader);
		return;
	}

	m_arrRoom[_roomID].SetMapID(static_cast<eGameMap>(_mapID));

	Packet pkt;
	pkt
		.Add<PacketType>(static_cast<PacketType>(eServer::RoomMapChoice))
		.Add<int8>(_mapID);
	m_arrRoom[_roomID].SendAll(pkt);
	m_lock.UnLock(eLockType::Reader);
}

void RoomManager::RoomCharacterChoice(uint32 _roomID, uint32 _roomUserIdx, int8 _characterIdx)
{
	if (_roomID >= ROOM_MAX) return;

	m_lock.Lock(eLockType::Reader);
	auto iter = m_setRoom.find(_roomID);
	if (iter == m_setRoom.end())
	{
		m_lock.UnLock(eLockType::Reader);
		return;
	}

	m_arrRoom[_roomID].SetMemberCharacterChoice(_roomUserIdx, _characterIdx);

	Packet pkt;
	pkt
		.Add<PacketType>(static_cast<PacketType>(eServer::RoomCharacterChoice))
		.Add<int8>(_roomUserIdx)
		.Add<int8>(_characterIdx);
	m_arrRoom[_roomID].SendAll(pkt);
	m_lock.UnLock(eLockType::Reader);
}

void RoomManager::MakePacketRoomListPage(uint32 _page, Packet& _pkt)
{
	m_lock.Lock(eLockType::Reader);
	
	uint32 result;
	if (m_count > 0 && m_count % LOBBY_ROOMLIST_PAGE == 0)
	{
		if (_page > 0 && _page >= m_count / LOBBY_ROOMLIST_PAGE)
			_page = m_count / LOBBY_ROOMLIST_PAGE - 1;
		result = LOBBY_ROOMLIST_PAGE;
	}
	else
	{
		if (_page > 0 && _page > m_count / LOBBY_ROOMLIST_PAGE)
			_page = m_count / LOBBY_ROOMLIST_PAGE;
		result = (m_count / LOBBY_ROOMLIST_PAGE == _page) ? m_count % LOBBY_ROOMLIST_PAGE : LOBBY_ROOMLIST_PAGE;
	}

	int32 startPos = LOBBY_ROOMLIST_PAGE * _page;

	_pkt.Add<int8>(_page);
	_pkt.Add<int8>(result);

	uint32 roomID = 0;
	uint32 count = 0;
	std::set<uint32>::iterator iter = m_setRoom.begin();

	if (_page != 0)		std::advance(iter, startPos);

	std::set<uint32>::iterator iterEnd = m_setRoom.end();
	for (; iter != iterEnd; ++count, ++iter)
	{
		if (count >= result) break;

		roomID = *iter;
		_pkt.Add<uint16>(roomID);
		_pkt.AddWString(m_arrRoom[roomID].GetTitle());
		_pkt.AddWString(m_arrRoom[roomID].GetOwner());
		_pkt.Add<int8>(m_arrRoom[roomID].GetNumOfUser());
		_pkt.Add<int8>(static_cast<int8>(m_arrRoom[roomID].GetState()));
	}

	m_lock.UnLock(eLockType::Reader);
}

void RoomManager::MakePacketUserSlotInfo(uint32 _roomID, Packet& _pkt)
{
	m_lock.Lock(eLockType::Reader);
	auto iter = m_setRoom.find(_roomID);
	if (iter == m_setRoom.end())
	{
		m_lock.UnLock(eLockType::Reader);
		return;
	}

	_pkt.Add<uint16>(m_arrRoom[*iter].GetId());
	_pkt.AddWString(m_arrRoom[*iter].GetTitle());

	m_arrRoom[*iter].PacketRoomUserSlotInfo(_pkt);
	m_lock.UnLock(eLockType::Reader);
}

void RoomManager::Send(const Packet& _pkt, uint32 _roomID, uint32 _exceptID)
{
	m_lock.Lock(eLockType::Reader);
	auto iter = m_setRoom.find(_roomID);
	if (iter != m_setRoom.end())
	{
		m_arrRoom[*iter].SendAll(_pkt, _exceptID);
	}
	m_lock.UnLock(eLockType::Reader);
}
