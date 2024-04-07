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
    CSLock              m_lock;
    int32               m_id;
    tAcceptedClient*    m_pAcceptedClient;
    RingBuffer          m_ringBuffer;

    WSABUF              m_dataBuf;
    WSAOVERLAPPED       m_overlapped;

    bool                m_gonnaBeDeleted;

public:
    Connection(int32 _id, tAcceptedClient* _pAcceptedClient);
    ~Connection();

    int32   GetId() const { return m_id; }
    SOCKET  GetSocket() const { return m_pAcceptedClient->clientSocket; }
    const char* GetIP() const { return m_pAcceptedClient->ipAddr; }
    uint16 GetPort() const { return m_pAcceptedClient->port; }

    bool GonnaBeDeleted() const { return m_gonnaBeDeleted; }

    void SetDeleted() { m_gonnaBeDeleted = true; }

    void OnRecv(uint32 _recvBytes);

    bool RecvWSA();
    void Send(const Packet& _packet);
};