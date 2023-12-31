#pragma once

#include "../Defines.h"
#include "../Types.h"
#include "../Packet/Packet.h"
#include "RingBuffer.h"

class Connection 
{
private:
    int32 m_id;
    SOCKET m_socket;
    RingBuffer m_ringBuffer;

    WSABUF m_dataBuf;
    WSAOVERLAPPED m_overlapped;

public:
    Connection(uint32 _id, SOCKET _socket);
    ~Connection();

    int32   GetId() const { return m_id; }
    SOCKET  GetSocket() const { return m_socket; }

    void OnRecv(uint32 _recvBytes);

    bool RecvWSA();
    void Send(const Packet& _packet);
};