#pragma once

#include "../../NetworkCore/Connection.h"
#include "../PacketReader.h"

namespace NLobby
{
	void LobbyChat(Connection& _conn, PacketReader& _packet);
	void LobbyUpdateInfo(Connection& _conn, PacketReader& _packet);
	void UserListGetPageInfo(Connection& _conn, PacketReader& _packet);
	void RoomListGetPageInfo(Connection& _conn, PacketReader& _packet);
	void CreateRoom(Connection& _conn, PacketReader& _packet);
	void EnterRoom(Connection& _conn, PacketReader& _packet);
}