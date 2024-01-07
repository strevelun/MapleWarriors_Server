#pragma once

#include "../Defines.h"
#include "../Types.h"
#include "../Packet/Packet.h"
#include "../Packet/PacketReader.h"
#include "RingBuffer.h"

class User;

class Connection 
{
private:
    int32           m_id;
    SOCKET          m_socket;
    RingBuffer      m_ringBuffer;
    User*           m_pUser;

    WSABUF          m_dataBuf;
    WSAOVERLAPPED   m_overlapped;

public:
    Connection(uint32 _id, SOCKET _socket);
    ~Connection();

    uint32   GetId() const { return m_id; }
    SOCKET  GetSocket() const { return m_socket; }
    User* GetUser() const { return m_pUser; }

    void SetUser(User* _pUser) { m_pUser = _pUser; }

    void OnRecv(uint32 _recvBytes);

    bool RecvWSA();
    void Send(const Packet& _packet);

    // Connect
    // Disconnect
};