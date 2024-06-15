#include "UDPHandler.h"
#include "../Packet/PacketHandler/PacketHandler.h"
#include "../Defines.h"

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
		printf("UDP ���� ���� ���� : %d\n", static_cast<int32>(m_udpSocket));
		return false;
	}

	m_numOfThread = _numOfThread;

	m_udpAddr.sin_family = AF_INET;
	m_udpAddr.sin_port = ::htons(SERVER_PORT);
	m_udpAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);

	return true;
}

bool UDPHandler::Bind()
{
	int32 result = ::bind(m_udpSocket, reinterpret_cast<SOCKADDR*>(&m_udpAddr), sizeof(m_udpAddr));
	if (result == SOCKET_ERROR)
	{
		int32 err = ::WSAGetLastError();
		printf("UDP Bind ���� : %d\n", err);
		return false;
	}

	return true;
}

void UDPHandler::OnRecv(int32 _idx)
{
	uint16 port;

	uint32 id = GetConnInfo(_idx, OUT port);
	Connection* pConn = ConnectionManager::GetInst()->Get(id);
	if (!pConn) 
	{
		// �̹� ������ Connection
		ConnectionManager::GetInst()->Release(id);
		return;
	}

	m_lock.Enter();
	if (pConn->GetUDPPort() != 0)
	{
		m_lock.Leave();
		ConnectionManager::GetInst()->Release(id);
		return;
	}
	pConn->SetMyUDPPort(ntohs(port));

	m_lock.Leave();
	ConnectionManager::GetInst()->Release(id);

	Packet pkt;
	pkt
		.Add(static_cast<PacketType>(eServer::CheckedClientInfo))
		.Add<uint16>(port);
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

	int32 result = ::WSARecvFrom(m_udpSocket, &m_vecWorks[_idx].udpDataBuf, 1, nullptr, &flags, reinterpret_cast<SOCKADDR*>(&m_vecWorks[_idx].udpAddr), &addrSize, &m_vecWorks[_idx].overlapped, nullptr);
	
	if (result == SOCKET_ERROR)
	{
		int32 err = ::WSAGetLastError();
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

uint32 UDPHandler::GetConnInfo(int32 _idx, OUT uint16& _port)
{
	_port = m_vecWorks[_idx].udpAddr.sin_port;
	return *reinterpret_cast<uint32*>(&m_vecWorks[_idx].udpBuf);
}
