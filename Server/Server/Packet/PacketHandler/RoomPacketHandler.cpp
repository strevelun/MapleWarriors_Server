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
	uint32 prevOwnerIdx = USER_NOT_IN_THE_ROOM;
	uint32 nextOwnerIdx = USER_NOT_IN_THE_ROOM;

	// �ڽ��� �����̸� ���� ����
	// ���� �� ���϶� ������ ������ ���� �����ؾ� �ϴµ� ������ ���´ٸ�?
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();

	Packet pktNotifyRoomUserExit;
	pktNotifyRoomUserExit
		.Add<PacketType>((PacketType)eServer::NotifyRoomUserExit)
		.Add<char>(roomUserIdx);
	uint32 leftNum = pLobby->LeaveRoom(pUser, roomId, prevOwnerIdx, nextOwnerIdx);
	pktNotifyRoomUserExit.Add<char>(prevOwnerIdx);
	pktNotifyRoomUserExit.Add<char>(nextOwnerIdx);
	printf("%d, %d", prevOwnerIdx, nextOwnerIdx);

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
	RoomManager* pRoomManager = pLobby->GetRoomManager();

	Packet pkt;
	pkt.Add<PacketType>((PacketType)eServer::RoomUsersInfo);
	pRoomManager->MakePacketUserSlotInfo(pUser->GetRoomId(), pkt);

	_conn.Send(pkt);
}

void NRoom::StartGame(Connection& _conn, PacketReader& _packet)
{
	// ������ �ƴϸ� return
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
	// �����̸� ����

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
			.Add<char>(pUser->GetRoomUserIdx())
			.Add<uint16>(_conn.GetId());
	}

	pRoom->SendAll(pkt); 
}

void NRoom::RoomStandby(Connection& _conn, PacketReader& _packet)
{
	// �����̸� ����

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
			.Add<char>(pUser->GetRoomUserIdx())
			.Add<uint16>(_conn.GetId());
	}

	pRoom->SendAll(pkt);
}
