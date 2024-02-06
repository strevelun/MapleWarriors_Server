#pragma once
#include "../../NetworkCore/Connection.h"
#include "../PacketReader.h"

namespace NInGame
{
	void ReqInitInfo(Connection& _conn, PacketReader& _packet);
	void BeginMove(Connection& _conn, PacketReader& _packet);
	void EndMove(Connection& _conn, PacketReader& _packet);

}