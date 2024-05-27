#pragma once

#include "../Defines.h"
#include "../Types.h"
#include "../Packet/Packet.h"
#include "../Packet/PacketReader.h"
#include "RingBuffer.h"
#include "../CSLock.h"

class Connection
{
private:
    std::atomic<int32>               m_ref; 

    uint32               m_id;
    tAcceptedClient*    m_pAcceptedClient;
    RingBuffer          m_ringBuffer;

    tWSAOVERLAPPED_EX     m_overlapped;
    WSABUF              m_dataBuf;

public:
    Connection(uint32 _id, tAcceptedClient* _pAcceptedClient);
    ~Connection();

    void AddRef() { m_ref.fetch_add(1); }
    void Release()
    {
        m_ref.fetch_sub(1);
        if (m_ref <= 0)
        {
            delete this;
        }
    }

    uint32   GetId() const { return m_id; }
    SOCKET  GetSocket() const { return m_pAcceptedClient->clientSocket; }
    const int8* GetIP() const { return m_pAcceptedClient->ipAddr; }
    const uint8* GetPrivateIP() const { return m_pAcceptedClient->privateIPAddr; }
    uint16 GetUDPPort() const { return m_pAcceptedClient->udpAddr.sin_port; }

    void SetMyUDPPort(uint16 _port);
    void SetPrivateIP(uint8 _a, uint8 _b, uint8 _c, uint8 _d);

    void OnRecv(uint32 _recvBytes);

    bool RecvWSA();
    void Send(const Packet& _packet);
};