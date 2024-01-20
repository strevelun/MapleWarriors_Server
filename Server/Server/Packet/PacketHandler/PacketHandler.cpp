#include "PacketHandler.h"
#include "../Types/PacketTypes.h"
#include "../../UserManager.h"
#include "LoginPacketHandler.h"
#include "LobbyPacketHandler.h"

void PacketHandler::Handle(Connection& _conn, PacketReader& _packet)
{
	eClient type = _packet.Get<eClient>();
	//printf("[%d] Before Handle : %d\n", (int)_conn.GetSocket(), type);
	switch (type)
	{
#pragma region Login
	case eClient::Test:
		Login::Test(_conn, _packet);
		break;

	case eClient::Exit:
		Login::Exit(_conn, _packet);
		break;

	case eClient::LoginReq:
		Login::LoginReq(_conn, _packet);
		break;

#pragma endregion

#pragma region Lobby
	case eClient::LobbyChat:
		NLobby::LobbyChat(_conn, _packet);
		break;
	case eClient::LobbyUpdateInfo:
		NLobby::LobbyUpdateInfo(_conn, _packet);
		break;
	case eClient::UserListGetPageInfo:
		NLobby::UserListGetPageInfo(_conn, _packet);
		break;
	case eClient::RoomListGetPageInfo:
		NLobby::RoomListGetPageInfo(_conn, _packet);
		break;
	case eClient::CreateRoom:
		NLobby::CreateRoom(_conn, _packet);
		break;
#pragma endregion
	}
	//printf("[%d] Handle over\n", (int)_conn.GetSocket());
}