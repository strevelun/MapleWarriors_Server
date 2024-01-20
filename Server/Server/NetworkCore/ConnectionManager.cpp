#include "ConnectionManager.h"

ConnectionManager* ConnectionManager::s_pInst = nullptr;

Connection* ConnectionManager::Create(SOCKET _socket)
{
	m_lock.Enter();
	Connection* pConn = new Connection(m_connectionId, _socket);
	pConn->SetSceneState(eSceneState::Login);
	m_mapConnection.insert({ m_connectionId, pConn });
	++m_connectionId;
	++m_count;
	m_lock.Leave();
	return pConn;
}

void ConnectionManager::Delete(uint32 _id)
{
	m_lock.Enter();
	std::unordered_map<uint32, Connection*>::iterator iter = m_mapConnection.find(_id);
	if (iter != m_mapConnection.cend())
	{
		--m_count;
		iter->second->Leave();
		delete iter->second;
		m_mapConnection.erase(_id);
	}
	m_lock.Leave();
	printf("현재 접속자 수 : %d\n", m_count);
}

ConnectionManager::ConnectionManager() :
	m_connectionId(1), m_count(0)
{
}

ConnectionManager::~ConnectionManager()
{
}
