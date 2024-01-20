#include "RoomPacketHandler.h"

void Room::ExitRoom(Connection& _conn, PacketReader& _packet)
{
	Packet pkt;
	pkt
		.Add<PacketType>((PacketType)eServer::ExitRoom);

	_conn.Send(pkt);
}
