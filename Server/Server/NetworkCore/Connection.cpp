#include "Connection.h"
#include "../Packet/PacketReader.h"
#include "../Packet/PacketHandler/PacketHandler.h"

Connection::Connection(uint32 _id, SOCKET _socket):
    m_id(_id), m_socket(_socket), m_overlapped{}
{
	m_dataBuf.buf = m_ringBuffer.GetWriteAddr();
	m_dataBuf.len = BUFFER_MAX;
}

Connection::~Connection()
{
    if (m_socket) closesocket(m_socket);
}

void Connection::OnRecv(uint32 _recvBytes)
{

	m_ringBuffer.MoveWritePos(_recvBytes);

	PacketReader reader;
	while (reader.IsBufferReadable(m_ringBuffer))
	{
		reader.SetBuffer(m_ringBuffer);
		m_ringBuffer.MoveReadPos(reader.GetPacketSize());
		PacketHandler::Handle(reader);
	}
	m_ringBuffer.HandleVerge();
}

bool Connection::RecvWSA()
{
	DWORD recvBytes = 0, flags = 0;
	int err = 0;

	if (m_ringBuffer.SetWriteBuf(m_dataBuf) == false)
	{
		printf("SetWriteBuf failed\n");
		return false;
	}

	int rc = WSARecv(m_socket, &m_dataBuf, 1, &recvBytes, &flags, &m_overlapped, nullptr);
	if (rc == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING != (err = WSAGetLastError()))
		{
			printf("WSARecv Error : %d\n", err);
			return false;
		}
	}
	return true;
}

void Connection::Send(const Packet& _packet)
{
    send(m_socket, _packet.GetBuffer(), _packet.GetAddedSize(), 0);
}
