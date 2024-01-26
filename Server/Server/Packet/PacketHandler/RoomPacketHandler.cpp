#include "RoomPacketHandler.h"
#include "../../Lobby/LobbyManager.h"
#include "../../User/UserManager.h"

void NRoom::RoomChat(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	const wchar_t* pChat = _packet.GetWString();
	const wchar_t* pNickname = pUser->GetNickname();

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::RoomChat)
		.AddWString(pNickname)
		.AddWString(pChat);

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	pLobby->SendRoom(pkt, pUser->GetRoomId());
}

void NRoom::ExitRoom(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	uint32 roomId = pUser->GetRoomId();
	uint32 roomUserIdx = pUser->GetRoomUserIdx();
	uint32 prevOwnerID = USER_NOT_IN_THE_ROOM;
	uint32 nextOwnerID = USER_NOT_IN_THE_ROOM;

	// 자신이 방장이면 룸을 제거
	// 방장 한 명일때 방장이 나가면 룸을 제거해야 하는데 누군가 들어온다면?
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();

	Packet pktNotifyRoomUserExit;
	pktNotifyRoomUserExit
		.Add<PacketType>((PacketType)eServer::NotifyRoomUserExit)
		.Add<char>(roomUserIdx);
	uint32 leftNum = pLobby->LeaveRoom(pUser, roomId, prevOwnerID, nextOwnerID);
	pktNotifyRoomUserExit.Add<char>(prevOwnerID);
	pktNotifyRoomUserExit.Add<char>(nextOwnerID);
	printf("%d, %d", prevOwnerID, nextOwnerID);

	if(leftNum != 0 && leftNum != ROOM_ID_NOT_FOUND)
		pLobby->SendRoom(pktNotifyRoomUserExit, roomId, roomUserIdx);

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::ExitRoom);

	_conn.Send(pkt);
}

void NRoom::ReqRoomUsersInfo(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();

	Packet pkt;
	pLobby->PacketRoomUserSlotInfo(pUser->GetRoomId(), pkt);

	_conn.Send(pkt);
}
