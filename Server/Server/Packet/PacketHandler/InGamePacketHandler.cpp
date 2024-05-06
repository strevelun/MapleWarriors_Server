#include "InGamePacketHandler.h"
#include "../../Lobby/LobbyManager.h"
#include "../../User/UserManager.h"

void NInGame::ReqInitInfo(Connection& _conn, PacketReader& _packet)
{
	// ������, �г���, ĳ���� ����, 
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return; // User�� ���� ���� ��������

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (!pLobby) return;

	RoomManager* pRoomManager = pLobby->GetRoomManager();
	Room* pRoom = pRoomManager->Find(pUser->GetRoomId());
	if (!pRoom) return;

	eGameMap mapID = pRoom->GetMapID();

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::ResInitInfo)
		.Add<int8>((int8)mapID);
	
	pRoom->PacketStartGameReqInitInfo(pkt, pUser->GetRoomUserIdx());

	_conn.Send(pkt);
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
