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
    CSLock          m_lock;
    int32           m_id;
    SOCKET          m_socket;
    RingBuffer      m_ringBuffer;

    WSABUF          m_dataBuf;
    WSAOVERLAPPED   m_overlapped;

    bool            m_gonnaBeDeleted;

public:
    Connection(int32 _id, SOCKET _socket);
    ~Connection();

    int32   GetId() const { return m_id; }
    SOCKET  GetSocket() const { return m_socket; }
    bool GonnaBeDeleted() const { return m_gonnaBeDeleted; }

    void SetDeleted() { m_gonnaBeDeleted = true; }

    void OnRecv(uint32 _recvBytes);

    bool RecvWSA();
    void Send(const Packet& _packet);
};