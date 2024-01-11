#include "Connection.h"
#include "../Packet/PacketReader.h"
#include "../Packet/PacketHandler/PacketHandler.h"
#include "../UserManager.h"
#include "../Lobby/LobbyManager.h"

Connection::Connection(uint32 _id, SOCKET _socket):
    m_id(_id), m_socket(_socket), m_overlapped{}, m_pUser(nullptr)
{
	m_dataBuf.buf = m_ringBuffer.GetWriteAddr();
	m_dataBuf.len = BUFFER_MAX;
}

Connection::~Connection()
{
    if (m_socket)	closesocket(m_socket);
	if(m_pUser)		m_pUser->SetState(eLoginState::Logout);
	switch (m_eSceneState)
	{
	case eSceneState::Lobby:
	{
		Lobby* pLobby =	LobbyManager::GetInst()->GetLobby();
		pLobby->Delete(m_id);
	}
		break;
	case eSceneState::Room:
		break;
	case eSceneState::InGame:
		break;
	}
}

void Connection::OnRecv(uint32 _recvBytes)
{
	m_ringBuffer.MoveWritePos(_recvBytes);

	PacketReader reader;
	while (reader.IsBufferReadable(m_ringBuffer))
	{
		// 읽을 버퍼를 set 한다.. 
		reader.SetBuffer(m_ringBuffer);
		// 링버퍼에게 얼마나 읽었는지 알려준다.
		PacketHandler::Handle(*this, reader);
		m_ringBuffer.MoveReadPos(reader.GetSize());
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
		if ((err = WSAGetLastError()) != WSA_IO_PENDING)
		{
			printf("WSARecv Error : %d\n", err);
			return false;
		}
	}
	return true;
}

void Connection::Send(const Packet& _packet)
{
    send(m_socket, _packet.GetBuffer(), _packet.GetSize(), 0);
}