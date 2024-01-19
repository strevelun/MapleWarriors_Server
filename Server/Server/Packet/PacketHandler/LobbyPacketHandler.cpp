#include "LobbyPacketHandler.h"
#include "../Packet.h"
#include "../../Lobby/LobbyManager.h"
#include "../../NetworkCore/ConnectionManager.h"
#include "../../User.h"

void NLobby::LobbyChat(Connection& _conn, PacketReader& _packet)
{
	const wchar_t* pChat = _packet.GetWString();
	//wprintf(L"%s\n", chat);
	const wchar_t* pNickname = _conn.GetUser()->GetNickname();

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::LobbyChat)
		.AddWString(pNickname)
		.AddWString(pChat);

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	pLobby->SendAll(pkt);
}

void NLobby::LobbyUpdateInfo(Connection& _conn, PacketReader& _packet)
{
	//printf("[ %d ] : LobbyUpdateInfo - start\n", (int)_conn.GetSocket());
	char userListPage = _packet.Get<char>();
	char roomListPage = _packet.Get<char>();
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	uint32 userCount = pLobby->GetUserCount();
	uint32 roomCount = pLobby->GetRoomCount();

	if (userCount != 0)// && userCount > userListPage * LOBBY_USERLIST_PAGE)
	{
		Packet pktUserList;
		pktUserList
			.Add<PacketType>((PacketType)eServer::LobbyUpdateInfo_UserList);

		pLobby->PacketUserListPage(userListPage, pktUserList);
		_conn.Send(pktUserList);
	}

	//if (roomCount != 0)
	//{
		Packet pktRoomList;
		pktRoomList
			.Add<PacketType>((PacketType)eServer::LobbyUpdateInfo_RoomList);

		pLobby->PacketRoomListPage(roomListPage, pktRoomList);
		_conn.Send(pktRoomList);
	//}
}

void NLobby::UserListGetPageInfo(Connection& _conn, PacketReader& _packet)
{
	char userListPage = _packet.Get<char>();
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	uint32 userCount = pLobby->GetUserCount();

	if (userCount == 0) return;
	//if (userCount <= userListPage * LOBBY_USERLIST_PAGE) return; // 10명 뿐인데 1페이지를 보여줄 수 없다

	Packet pktUserList;
	pktUserList
		.Add<PacketType>((PacketType)eServer::LobbyUpdateInfo_UserList);

	pLobby->PacketUserListPage(userListPage, pktUserList);
	_conn.Send(pktUserList);
}

void NLobby::RoomListGetPageInfo(Connection& _conn, PacketReader& _packet)
{
	char roomListPage = _packet.Get<char>();
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	uint32 roomCount = pLobby->GetRoomCount();

	if (roomCount == 0) return;

	Packet pktRoomList;
	pktRoomList
		.Add<PacketType>((PacketType)eServer::LobbyUpdateInfo_RoomList);

	pLobby->PacketRoomListPage(roomListPage, pktRoomList);
	_conn.Send(pktRoomList);
}

void NLobby::CreateRoom(Connection& _conn, PacketReader& _packet)
{
	const wchar_t* pTitle = _packet.GetWString();
	Packet pkt;

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	Room* pRoom = pLobby->CreateRoom(_conn, pTitle);
	if (pRoom)
	{
		_conn.SetSceneState(eSceneState::Room);

		pkt.Add<PacketType>((PacketType)eServer::CreateRoom);
		pkt.Add<char>(pRoom->GetId());
	}
	else
	{

	}


	_conn.Send(pkt);
}
