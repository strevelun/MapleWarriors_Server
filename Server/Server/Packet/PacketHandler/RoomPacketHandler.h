#pragma once

#include "../../NetworkCore/Connection.h"
#include "../PacketReader.h"

namespace NRoom
{
	void RoomChat(Connection& _conn, PacketReader& _packet);
	void ExitRoom(Connection& _conn, PacketReader& _packet);
	void ReqRoomUsersInfo(Connection& _conn, PacketReader& _packet);
	void StartGame(Connection& _conn, PacketReader& _packet);
	void RoomReady(Connection& _conn, PacketReader& _packet);
	void RoomStandby(Connection& _conn, PacketReader& _packet);
	void RoomMapChoice(Connection& _conn, PacketReader& _packet);
	void RoomCharacterChoice(Connection& _conn, PacketReader& _packet);
}

