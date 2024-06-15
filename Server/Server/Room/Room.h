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
	SRWLock					m_lock;

private:
	std::array<RoomUser, ROOM_USER_MAX> m_arrUser;

	uint32					m_id;
	wchar_t					m_title[ROOMTITLE_LEN];
	const wchar_t*			m_pOwnerNickname;
	uint32					m_ownerIdx;
	uint32					m_numOfUser;
	eRoomState				m_eState;
	eGameMap				m_eMap;

public:
	Room();
	~Room();

	void Init(Connection& _conn, User* _pUser, const wchar_t* _pTitle, uint32 _id);
	void Clear();

	uint32 GetId() const { return m_id; }
	const wchar_t* GetTitle() const { return m_title; }
	const wchar_t* GetOwner() const { return m_pOwnerNickname; }
	uint32 GetOwnerIdx() const { return m_ownerIdx; }
	uint32 GetNumOfUser() const { return m_numOfUser; }
	eRoomState GetState() const { return m_eState; }
	eGameMap GetMapID() const { return m_eMap; }
	//eCharacterChoice GetCharacterChoice(uint32 _memberIdx) const { return m_arrUser[_memberIdx].GetCharacterChoice(); } // 범위 체크

	// 게임이 시작된 상태에서 Set은 불가능
	bool SetMemberState(uint32 _idx, eRoomUserState _eState);
	void SetState(eRoomState _eState) { m_eState = _eState; }
	void SetMapID(eGameMap _mapID) { m_eMap = _mapID; }
	void SetMemberCharacterChoice(uint32 _memberIdx, uint32 _characterIdx) { m_arrUser[_memberIdx].SetCharacterChoice(static_cast<eCharacterChoice>(_characterIdx)); }

	bool StartGame();
	void GameOver();

	void PacketRoomUserSlotInfo(Packet& _pkt); // change name
	void PacketStartGameReqInitInfo(Packet& _pkt, uint32 _roomUserIdx);

	uint32 Enter(Connection& _conn, User* _pUser);
	uint32 Leave(uint32 _myRoomIdx, OUT uint32& _prevOwnerIdx, OUT uint32& _newOwnerIdx);

	void SendAll(const Packet& _pkt, uint32 _exceptIdx = USER_NOT_IN_THE_ROOM);

private:
	uint32 FindNextOwner(uint32 _prevOwnerIdx);
	bool CheckAllReady();
	void MakePacketIP(Packet& _pkt, const int8* _ip);
	void MakePacketIP(Packet& _pkt, const uint8* _ip);
};

