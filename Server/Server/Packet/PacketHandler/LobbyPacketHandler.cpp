#include "LobbyPacketHandler.h"
#include "../Packet.h"
#include "../../Lobby/LobbyManager.h"
#include "../../NetworkCore/ConnectionManager.h"
#include "../../User/UserManager.h"

void NLobby::LobbyChat(Connection& _conn, PacketReader& _packet)
{
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());

	const wchar_t* pChat = _packet.GetWString();
	const wchar_t* pNickname = pUser->GetNickname();

	Packet pkt;
	pkt
		.Add<PacketType>(static_cast<PacketType>(eServer::LobbyChat))
		.AddWString(pNickname)
		.AddWString(pChat);

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	pLobby->SendAllInLobby(pkt);
}

void NLobby::LobbyUpdateInfo(Connection& _conn, PacketReader& _packet)
{
	//printf("[ %d ] : LobbyUpdateInfo - start\n", (int32)_conn.GetSocket());
	int8 userListPage = _packet.GetInt8();
	int8 roomListPage = _packet.GetInt8();
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();
	uint32 userCount = pLobby->GetUserCount();

	if (userCount != 0)// && userCount > userListPage * LOBBY_USERLIST_PAGE)
	{
		Packet pktUserList;
		pLobby->PacketUserListPage(userListPage, pktUserList);	
		_conn.Send(pktUserList);
	}

	Packet pktRoomList;
	pktRoomList.Add<PacketType>(static_cast<PacketType>(eServer::LobbyUpdateInfo_RoomList));
	pRoomManager->MakePacketRoomListPage(roomListPage, pktRoomList);

	_conn.Send(pktRoomList);
}

void NLobby::UserListGetPageInfo(Connection& _conn, PacketReader& _packet)
{
	int8 userListPage = _packet.GetInt8();
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	uint32 userCount = pLobby->GetUserCount();

	if (userCount == 0) return;

	Packet pktUserList;
	pLobby->PacketUserListPage(userListPage, pktUserList);
	_conn.Send(pktUserList);
}

void NLobby::RoomListGetPageInfo(Connection& _conn, PacketReader& _packet)
{
	int8 roomListPage = _packet.GetInt8();
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	RoomManager* pRoomManager = pLobby->GetRoomManager();

	Packet pktRoomList;
	pktRoomList.Add<PacketType>(static_cast<PacketType>(eServer::LobbyUpdateInfo_RoomList));
	pRoomManager->MakePacketRoomListPage(roomListPage, pktRoomList);
	_conn.Send(pktRoomList);
}

void NLobby::CreateRoom(Connection& _conn, PacketReader& _packet)
{
	const wchar_t* pTitle = _packet.GetWString();
	Packet pkt;
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	if (!pUser) return;

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	Room* pRoom = pLobby->CreateRoom(_conn, pUser, pTitle);
	if (pRoom)
	{
		pkt.Add<PacketType>(static_cast<PacketType>(eServer::CreateRoom_Success));
		pkt.Add<uint32>(pRoom->GetId());
		pkt.AddWString(pTitle);
	}
	else // 방 만들기 실패
	{
		pkt.Add<PacketType>(static_cast<PacketType>(eServer::CreateRoom_Fail));
	}

	_conn.Send(pkt);
	//wprintf(L"[%s] CreateRoom\n", pUser->GetNickname());
}

void NLobby::EnterRoom(Connection& _conn, PacketReader& _packet)
{
	uint32 roomID = _packet.GetUShort();

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	User* pUser = UserManager::GetInst()->FindConnectedUser(_conn.GetId());
	eEnterRoomResult eResult = pLobby->EnterRoom(_conn, pUser, roomID);

	Packet pkt;

	switch (eResult)
	{
	case eEnterRoomResult::Success:
	{
		pkt.Add<PacketType>(static_cast<PacketType>(eServer::EnterRoom_Success));

		uint32 idx = pUser->GetRoomUserIdx();
		Packet pktNotifyRoomUserEnter;
		pktNotifyRoomUserEnter
			.Add<PacketType>(static_cast<PacketType>(eServer::NotifyRoomUserEnter))
			.Add<int8>(idx)
			.AddWString(pUser->GetNickname());
		pLobby->SendRoom(pktNotifyRoomUserEnter, roomID, idx);
		break;
	}
	case eEnterRoomResult::Full:
		pkt.Add<PacketType>(static_cast<PacketType>(eServer::EnterRoom_Full));
		break;
	case eEnterRoomResult::InGame:
		pkt.Add<PacketType>(static_cast<PacketType>(eServer::EnterRoom_InGame));
		break;
	case eEnterRoomResult::NoRoom:
		pkt.Add<PacketType>(static_cast<PacketType>(eServer::EnterRoom_NoRoom));
		break;
	default:
		return;
	}
	_conn.Send(pkt);
	//wprintf(L"[%s] EnterRoom : %d\n", pUser->GetNickname(), (int32)eResult);
}
