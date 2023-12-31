#pragma once

#include "../../Types.h"

enum class Server : uint16
{
	None = 0,
	Test,
	LoginReqOK,
	LoginReqFail,
};

enum class Client : uint16
{
	None = 0,
	Test,
	LoginReq,
};