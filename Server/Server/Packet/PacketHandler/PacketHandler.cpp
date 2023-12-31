#include "PacketHandler.h"
#include "../Types/PacketTypes.h"

void PacketHandler::Handle(PacketReader& _packet)
{
	Client type = _packet.Get<Client>();
	switch (type)
	{
	case Client::Test:
		Test(_packet);
		break;

	case Client::LoginReq:
		LoginReq(_packet);
		break;
	}
}

void PacketHandler::Test(PacketReader& _packet)
{
	//printf("Test\n");
}

void PacketHandler::LoginReq(PacketReader& _packet)
{
	printf("LoginReq\n");
}
