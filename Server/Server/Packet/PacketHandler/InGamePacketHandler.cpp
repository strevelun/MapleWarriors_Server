#include "InGamePacketHandler.h"
#include "../../Lobby/LobbyManager.h"
#include "../../User/UserManager.h"

void NInGame::ReqInitInfo(Connection& _conn, PacketReader& _packet)
{
	// 맵정보, 닉네임, 캐릭터 선택, 
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (!pLobby) return;

	RoomManager* pRoomManager = pLobby->GetRoomManager();
	Room* pRoom = pRoomManager->Find(pUser->GetRoomId());
	if (!pRoom) return;

	//int64 seed = time(nullptr);
	eGameMap mapID = pRoom->GetMapID();

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::ResInitInfo)
		.Add<int8>((int8)mapID);

	pRoom->PacketStartGameReqInitInfo(pkt);

	_conn.Send(pkt);
}

void NInGame::BeginMove(Connection& _conn, PacketReader& _packet)
{
	int32 xpos = _packet.GetInt32();
	int32 ypos = _packet.GetInt32();
	int8 dir = _packet.GetInt8();

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;
	
	int8 roomSlot = pUser->GetRoomUserIdx();

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (!pLobby) return;

	Room* pRoom = pLobby->GetRoomManager()->Find(pUser->GetRoomId());

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::BeginMove)
		.Add<int8>(roomSlot)
		.Add<int32>(xpos)
		.Add<int32>(ypos)
		.Add<int8>(dir);

	//Sleep(1000);

	pRoom->SendAll(pkt, pUser->GetRoomUserIdx());
}

void NInGame::EndMove(Connection& _conn, PacketReader& _packet)
{
	int32 xpos = _packet.GetInt32();
	int32 ypos = _packet.GetInt32();

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;

	int8 roomSlot = pUser->GetRoomUserIdx();

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (!pLobby) return;

	Room* pRoom = pLobby->GetRoomManager()->Find(pUser->GetRoomId());

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::EndMove)
		.Add<int8>(roomSlot)
		.Add<int32>(xpos)
		.Add<int32>(ypos);

	//Sleep(1000);

	pRoom->SendAll(pkt, pUser->GetRoomUserIdx());
}

void NInGame::MonsterAttack(Connection& _conn, PacketReader& _packet)
{
	int8 targetCnt = _packet.GetInt8();
	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::MonsterAttack)
		.Add<int8>(targetCnt);

	for (int32 i = 0; i < targetCnt; ++i)
		pkt.Add<int8>(_packet.GetInt8());

	int8 monsterIdx = _packet.GetInt8();
	int8 monsterNum = _packet.GetInt8();
	pkt
		.Add<int8>(monsterIdx)
		.Add<int8>(monsterNum);

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;

	int8 roomSlot = pUser->GetRoomUserIdx();

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (!pLobby) return;

	Room* pRoom = pLobby->GetRoomManager()->Find(pUser->GetRoomId());

	pRoom->SendAll(pkt);
}

void NInGame::BeginMoveMonster(Connection& _conn, PacketReader& _packet)
{
	int8 monsterIdx = _packet.GetInt8();
	int8 monsterNum = _packet.GetInt8();
	uint16 cellXPos = _packet.GetUShort();
	uint16 cellYPos = _packet.GetUShort();

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::BeginMoveMonster)
		.Add<int8>(monsterIdx)
		.Add<int8>(monsterNum)
		.Add<uint16>(cellXPos)
		.Add<uint16>(cellYPos);

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (!pLobby) return;

	Room* pRoom = pLobby->GetRoomManager()->Find(pUser->GetRoomId());
	pRoom->SendAll(pkt);
}

void NInGame::Attack(Connection& _conn, PacketReader& _packet)
{
	uint16 count = _packet.GetUShort();

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;

	int8 roomSlot = pUser->GetRoomUserIdx();

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (!pLobby) return;

	Room* pRoom = pLobby->GetRoomManager()->Find(pUser->GetRoomId());

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::Attack)
		.Add<int8>(roomSlot)
		.Add<uint16>(count);

	int8 monsterIdx;
	int8 monsterNum;

	for (int i = 0; i < count; ++i)
	{
		monsterIdx = _packet.GetInt8();
		monsterNum = _packet.GetInt8();
		pkt
			.Add<int8>(monsterIdx)
			.Add<int8>(monsterNum);
	}

	int8 skill = _packet.GetInt8();
	pkt.Add<uint8>(skill);

	pRoom->SendAll(pkt, pUser->GetRoomUserIdx());
}

void NInGame::RangedAttack(Connection& _conn, PacketReader& _packet)
{
	uint16 count = _packet.GetUShort();
	int16 x = _packet.GetShort();
	int16 y = _packet.GetShort();

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;

	int8 roomSlot = pUser->GetRoomUserIdx();

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (!pLobby) return;

	Room* pRoom = pLobby->GetRoomManager()->Find(pUser->GetRoomId());

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::RangedAttack)
		.Add<int8>(roomSlot)
		.Add<uint16>(count)
		.Add<int16>(x)
		.Add<int16>(y);

	int8 monsterIdx;
	int8 monsterNum;

	for (int i = 0; i < count; ++i)
	{
		monsterIdx = _packet.GetInt8();
		monsterNum = _packet.GetInt8();
		pkt
			.Add<int8>(monsterIdx)
			.Add<int8>(monsterNum);
	}

	int8 skill = _packet.GetInt8();
	pkt.Add<uint8>(skill);

	pRoom->SendAll(pkt, pUser->GetRoomUserIdx());
}

void NInGame::GameOver(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;

	int8 roomSlot = pUser->GetRoomUserIdx();

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (!pLobby) return;

	Room* pRoom = pLobby->GetRoomManager()->Find(pUser->GetRoomId());

	pRoom->SetState(eRoomState::Standby);
	Packet pkt;
	pkt.
		Add<PacketType>((PacketType)eServer::GameOver);
	pRoom->SendAll(pkt);
}
