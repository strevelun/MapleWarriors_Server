#pragma once

#include "../PacketReader.h"
#include "../../NetworkCore/Connection.h"

class PacketHandler
{
public:
	static void Handle(Connection& _conn, PacketReader& _packet);
};