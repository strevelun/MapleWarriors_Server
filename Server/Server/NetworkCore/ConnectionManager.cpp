#include "ConnectionManager.h"
#include "../User/UserManager.h"

ConnectionManager* ConnectionManager::s_pInst = nullptr;

Connection* ConnectionManager::Create(tAcceptedClient* _pAcceptedClient)
{
	Connection* pConn = new Connection(m_connectionId, _pAcceptedClient);
	m_lock.Enter();
	m_umapConnection.insert({ m_connectionId, { 1, pConn } });
	++m_connectionId;
	++m_count;
	m_lock.Leave();
	return pConn;
}

Connection* ConnectionManager::Get(uint32 _id)
{
	m_lock.Enter();
	std::unordered_map<uint32, _tConnection>::iterator iter = m_umapConnection.find(_id);
	if (iter == m_umapConnection.end())
	{
		m_lock.Leave();
		return nullptr;
	}
	++(iter->second.refCnt);
	Connection* pConn = iter->second.pConn;
	m_lock.Leave();
	return pConn;
}

void ConnectionManager::Release(uint32 _id)
{
	m_lock.Enter();
	std::unordered_map<uint32, _tConnection>::iterator iter = m_umapConnection.find(_id);	
	if (iter != m_umapConnection.end())
	{
		--(iter->second.refCnt);
		if (iter->second.refCnt < 1)
		{
			--m_count;
			printf("id[%u], socket[%d], IP[%s]		접속 종료됨		(현재 접속자 수 : %d)\n", _id, static_cast<int32>(iter->second.pConn->GetSocket()), iter->second.pConn->GetIP(), m_count);
			delete iter->second.pConn;
			m_umapConnection.erase(_id);
		}
	}
	m_lock.Leave();
}

ConnectionManager::ConnectionManager() :
	m_connectionId(1), m_count(0)
{}

ConnectionManager::~ConnectionManager()
{}