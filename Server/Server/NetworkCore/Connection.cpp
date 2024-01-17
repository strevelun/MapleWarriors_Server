#include "Connection.h"
#include "../Packet/PacketReader.h"
#include "../Packet/PacketHandler/PacketHandler.h"
#include "../UserManager.h"
#include "../Lobby/LobbyManager.h"

Connection::Connection(int32 _id, SOCKET _socket) :
    m_id(_id), m_lobbyID(-1), m_socket(_socket), m_overlapped{}, m_pUser(nullptr), m_gonnaBeDeleted(false)
{
	m_dataBuf.buf = m_ringBuffer.GetWriteAddr();
	m_dataBuf.len = BUFFER_MAX;
}

Connection::~Connection()
{
	if (m_pUser)
	{
		m_pUser->SetState(eLoginState::Logout);
		wprintf(L"[%s] �� ��������\n", m_pUser->GetNickname());
	}
	else
		printf("[%d] �� ��������\n", (int)m_socket);

	if (m_socket)
	{
		//shutdown(m_socket, SD_BOTH);
		//CancelIoEx((HANDLE)m_socket, nullptr);
		closesocket(m_socket);
	}
}

void Connection::OnRecv(uint32 _recvBytes)
{
	m_ringBuffer.MoveWritePos(_recvBytes);

	PacketReader reader;
	while (reader.IsBufferReadable(m_ringBuffer))
	{
		// ���� ���۸� set �Ѵ�.. 
		reader.SetBuffer(m_ringBuffer);
		// �����ۿ��� �󸶳� �о����� �˷��ش�.
		PacketHandler::Handle(*this, reader);
 		m_ringBuffer.MoveReadPos(reader.GetSize());
	}
	m_ringBuffer.HandleVerge();
}

bool Connection::RecvWSA()
{
	if (m_gonnaBeDeleted) return false;

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
			printf("[%d] WSARecv Error : %d\n", (int)m_socket, err);
			return false;
		}
	}
	return true;
}

void Connection::Send(const Packet& _packet)
{
	uint16 size = _packet.GetSize();
    send(m_socket, _packet.GetBuffer(), _packet.GetSize(), 0);
	//printf("[ %d ] ���� ����Ʈ : %d\n", (int)m_socket, size);
}

void Connection::Leave()
{
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	
	if (m_eSceneState != eSceneState::Login)
	{
		pLobby->Leave(m_lobbyID);
	}
}
