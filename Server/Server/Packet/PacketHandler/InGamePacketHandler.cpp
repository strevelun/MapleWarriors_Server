#include "InGamePacketHandler.h"


void NInGame::ReqInitInfo(Connection& _conn, PacketReader& _packet)
{
	// ������, �г���, ĳ���� ����, 

	Packet pkt;
	pkt.Add<PacketType>((PacketType)eServer::ResInitInfo);
}
