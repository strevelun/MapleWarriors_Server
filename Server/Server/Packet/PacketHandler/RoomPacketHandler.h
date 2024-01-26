#pragma once

#include "../../NetworkCore/Connection.h"
#include "../PacketReader.h"

namespace NRoom
{
	void RoomChat(Connection& _conn, PacketReader& _packet);
	void ExitRoom(Connection& _conn, PacketReader& _packet);
	void ReqRoomUsersInfo(Connection& _conn, PacketReader& _packet);
}

