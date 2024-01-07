#pragma once

#include "../../Types.h"

enum class Server : PacketType
{
	None = 0,
	Test,
	LoginFailure,
	LoginSuccess,
};

enum class Client : PacketType
{
	None = 0,
	Test,
	LoginReq,
};