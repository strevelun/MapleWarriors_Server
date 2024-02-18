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

	int64 seed = time(nullptr);
	eGameMap mapID = pRoom->GetMapID();

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::ResInitInfo)
		.Add<int64>(seed)
		.Add<int8>((int8)mapID);

	pRoom->PacketStartGameReqInitInfo(pkt);

	_conn.Send(pkt);
}

void NInGame::BeginMove(Connection& _conn, PacketReader& _packet)
{
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

void NInGame::BeginMoveMonster(Connection& _conn, PacketReader& _packet)
{
	std::wstring name = _packet.GetWString();
	uint16 pathIdx = _packet.GetUShort();
	uint16 cellXPos = _packet.GetUShort();
	uint16 cellYPos = _packet.GetUShort();

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::BeginMoveMonster)
		.AddWString(name)
		.Add<uint16>(pathIdx)
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
	std::wstring monsterName = _packet.GetWString();
	uint16 cellXPos = _packet.GetUShort();
	uint16 cellYPos = _packet.GetUShort();

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
		.AddWString(monsterName);

	pRoom->SendAll(pkt, pUser->GetRoomUserIdx());
}
