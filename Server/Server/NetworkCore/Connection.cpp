#include "Connection.h"
#include "../Packet/PacketReader.h"
#include "../Packet/PacketHandler/PacketHandler.h"
#include "../User/UserManager.h"
#include "../Lobby/LobbyManager.h"

Connection::Connection(int32 _id, tAcceptedClient* _pAcceptedClient) :
    m_id(_id), m_pAcceptedClient(_pAcceptedClient), m_overlapped{}, m_gonnaBeDeleted(false)
{
	m_dataBuf.buf = m_ringBuffer.GetWriteAddr();
	m_dataBuf.len = BUFFER_MAX;
}

Connection::~Connection()
{
	if (m_pAcceptedClient->clientSocket) closesocket(m_pAcceptedClient->clientSocket);
	if(m_pAcceptedClient) delete m_pAcceptedClient;
}

void Connection::OnRecv(uint32 _recvBytes)
{
	m_ringBuffer.MoveWritePos(_recvBytes); // 3110

	PacketReader reader;
	while (reader.IsBufferReadable(m_ringBuffer))
	{
		//printf("[%d] while... : %d\n", (int32)m_socket, _recvBytes);
		reader.SetBuffer(m_ringBuffer);
		PacketHandler::Handle(*this, reader);
		//printf("[%d] handle 끝 : %d\n", (int32)m_socket, _recvBytes);
 		m_ringBuffer.MoveReadPos(reader.GetSize());
		//printf("[%d] MoveReadPos 끝 : %d\n", (int32)m_socket, reader.GetSize());
	}
	//m_ringBuffer.HandleVerge();
}

bool Connection::RecvWSA()
{
	if (m_gonnaBeDeleted) return false;

	DWORD recvBytes = 0, flags = 0;
	int32 err = 0;

	if (m_ringBuffer.SetWriteBuf(m_dataBuf) == false)
	{
		printf("SetWriteBuf failed\n");
		return false;
	}

	int32 rc = WSARecv(m_pAcceptedClient->clientSocket, &m_dataBuf, 1, &recvBytes, &flags, &m_overlapped, nullptr);
	if (rc == SOCKET_ERROR)
	{
		if ((err = WSAGetLastError()) != WSA_IO_PENDING)
		{
			printf("[%d] WSARecv Error : %d\n", (int32)m_pAcceptedClient->clientSocket, err);
			return false;
		}
		//else
		//	printf("WSA_IO_PENDING\n");
	}
	//else
		//printf("I/O started : %d\n", rc);

	//printf("RecvWSA\n");
	return true;
}

void Connection::Send(const Packet& _packet)
{
	uint16 size = _packet.GetSize();
	//printf(".");
    send(m_pAcceptedClient->clientSocket, _packet.GetBuffer(), _packet.GetSize(), 0);
	//printf("[ %d ] 보낸 바이트 : %d, 남은 처리바이트 : %d\n", (int32)m_socket, size, m_ringBuffer.GetWrittenBytes());
}