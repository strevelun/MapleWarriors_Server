#pragma once

#include "../Defines.h"
#include "RingBuffer.h"
#include "../CSLock.h"
#include "ConnectionManager.h"

typedef struct _tWork
{
	SOCKADDR_IN				udpAddr = {};
	int8					udpBuf[PACKET_MAX_SIZE] = {};
	WSABUF					udpDataBuf = {};
	tWSAOVERLAPPED_EX       overlapped = {};
} tWork;

class UDPHandler
{
	CSLock					m_lock;
	SOCKET					m_udpSocket;
	SOCKADDR_IN				m_udpAddr;
	uint32					m_numOfThread;
	std::vector<tWork>		m_vecWorks;

public:
	bool Init(uint32 _numOfThread);
	bool Bind();
	bool RecvFromWSA(int32 _idx);
	bool RecvReady();
	void OnRecv(int32 _idx);
	std::shared_ptr<Connection> GetConnInfo(int32 _idx, OUT uint16& _port);

	SOCKET GetSocket() const { return m_udpSocket; }

#pragma region Singleton
private:
	static UDPHandler* s_pInst;

public:
	static UDPHandler* GetInst()
	{
		if (!s_pInst)
			s_pInst = new UDPHandler;
		return s_pInst;
	}

	static void DestInst()
	{
		if (s_pInst)
			delete s_pInst;
		s_pInst = nullptr;
	}

private:
	UDPHandler();
	~UDPHandler();
#pragma endregion 
};

