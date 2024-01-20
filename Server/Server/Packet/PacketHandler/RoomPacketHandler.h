#pragma once

#include "../../NetworkCore/Connection.h"
#include "../PacketReader.h"

namespace Room
{
	void ExitRoom(Connection& _conn, PacketReader& _packet);
}

