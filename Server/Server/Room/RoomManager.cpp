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
	m_lock.Enter();
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
	m_lock.Leave();

	return pRoom;
}

eEnterRoomResult RoomManager::Enter(Connection& _conn, User* _pUser, uint32 _roomID, OUT uint32& _myRoomSlotIdx)
{
	m_lock.Enter();

	eEnterRoomResult eResult = eEnterRoomResult::None;
	auto iter = m_setRoom.find(_roomID);
	if (iter == m_setRoom.cend()) eResult = eEnterRoomResult::NoRoom;
	else if (m_arrRoom[*iter].GetState() == eRoomState::InGame)	eResult = eEnterRoomResult::InGame;

	if (eResult != eEnterRoomResult::None)
	{
		m_lock.Leave();
		return eResult;
	}
	m_lock.Leave();

	_myRoomSlotIdx = m_arrRoom[*iter].Enter(_conn, _pUser);

	if (_myRoomSlotIdx != USER_NOT_IN_THE_ROOM) eResult = eEnterRoomResult::Success;
	else eResult = eEnterRoomResult::Full;

	return eResult;
}

uint32 RoomManager::Leave(uint32 _myRoomIdx, uint32 _roomID, OUT uint32& _prevOwnerIdx, OUT uint32 &_newOwnerIdx)
{
	uint32 leftNum = ROOM_ID_NOT_FOUND;

	m_lock.Enter(); 

	if (m_setRoom.find(_roomID) == m_setRoom.cend())
	{
		m_lock.Leave();
		return leftNum;
	}
	
	if (m_arrRoom[_roomID].GetNumOfUser() == 1)
	{
		m_arrRoom[_roomID].Clear();
		m_vecUnusedRoomIDs.push_back(_roomID);
		m_setRoom.erase(_roomID);
		--m_count;
	}
	m_lock.Leave();

	leftNum = m_arrRoom[_roomID].Leave(_myRoomIdx, OUT _prevOwnerIdx, OUT _newOwnerIdx);

    return leftNum;
}

Room* RoomManager::Find(uint32 _roomID)
{
	if (_roomID >= ROOM_MAX) return nullptr;

	Room* pRoom = nullptr;
	m_lock.Enter();
	auto iter = m_setRoom.find(_roomID);
	if (iter != m_setRoom.end())
	{
		pRoom = &m_arrRoom[_roomID];
	}
	m_lock.Leave();

	return pRoom;
}

void RoomManager::SetRoomState(uint32 _roomID, eRoomState _eState)
{
	if (_roomID >= ROOM_MAX) return;

	m_lock.Enter();
	auto iter = m_setRoom.find(_roomID);
	if (iter != m_setRoom.end())
	{
		m_arrRoom[_roomID].SetState(_eState);
	}
	m_lock.Leave();
}

void RoomManager::MakePacketRoomListPage(uint32 _page, Packet& _pkt)
{
	m_lock.Enter();
	{
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
	}
	m_lock.Leave();
}

void RoomManager::MakePacketUserSlotInfo(uint32 _roomID, Packet& _pkt)
{
	m_lock.Enter();
	auto iter = m_setRoom.find(_roomID);
	if (iter == m_setRoom.cend())
	{
		m_lock.Leave();
		return;
	}

	_pkt.Add<uint16>(m_arrRoom[*iter].GetId());
	_pkt.AddWString(m_arrRoom[*iter].GetTitle());

	m_lock.Leave();

	m_arrRoom[*iter].PacketRoomUserSlotInfo(_pkt);
}

void RoomManager::Send(const Packet& _pkt, uint32 _roomID, uint32 _exceptID)
{
	m_lock.Enter();
	{
		auto iter = m_setRoom.find(_roomID);
		if (iter != m_setRoom.cend())
		{
			m_arrRoom[*iter].SendAll(_pkt, _exceptID);
		}
	}
	m_lock.Leave();
}
