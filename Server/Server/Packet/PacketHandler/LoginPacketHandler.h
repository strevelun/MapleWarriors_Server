#pragma once

#include "../../NetworkCore/Connection.h"
#include "../PacketReader.h"

namespace Login
{
	void Test(Connection& _conn, PacketReader& _packet);
	void LoginReq(Connection& _conn, PacketReader& _packet);
}