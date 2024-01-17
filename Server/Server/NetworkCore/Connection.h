#pragma once

#include "../Defines.h"
#include "../Types.h"
#include "../Packet/Packet.h"
#include "../Packet/PacketReader.h"
#include "RingBuffer.h"
#include "../CSLock.h"

class User;
class Lobby;

class Connection
{
private:
    CSLock          m_lock;
    eSceneState     m_eSceneState;
    int32           m_id;
    uint32          m_lobbyID;
    SOCKET          m_socket;
    RingBuffer      m_ringBuffer;
    User* m_pUser;

    WSABUF          m_dataBuf;
    WSAOVERLAPPED   m_overlapped;

    bool            m_gonnaBeDeleted;

public:
    Connection(int32 _id, SOCKET _socket);
    ~Connection();

    int32   GetId() const { return m_id; }
    uint32 GetLobbyId() const { return m_lobbyID; }
    SOCKET  GetSocket() const { return m_socket; }
    User* GetUser() const { return m_pUser; }
    eSceneState GetSceneState() const { return m_eSceneState; }
    bool GonnaBeDeleted() const { return m_gonnaBeDeleted; }

    void SetUser(User* _pUser) { m_pUser = _pUser; }
    void SetSceneState(eSceneState _eState) { m_eSceneState = _eState; }
    void SetLobbyID(uint32 _id) { m_lobbyID = _id; }
    void SetDeleted() { m_gonnaBeDeleted = true; }

    void OnRecv(uint32 _recvBytes);

    bool RecvWSA();
    void Send(const Packet& _packet);

    void Leave();
};