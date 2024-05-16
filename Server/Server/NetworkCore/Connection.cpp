#include "Connection.h"
#include "../Packet/PacketReader.h"
#include "../Packet/PacketHandler/PacketHandler.h"
#include "../User/UserManager.h"
#include "../Lobby/LobbyManager.h"
#include "ConnectionManager.h"

Connection::Connection(int32 _id, tAcceptedClient* _pAcceptedClient) :
    m_id(_id), m_pAcceptedClient(_pAcceptedClient), m_overlapped{}
{
	m_dataBuf.buf = m_ringBuffer.GetWriteAddr();
	m_dataBuf.len = BUFFER_MAX;
}

Connection::~Connection()
{
	if (m_pAcceptedClient->clientSocket) closesocket(m_pAcceptedClient->clientSocket);
	if(m_pAcceptedClient) delete m_pAcceptedClient;
}

void Connection::SetPrivateIP(uint8 _a, uint8 _b, uint8 _c, uint8 _d)
{
	m_pAcceptedClient->privateIPAddr[0] = _a;
	m_pAcceptedClient->privateIPAddr[1] = _b;
	m_pAcceptedClient->privateIPAddr[2] = _c;
	m_pAcceptedClient->privateIPAddr[3] = _d;
}

void Connection::OnRecv(uint32 _recvBytes)
{
	m_ringBuffer.MoveWritePos(_recvBytes); 

	PacketReader reader;
	while (reader.IsBufferReadable(m_ringBuffer)) // 무한 루프
	{
		reader.SetBuffer(m_ringBuffer);
		PacketHandler::Handle(*this, reader);
 		m_ringBuffer.MoveReadPos(reader.GetSize());
	}

	RecvWSA();
}

void Connection::RecvWSA()
{
	DWORD recvBytes = 0, flags = 0;
	int32 err = 0;

	m_ringBuffer.SetWriteBuf(m_dataBuf);

	int32 rc = WSARecv(m_pAcceptedClient->clientSocket, &m_dataBuf, 1, &recvBytes, &flags, &m_overlapped, nullptr);
	if (rc == SOCKET_ERROR)
	{
		err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			printf("[%d] WSARecv Error : %d", (int32)m_pAcceptedClient->clientSocket, err);
			switch (err)
			{
			case WSAECONNRESET:
				printf(" (클라이언트가 이미 소켓을 닫았습니다.)");
				break;
			}
			printf("\n");
		}
	}
}

void Connection::Send(const Packet& _packet)
{
    send(m_pAcceptedClient->clientSocket, _packet.GetBuffer(), _packet.GetSize(), 0);
}
