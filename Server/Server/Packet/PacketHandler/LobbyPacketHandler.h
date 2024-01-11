#pragma once

#include "../../NetworkCore/Connection.h"
#include "../PacketReader.h"

namespace NLobby
{
	void LobbyChat(Connection& _conn, PacketReader& _packet);
}