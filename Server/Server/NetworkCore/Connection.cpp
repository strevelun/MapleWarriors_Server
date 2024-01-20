#include "Connection.h"
#include "../Packet/PacketReader.h"
#include "../Packet/PacketHandler/PacketHandler.h"
#include "../UserManager.h"
#include "../Lobby/LobbyManager.h"

Connection::Connection(int32 _id, SOCKET _socket) :
    m_id(_id), m_lobbyID(-1), m_roomID(-1), m_socket(_socket), m_overlapped{}, m_pUser(nullptr), m_gonnaBeDeleted(false)
{
	m_dataBuf.buf = m_ringBuffer.GetWriteAddr();
	m_dataBuf.len = BUFFER_MAX;
}

Connection::~Connection()
{
	if (m_pUser)
	{
		m_pUser->SetState(eLoginState::Logout);
		wprintf(L"[%s] 님 접속종료\n", m_pUser->GetNickname());
	}
	else
		printf("[%d] 님 접속종료\n", (int)m_socket);

	if (m_socket)
	{
		//shutdown(m_socket, SD_BOTH);
		//CancelIoEx((HANDLE)m_socket, nullptr);
		closesocket(m_socket);
	}
}

void Connection::OnRecv(uint32 _recvBytes)
{
	m_ringBuffer.MoveWritePos(_recvBytes); // 3110

	PacketReader reader;
	while (reader.IsBufferReadable(m_ringBuffer))
	{
		//printf("[%d] while... : %d\n", (int)m_socket, _recvBytes);
		// 읽을 버퍼를 set 한다.. 
		reader.SetBuffer(m_ringBuffer);
		// 링버퍼에게 얼마나 읽었는지 알려준다.
		PacketHandler::Handle(*this, reader);
		//printf("[%d] handle 끝 : %d\n", (int)m_socket, _recvBytes);
 		m_ringBuffer.MoveReadPos(reader.GetSize());
		//printf("[%d] MoveReadPos 끝 : %d\n", (int)m_socket, reader.GetSize());
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
    send(m_socket, _packet.GetBuffer(), _packet.GetSize(), 0);
	//printf("[ %d ] 보낸 바이트 : %d, 남은 처리바이트 : %d\n", (int)m_socket, size, m_ringBuffer.GetWrittenBytes());
}

void Connection::Leave()
{
	Lobby* pLobby = LobbyManager::GetInst()->GetLobby();
	
	switch (m_eSceneState)
	{
	case eSceneState::Room:
		pLobby->LeaveRoom(m_roomID);
	case eSceneState::Lobby:
		pLobby->Leave(m_lobbyID);
		break;
	}
}
