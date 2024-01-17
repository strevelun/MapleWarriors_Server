#include "LobbyPacketHandler.h"
#include "../Packet.h"
#include "../../Lobby/LobbyManager.h"
#include "../../NetworkCore/ConnectionManager.h"

void NLobby::LobbyChat(Connection& _conn, PacketReader& _packet)
{
	const wchar_t* pNickname = _packet.GetWString();
	const wchar_t* pChat = _packet.GetWString();
	//wprintf(L"%s\n", chat);

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

	if (userCount != 0)// && userCount > userListPage * LOBBY_USERLIST_PAGE)
	{
		Packet pktUserList;
		pktUserList
			.Add<PacketType>((PacketType)eServer::LobbyUpdateInfo_UserList);

		pLobby->PacketUserListPage(userListPage, pktUserList);
		_conn.Send(pktUserList);
	}
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
}
