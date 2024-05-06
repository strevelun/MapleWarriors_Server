#pragma once
#include "../../NetworkCore/Connection.h"
#include "../PacketReader.h"

namespace NInGame
{
	void ReqInitInfo(Connection& _conn, PacketReader& _packet);
	void GameOver(Connection& _conn, PacketReader& _packet);
}