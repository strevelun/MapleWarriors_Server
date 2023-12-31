#pragma once

#include "../PacketReader.h"

class PacketHandler
{
public:
	static void Handle(PacketReader& _packet);

private:
#pragma region Test
	static void Test(PacketReader& _packet);
#pragma endregion

#pragma region Login
	static void LoginReq(PacketReader& _packet);
#pragma endregion
};