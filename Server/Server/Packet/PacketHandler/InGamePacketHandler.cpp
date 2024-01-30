#include "InGamePacketHandler.h"


void NInGame::ReqInitInfo(Connection& _conn, PacketReader& _packet)
{
	// 맵정보, 닉네임, 캐릭터 선택, 

	Packet pkt;
	pkt.Add<PacketType>((PacketType)eServer::ResInitInfo);
}
