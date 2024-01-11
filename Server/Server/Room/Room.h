#pragma once

#include "../Defines.h"
#include "../NetworkCore/Connection.h"

class Room
{
private:
	std::array<Connection*, ROOM_USER_MAX> m_arrConn;

public:
	Room();
	~Room();
};

