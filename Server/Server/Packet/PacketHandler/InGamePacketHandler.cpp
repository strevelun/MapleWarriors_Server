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
	int8 roomSlot = _packet.GetInt8();
	int8 dir = _packet.GetInt8();

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (!pLobby) return;

	Room* pRoom = pLobby->GetRoomManager()->Find(pUser->GetRoomId());

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::BeginMove)
		.Add<int8>(roomSlot)
		.Add<int8>(dir);

	pRoom->SendAll(pkt, pUser->GetRoomUserIdx());
}

void NInGame::EndMove(Connection& _conn, PacketReader& _packet)
{
	int8 roomSlot = _packet.GetInt8();
	int32 xpos = _packet.GetInt32();
	int32 ypos = _packet.GetInt32();

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (!pLobby) return;

	Room* pRoom = pLobby->GetRoomManager()->Find(pUser->GetRoomId());

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::EndMove)
		.Add<int8>(roomSlot)
		.Add<int32>(xpos)
		.Add<int32>(ypos);

	pRoom->SendAll(pkt, pUser->GetRoomUserIdx());
}
