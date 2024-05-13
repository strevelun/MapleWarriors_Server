#include "ConnectionManager.h"
#include "../User/UserManager.h"

ConnectionManager* ConnectionManager::s_pInst = nullptr;

Connection* ConnectionManager::Create(tAcceptedClient* _pAcceptedClient)
{
	m_lock.Enter();
	Connection* pConn = new Connection(m_connectionId, _pAcceptedClient);
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
		printf("id[%d], socket[%d], IP[%s], Port[%d]		접속 종료됨		(현재 접속자 수 : %d)\n", _id, (int32)iter->second->GetSocket(), iter->second->GetIP(), iter->second->GetPort(), m_count);
		delete iter->second;
		m_mapConnection.erase(_id);
	}
	m_lock.Leave();
}

ConnectionManager::ConnectionManager() :
	m_connectionId(1), m_count(0)
{}

ConnectionManager::~ConnectionManager()
{}