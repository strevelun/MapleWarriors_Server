#include "LobbyPacketHandler.h"
#include "../Packet.h"
#include "../../Lobby/LobbyManager.h"

void NLobby::LobbyChat(Connection& _conn, PacketReader& _packet)
{
	const wchar_t* chat = _packet.GetWString();
	wprintf(L"%s\n", chat);

	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)Server::LobbyChat)
		.AddWString(chat);

	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	pLobby->SendAll(pkt);
}
