#include "PacketHandler.h"
#include "../Types/PacketTypes.h"
#include "../../UserManager.h"
#include "LoginPacketHandler.h"

void PacketHandler::Handle(Connection& _conn, PacketReader& _packet)
{
	switch (_packet.Get<Client>())
	{
#pragma region Login
	case Client::Test:
		Login::Test(_conn, _packet);
		break;

	case Client::LoginReq:
		Login::LoginReq(_conn, _packet);
		break;

#pragma endregion

#pragma region Lobby

#pragma endregion
	}
}