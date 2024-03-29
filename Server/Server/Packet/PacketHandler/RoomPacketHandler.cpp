#include "RoomPacketHandler.h"
#include "../../Lobby/LobbyManager.h"
#include "../../User/UserManager.h"

void NRoom::RoomChat(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;

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
	uint32 prevOwnerIdx = USER_NOT_IN_THE_ROOM;
	uint32 nextOwnerIdx = USER_NOT_IN_THE_ROOM;

	// 자신이 방장이면 룸을 제거
	// 방장 한 명일때 방장이 나가면 룸을 제거해야 하는데 누군가 들어온다면?
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();

	Packet pktNotifyRoomUserExit;
	pktNotifyRoomUserExit
		.Add<PacketType>((PacketType)eServer::NotifyRoomUserExit)
		.Add<int8>(roomUserIdx);
	uint32 leftNum = pLobby->LeaveRoom(pUser, roomId, prevOwnerIdx, nextOwnerIdx);
	pktNotifyRoomUserExit.Add<int8>(prevOwnerIdx);
	pktNotifyRoomUserExit.Add<int8>(nextOwnerIdx);
	printf("%d, %d", prevOwnerIdx, nextOwnerIdx);

	if(leftNum != 0 && leftNum != ROOM_ID_NOT_FOUND)
		pLobby->SendRoom(pktNotifyRoomUserExit, roomId, roomUserIdx);

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::ExitRoom);

	_conn.Send(pkt);
}

// ReqRoomInfo
void NRoom::ReqRoomUsersInfo(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();
	Room* pRoom = pRoomManager->Find(pUser->GetRoomId());

	Packet pkt;
	pkt.Add<PacketType>((PacketType)eServer::RoomUsersInfo);
	pRoomManager->MakePacketUserSlotInfo(pUser->GetRoomId(), pkt);
	pkt.Add<int8>((int8)pRoom->GetMapID());


	_conn.Send(pkt);
}

void NRoom::StartGame(Connection& _conn, PacketReader& _packet)
{
	// 방장이 아니면 return
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();
	Room* pRoom = pRoomManager->Find(pUser->GetRoomId());

	Packet pkt;
	bool bSuccess = pRoom->StartGame();
	if (bSuccess)
	{
		pkt.Add<PacketType>((PacketType)eServer::StartGame_Success);
		pRoom->SendAll(pkt);
	}
	else
	{
		pkt.Add<PacketType>((PacketType)eServer::StartGame_Fail);
		_conn.Send(pkt);
	}
}

void NRoom::RoomReady(Connection& _conn, PacketReader& _packet)
{
	// 방장이면 리턴

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();
	Room* pRoom = pRoomManager->Find(pUser->GetRoomId());

	Packet pkt;
	if (!pRoom->SetMemberState(pUser->GetRoomUserIdx(), eRoomUserState::Ready))
	{
		pkt.Add<PacketType>((PacketType)eServer::RoomReady_Fail);
	}	
	else
	{
		pkt.Add<PacketType>((PacketType)eServer::RoomReady)
			.Add<int8>(pUser->GetRoomUserIdx())
			.Add<uint16>(_conn.GetId());
	}

	pRoom->SendAll(pkt); 
}

void NRoom::RoomStandby(Connection& _conn, PacketReader& _packet)
{
	// 방장이면 리턴

	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();
	Room* pRoom = pRoomManager->Find(pUser->GetRoomId());

	Packet pkt;
	if (!pRoom->SetMemberState(pUser->GetRoomUserIdx(), eRoomUserState::Standby))
	{
		pkt.Add<PacketType>((PacketType)eServer::RoomStandby_Fail);
	}
	else
	{
		pkt.Add<PacketType>((PacketType)eServer::RoomStandby)
			.Add<int8>(pUser->GetRoomUserIdx())
			.Add<uint16>(_conn.GetId());
	}

	pRoom->SendAll(pkt);
}

void NRoom::RoomMapChoice(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();
	Room* pRoom = pRoomManager->Find(pUser->GetRoomId());

	int8 mapID = _packet.GetInt8();
	pRoom->SetMapID((eGameMap)mapID);

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::RoomMapChoice)
		.Add<int8>(mapID);
	pRoom->SendAll(pkt);
}

void NRoom::RoomCharacterChoice(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();
	Room* pRoom = pRoomManager->Find(pUser->GetRoomId());

	int8 characterIdx = _packet.GetInt8();
	pRoom->SetMemberCharacterChoice(pUser->GetRoomUserIdx(), characterIdx);

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::RoomCharacterChoice)
		.Add<int8>(pUser->GetRoomUserIdx())
		.Add<int8>(characterIdx);
	pRoom->SendAll(pkt);
}
