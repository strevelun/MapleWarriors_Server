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
		.Add<PacketType>(static_cast<PacketType>(eServer::RoomChat))
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
	uint32 prevOwnerIdx = USER_NOT_IN_THE_ROOM;
	uint32 nextOwnerIdx = USER_NOT_IN_THE_ROOM;

	// �ڽ��� �����̸� ���� ����
	// ���� �� ���϶� ������ ������ ���� �����ؾ� �ϴµ� ������ ���´ٸ�?
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();

	Packet pktNotifyRoomUserExit;
	pktNotifyRoomUserExit
		.Add<PacketType>(static_cast<PacketType>(eServer::NotifyRoomUserExit))
		.Add<int8>(roomUserIdx);
	uint32 leftNum = pLobby->LeaveRoom(pUser, roomId, OUT prevOwnerIdx, OUT nextOwnerIdx);
	pktNotifyRoomUserExit.Add<int8>(prevOwnerIdx);
	pktNotifyRoomUserExit.Add<int8>(nextOwnerIdx);
	printf("%d, %d", prevOwnerIdx, nextOwnerIdx);

	if(leftNum != 0 && leftNum != ROOM_ID_NOT_FOUND)
		pLobby->SendRoom(pktNotifyRoomUserExit, roomId, roomUserIdx);

	Packet pkt;
	pkt
		.Add<PacketType>(static_cast<PacketType>(eServer::ExitRoom));

	_conn.Send(pkt);
}

// ReqRoomInfo
void NRoom::ReqRoomUsersInfo(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();
	const Room* pRoom = pRoomManager->Find(pUser->GetRoomId());

	Packet pkt;
	pkt.Add<PacketType>(static_cast<PacketType>(eServer::RoomUsersInfo));
	pRoomManager->MakePacketUserSlotInfo(pUser->GetRoomId(), pkt);
	pkt.Add<int8>(static_cast<int8>(pRoom->GetMapID()));

	_conn.Send(pkt);
}

void NRoom::StartGame(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();

	Packet pkt;
	pRoomManager->GameStart(OUT pkt, pUser->GetRoomId(), pUser->GetRoomUserIdx());

	_conn.Send(pkt);
}

void NRoom::RoomReady(Connection& _conn, PacketReader& _packet)
{
	// �����̸� ����

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();

	pRoomManager->RoomReady(pUser->GetRoomId(), pUser->GetRoomUserIdx(), _conn.GetId());
	
}

void NRoom::RoomStandby(Connection& _conn, PacketReader& _packet)
{
	// �����̸� ����

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();
	pRoomManager->RoomStandby(pUser->GetRoomId(), pUser->GetRoomUserIdx(), _conn.GetId());
}

void NRoom::RoomMapChoice(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();
	
	int8 mapID = _packet.GetInt8();
	pRoomManager->RoomMapChoice(pUser->GetRoomId(), mapID);
}

void NRoom::RoomCharacterChoice(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();

	int8 characterIdx = _packet.GetInt8();
	pRoomManager->RoomCharacterChoice(pUser->GetRoomId(), pUser->GetRoomUserIdx(), characterIdx);
}
