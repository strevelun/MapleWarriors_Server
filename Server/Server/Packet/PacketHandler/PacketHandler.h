#pragma once

#include "../PacketReader.h"
#include "../../NetworkCore/Connection.h"

class PacketHandler
{
public:
	static void Handle(Connection& conn, PacketReader& _packet);

private:
	static void Test(Connection& conn, PacketReader& _packet);
	static void LoginReq(Connection& conn, PacketReader& _packet);
};