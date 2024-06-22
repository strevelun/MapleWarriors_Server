#pragma once

#include "../Defines.h"
#include "../Types.h"
#include "../Packet/Packet.h"
#include "../Packet/PacketReader.h"
#include "RingBuffer.h"
#include "../Lock/CSLock.h"

class Connection
{
private:
    uint32               m_id;
    tAcceptedClient*    m_pAcceptedClient;
    RingBuffer          m_ringBuffer;

    tWSAOVERLAPPED_EX     m_overlapped;
    WSABUF              m_dataBuf;

public:
    Connection(uint32 _id, tAcceptedClient* _pAcceptedClient);
    ~Connection();

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