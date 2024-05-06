#include "PacketHandler.h"
#include "../Types/PacketTypes.h"
#include "LoginPacketHandler.h"
#include "LobbyPacketHandler.h"
#include "RoomPacketHandler.h"
#include "InGamePacketHandler.h"

void PacketHandler::Handle(Connection& _conn, PacketReader& _packet)
{
	eClient type = (eClient)_packet.GetPacketType();
	//printf("[%d] Before Handle : %d\n", (int32)_conn.GetSocket(), type);
	switch (type)
	{
#pragma region Login
	case eClient::Test:
		NLogin::Test(_conn, _packet);
		break;
	case eClient::LoginReq:
		NLogin::LoginReq(_conn, _packet);
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
	case eClient::EnterRoom:
		NLobby::EnterRoom(_conn, _packet);
		break;
#pragma endregion

#pragma region Room
	case eClient::RoomChat:
		NRoom::RoomChat(_conn, _packet);
		break;
	case eClient::ExitRoom:
		NRoom::ExitRoom(_conn, _packet);
		break;	
	case eClient::ReqRoomUsersInfo:
		NRoom::ReqRoomUsersInfo(_conn, _packet);
		break;
	case eClient::StartGame:
		NRoom::StartGame(_conn, _packet);
		break;
	case eClient::RoomReady:
		NRoom::RoomReady(_conn, _packet);
		break;
	case eClient::RoomStandby:
		NRoom::RoomStandby(_conn, _packet);
		break;
	case eClient::RoomMapChoice:
		NRoom::RoomMapChoice(_conn, _packet);
		break;
	case eClient::RoomCharacterChoice:
		NRoom::RoomCharacterChoice(_conn, _packet);
		break;
#pragma endregion

#pragma region InGame
	case eClient::ReqInitInfo:
		NInGame::ReqInitInfo(_conn, _packet);
		break;
	case eClient::GameOver:
		NInGame::GameOver(_conn, _packet);
		break;
#pragma endregion
	}
	//printf("[%d] Handle over\n", (int32)_conn.GetSocket());
}