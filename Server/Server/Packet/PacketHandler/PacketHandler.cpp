#include "PacketHandler.h"
#include "../Types/PacketTypes.h"

// TODO : 상속으로 
void PacketHandler::Handle(Connection& conn, PacketReader& _packet)
{
	Client type = _packet.Get<Client>();
	switch (type)
	{
	case Client::Test:
		Test(conn, _packet);
		break;

	case Client::LoginReq:
		LoginReq(conn, _packet);
		break;
	}
}

void PacketHandler::Test(Connection& conn, PacketReader& _packet)
{
	//printf("Test\n");
}

void PacketHandler::LoginReq(Connection& conn, PacketReader& _packet)
{
	const wchar_t* str = _packet.GetWString();
	int result = _packet.Get<char>();
	wprintf(L"%s\n", str);

	Packet packet;
	packet
		.Add<PacketType>((PacketType)Server::LoginReqOK)
		.AddWString(str)
		.Add<char>(result);
	conn.Send(packet);

	printf("LoginReq\n");
}
