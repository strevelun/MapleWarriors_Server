#include "UDPHandler.h"
#include "ConnectionManager.h"
#include "../Packet/PacketHandler/PacketHandler.h"

UDPHandler* UDPHandler::s_pInst = nullptr;

UDPHandler::UDPHandler() :
	m_udpSocket{ 0 }, m_udpAddr{0}, m_numOfThread(0)
{
}

UDPHandler::~UDPHandler()
{
	if (m_udpSocket)
	{
		::closesocket(m_udpSocket);
		m_udpSocket = INVALID_SOCKET;
	}
}

bool UDPHandler::Init(uint32 _numOfThread)
{
	m_udpSocket = ::WSASocketW(AF_INET, SOCK_DGRAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (m_udpSocket == INVALID_SOCKET)
	{
		printf("UDP 소켓 생성 에러 : %d\n", (int32)m_udpSocket);
		return false;
	}

	m_numOfThread = _numOfThread;

	m_udpAddr.sin_family = AF_INET;
	m_udpAddr.sin_port = ::htons(30001);
	m_udpAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);

	return true;
}

bool UDPHandler::Bind()
{
	int32 result = ::bind(m_udpSocket, (SOCKADDR*)&m_udpAddr, sizeof(m_udpAddr));
	if (result == SOCKET_ERROR)
	{
		int32 err = ::WSAGetLastError();
		printf("UDP Bind 실패 : %d\n", err);
		return false;
	}

	return true;
}

void UDPHandler::OnRecv(int32 _idx)
{
	uint16 port;
	uint32 id;

	GetConnInfo(_idx, port, id);

	Connection* pConn = ConnectionManager::GetInst()->Get(id);
	if (!pConn) 
	{
		// 이미 삭제된 Connection
		pConn->Release();
		return;
	}

	m_lock.Enter();
	if (pConn->GetUDPPort() != 0)
	{
		pConn->Release();
		m_lock.Leave();
		return;
	}
	pConn->SetMyUDPPort(ntohs(port));

	pConn->Release();
	m_lock.Leave();

	Packet pkt;
	pkt
		.Add((PacketType)eServer::CheckedClientInfo)
		.Add((uint16)port);
	pConn->Send(pkt);

	RecvFromWSA(_idx);
}

bool UDPHandler::RecvFromWSA(int32 _idx)
{
	DWORD flags = 0;
	int32 addrSize = sizeof(m_vecWorks[_idx].udpAddr);
	memset(&m_vecWorks[_idx], 0, sizeof(m_vecWorks[_idx]));
	m_vecWorks[_idx].overlapped.udpIdx = _idx;
	m_vecWorks[_idx].overlapped.connType = eConnType::UDP;
	m_vecWorks[_idx].udpDataBuf.buf = m_vecWorks[_idx].udpBuf;
	m_vecWorks[_idx].udpDataBuf.len = PACKET_MAX_SIZE;

	int32 result = ::WSARecvFrom(m_udpSocket, &m_vecWorks[_idx].udpDataBuf, 1, nullptr, &flags, (SOCKADDR*)&m_vecWorks[_idx].udpAddr, &addrSize, &m_vecWorks[_idx].overlapped, nullptr);
	
	if (result == SOCKET_ERROR)
	{
		int err = ::WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			printf("WSARecvFrom err : %d\n", err);
			return false;
		}
	}
	return true;
}

bool UDPHandler::RecvReady()
{
	m_vecWorks.assign(m_numOfThread, tWork());
	for (uint32 i = 0; i < m_numOfThread; ++i)
	{
		if (!RecvFromWSA(i)) return false;
	}
	return true;
}

void UDPHandler::GetConnInfo(int32 _idx, uint16& _port, uint32& _id)
{
	_port = m_vecWorks[_idx].udpAddr.sin_port;
	_id = *reinterpret_cast<const uint32*>(&m_vecWorks[_idx].udpBuf);
}
