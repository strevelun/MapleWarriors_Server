#include "InGamePacketHandler.h"
#include "../../Lobby/LobbyManager.h"
#include "../../User/UserManager.h"

void NInGame::GameOver(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;

	int8 roomSlot = pUser->GetRoomUserIdx();

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	if (!pLobby) return;

	Room* pRoom = pLobby->GetRoomManager()->Find(pUser->GetRoomId());

	pRoom->SetState(eRoomState::Standby);
	pRoom->GameOver();

	Packet pkt;
	pkt.
		Add<PacketType>((PacketType)eServer::GameOver);
	pRoom->SendAll(pkt);
}
