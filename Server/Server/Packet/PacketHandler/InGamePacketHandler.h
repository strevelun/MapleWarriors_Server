#pragma once
#include "../../NetworkCore/Connection.h"
#include "../PacketReader.h"

namespace NInGame
{
	void GameOver(Connection& _conn, PacketReader& _packet);
}